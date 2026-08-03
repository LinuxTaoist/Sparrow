#!/usr/bin/env python3
"""
CodecX-compatible protocol dump tool with a simple GUI.

Features:
- Input JSON config and protocol byte stream in hex text.
- Parse by CodecX-like rules: static_field, dynamic_field, len_ref, len_mode, len_formula.
- Supports relative/absolute/fixed len_ref.
- Supports atom types: u8/u16/u32/u64/s8/s16/s32/s64/leb128.
- Optional frame split by root child head_flag.
- Error-tolerant: shows partial results with error markers and warning popup.

This script uses only Python standard library so it can run on Windows directly.
"""

from __future__ import annotations

import ast
import copy
import json
import re
from datetime import datetime
from dataclasses import dataclass, field
from typing import Any, Dict, List, Optional, Tuple

try:
    import tkinter as tk
    from tkinter import font as tkfont
    from tkinter import filedialog, messagebox, ttk
    TK_AVAILABLE = True
except Exception:
    tk = None
    tkfont = None
    filedialog = None
    messagebox = None
    ttk = None
    TK_AVAILABLE = False


ATOM_FIXED_SIZE = {
    "u8": 1,
    "s8": 1,
    "u16": 2,
    "s16": 2,
    "u32": 4,
    "s32": 4,
    "u64": 8,
    "s64": 8,
}

ATOM_TYPES = ["u8", "u16", "u32", "u64", "s8", "s16", "s32", "s64", "leb128"]
LEN_MODES = ["count", "bytes", "bit", "condition"]


CONFIG_GUIDE_TEXT = """Propshow 配置规则说明

1. 根节点
- 根节点必须是 JSON 对象。
- 必须包含: name, type。
- type 建议使用 static_field。

2. 基础字段(原子类型)
- 支持: u8/u16/u32/u64/s8/s16/s32/s64/leb128。
- 示例:
    {"name": "msg_id", "type": "u16"}

3. static_field(固定结构)
- 用 children 定义固定顺序子字段。
- 示例:
    {
        "name": "header",
        "type": "static_field",
        "children": [
            {"name": "ver", "type": "u8"},
            {"name": "len", "type": "u16"}
        ]
    }

4. dynamic_field(动态结构)
- 必填: len_ref, len_mode, child_template。
- child_template 是单个模板，会按长度展开。
- 示例:
    {
        "name": "payload",
        "type": "dynamic_field",
        "len_ref": "../len",
        "len_mode": "bytes",
        "child_template": {"name": "byte", "type": "u8"}
    }

5. len_ref 规则
- fixed_N: 固定长度，例如 fixed_8。
- 相对路径: ../field, ../../x/y。
- 绝对路径: /root/field 或 /field。

6. len_mode 规则
- count: 按元素个数展开。
- bytes: 按字节长度展开。
- bit: 先把位数换算成字节数 (len + 7) // 8。
- condition: len_ref 为 0 不解析，非 0 解析 1 次。

7. len_formula 规则
- 可用变量: len_ref, curpos。
- 可用运算: + - * / ()。
- 特殊反转: "!"。
- 示例:
    "len_formula": "len_ref - curpos"

8. head_flag 分帧
- 根节点 children 下若有字段名 head_flag，程序可按它自动分帧。
- head_flag 的 value 可以是数字或十六进制字符串。

9. 常见错误排查
- dynamic_field 缺少 child_template。
- len_ref 路径写错，引用不到字段。
- bytes 长度不足，导致越界解析。
- 配置合法但类型与协议字节不匹配。

10. 推荐修改流程
1) 先改配置 JSON。
2) 点“解析”查看协议视图。
3) 必要时导出配置/字节流/协议视图做比对。
"""


@dataclass
class ParsedNode:
    name: str
    node_type: str
    schema: Dict[str, Any]
    parent: Optional["ParsedNode"]
    start: int
    end: int = -1
    value: bytes = b""
    children: List["ParsedNode"] = field(default_factory=list)
    sibling_index: int = 0
    error: bool = False
    error_msg: str = ""

    def add_child(self, child: "ParsedNode") -> None:
        same = sum(1 for x in self.children if x.name == child.name)
        child.sibling_index = same
        self.children.append(child)

    def display_name(self) -> str:
        if self.parent is None:
            return self.name
        same_name_cnt = sum(1 for x in self.parent.children if x.name == self.name)
        if same_name_cnt > 1:
            return f"{self.name}[{self.sibling_index}]"
        return self.name

    def path(self) -> str:
        nodes: List[ParsedNode] = []
        cur: Optional[ParsedNode] = self
        while cur is not None:
            nodes.append(cur)
            cur = cur.parent
        nodes.reverse()
        return "/" + "/".join(n.display_name() for n in nodes)


@dataclass
class FieldRow:
    path: str
    node_type: str
    offset: str
    hex_value: str
    int_value: str


class ParseError(Exception):
    def __init__(self, message: str, line: Optional[int] = None):
        super().__init__(message)
        self.line = line


class ExprEvaluator(ast.NodeVisitor):
    def visit_Expression(self, node: ast.Expression) -> int:
        return self.visit(node.body)

    def visit_BinOp(self, node: ast.BinOp) -> int:
        left = self.visit(node.left)
        right = self.visit(node.right)
        if isinstance(node.op, ast.Add):
            return left + right
        if isinstance(node.op, ast.Sub):
            return left - right
        if isinstance(node.op, ast.Mult):
            return left * right
        if isinstance(node.op, ast.Div):
            if right == 0:
                raise ParseError("len_formula divide by zero")
            return int(left / right)
        if isinstance(node.op, ast.FloorDiv):
            if right == 0:
                raise ParseError("len_formula divide by zero")
            return int(left / right)
        raise ParseError("len_formula contains unsupported operator")

    def visit_UnaryOp(self, node: ast.UnaryOp) -> int:
        val = self.visit(node.operand)
        if isinstance(node.op, ast.USub):
            return -val
        if isinstance(node.op, ast.UAdd):
            return +val
        raise ParseError("len_formula contains unsupported unary operator")

    def visit_Constant(self, node: ast.Constant) -> int:
        if not isinstance(node.value, int):
            raise ParseError("len_formula only supports integer numbers")
        return node.value

    def generic_visit(self, node: ast.AST) -> int:
        raise ParseError("len_formula contains unsupported syntax")


class CodecXDecoder:
    def __init__(self, schema: Dict[str, Any], payload: bytes):
        self.schema = schema
        self.payload = payload
        self.rows: List[FieldRow] = []

    def decode_one(self, payload: Optional[bytes] = None) -> Tuple[ParsedNode, int, List[FieldRow]]:
        data = payload if payload is not None else self.payload
        self.rows = []
        try:
            root = self._decode_node(self.schema, None, data, 0)
        except ParseError as exc:
            root = ParsedNode(name="<root_error>", node_type="error", schema={}, parent=None, start=0)
            root.error = True
            root.error_msg = str(exc)
            root.end = -1
        consumed = root.end + 1 if root.end >= root.start else root.start
        return root, consumed, self.rows

    def _decode_node(
        self,
        schema: Dict[str, Any],
        parent: Optional[ParsedNode],
        data: bytes,
        offset: int,
    ) -> ParsedNode:
        node_type = schema.get("type", "")
        name = schema.get("name", "")
        node = ParsedNode(name=name, node_type=node_type, schema=schema, parent=parent, start=offset)

        if parent is not None:
            parent.add_child(node)

        if node_type in ("static_field", "dynamic_field"):
            if node_type == "static_field":
                children = schema.get("children", [])
                if not isinstance(children, list):
                    raise ParseError(f"{node.path()} children must be a list")
                cur = offset
                for child_schema in children:
                    child = self._decode_node(child_schema, node, data, cur)
                    if child.error and child.end < child.start:
                        node.error = True
                        node.error_msg = f"子字段 '{child_schema.get('name')}' 解析失败，停止后续解析"
                        break
                    cur = child.end + 1 if child.end >= child.start else child.start
                node.end = cur - 1 if cur > offset else offset - 1
            else:
                template = schema.get("child_template")
                if not isinstance(template, dict):
                    node.error = True
                    node.error_msg = "dynamic_field 缺少 child_template"
                    node.end = offset - 1
                    return node

                try:
                    count = self._calc_dynamic_len(node, data, offset)
                except ParseError as exc:
                    node.error = True
                    node.error_msg = f"动态长度计算失败: {exc}"
                    node.end = offset - 1
                    return node

                cur = offset
                for _ in range(count):
                    child = self._decode_node(template, node, data, cur)
                    if child.error and child.end < child.start:
                        node.error = True
                        node.error_msg = f"子元素解析失败: {child.error_msg}"
                        break
                    cur = child.end + 1 if child.end >= child.start else child.start
                node.end = cur - 1 if cur > offset else offset - 1

            self.rows.append(
                FieldRow(
                    path=node.path(),
                    node_type=node.node_type,
                    offset=f"{node.start}-{node.end}" if node.end >= node.start else f"{node.start}",
                    hex_value="",
                    int_value="",
                )
            )
            return node

        try:
            value, size = self._decode_atom(node_type, data, offset, node.path())
            node.value = value
            node.end = offset + size - 1
        except ParseError as exc:
            node.error = True
            node.error_msg = str(exc)
            node.end = offset - 1
            value = b""

        int_text = self._to_int_text(node_type, node.value) if not node.error else ""
        self.rows.append(
            FieldRow(
                path=node.path(),
                node_type=node_type,
                offset=f"{node.start}-{node.end}" if not node.error else f"{node.start}(ERR)",
                hex_value=_bytes_to_hex(node.value) if not node.error else "",
                int_value=int_text,
            )
        )
        return node

    def _decode_atom(self, node_type: str, data: bytes, offset: int, path: str) -> Tuple[bytes, int]:
        if node_type == "leb128":
            return self._decode_leb128(data, offset, path)

        if node_type not in ATOM_FIXED_SIZE:
            raise ParseError(f"{path} unsupported atom type: {node_type}")

        size = ATOM_FIXED_SIZE[node_type]
        end = offset + size
        if end > len(data):
            raise ParseError(f"{path} out of range, need {size} bytes at offset {offset}, total {len(data)}")
        return data[offset:end], size

    def _decode_leb128(self, data: bytes, offset: int, path: str) -> Tuple[bytes, int]:
        cur = offset
        out = bytearray()
        for _ in range(10):
            if cur >= len(data):
                raise ParseError(f"{path} invalid leb128 at offset {offset}")
            b = data[cur]
            out.append(b)
            cur += 1
            if (b & 0x80) == 0:
                return bytes(out), len(out)
        raise ParseError(f"{path} leb128 too long at offset {offset}")

    def _to_int_text(self, node_type: str, value: bytes) -> str:
        if not value:
            return ""
        if node_type == "leb128":
            v = 0
            shift = 0
            for b in value:
                v |= (b & 0x7F) << shift
                shift += 7
            return str(v)

        signed = node_type.startswith("s")
        return str(int.from_bytes(value, byteorder="big", signed=signed))

    def _calc_dynamic_len(self, dynamic_node: ParsedNode, data: bytes, curpos: int) -> int:
        schema = dynamic_node.schema
        len_ref = schema.get("len_ref", "")
        len_mode = schema.get("len_mode", "")
        len_formula = schema.get("len_formula", "")

        if not isinstance(len_ref, str) or not len_ref:
            raise ParseError(f"{dynamic_node.path()} len_ref is required")

        if len_ref.startswith("fixed_"):
            try:
                length = int(len_ref[len("fixed_"):])
            except ValueError as exc:
                raise ParseError(f"{dynamic_node.path()} invalid fixed len_ref: {len_ref}") from exc
        else:
            ref_node = self._resolve_path(dynamic_node, len_ref)
            if ref_node is None:
                raise ParseError(f"{dynamic_node.path()} len_ref target not found: {len_ref}")

            if ref_node.node_type in ("static_field", "dynamic_field"):
                total = 0
                for child in ref_node.children:
                    if child.node_type in ("static_field", "dynamic_field"):
                        continue
                    total += self._atom_to_int(child)
                length = total
            else:
                length = self._atom_to_int(ref_node)

        if len_mode == "bit":
            length = (length + 7) // 8
        elif len_mode == "condition":
            length = 0 if length == 0 else 1

        if isinstance(len_formula, str) and len_formula:
            if len_formula == "!":
                length = 0 if length else 1
            else:
                expr = len_formula.replace("curpos", str(curpos)).replace("len_ref", str(length))
                try:
                    parsed = ast.parse(expr, mode="eval")
                    length = ExprEvaluator().visit(parsed)
                except ParseError:
                    raise
                except Exception as exc:
                    raise ParseError(f"{dynamic_node.path()} invalid len_formula: {len_formula}") from exc

        if length < 0:
            raise ParseError(f"{dynamic_node.path()} dynamic length < 0")
        return length

    def _atom_to_int(self, node: ParsedNode) -> int:
        if node.node_type == "leb128":
            v = 0
            shift = 0
            for b in node.value:
                v |= (b & 0x7F) << shift
                shift += 7
            return v
        if node.node_type.startswith("s"):
            return int.from_bytes(node.value, byteorder="big", signed=True)
        return int.from_bytes(node.value, byteorder="big", signed=False)

    def _resolve_path(self, current_node: ParsedNode, path: str) -> Optional[ParsedNode]:
        parts = [p for p in path.split("/") if p and p != "."]
        if not parts:
            return None

        if path.startswith("/"):
            root = current_node
            while root.parent is not None:
                root = root.parent

            idx = 0
            if parts and parts[0] == root.name:
                idx = 1
            node: ParsedNode = root
            while idx < len(parts):
                node = self._get_child_node(node, parts[idx], current_node)
                if node is None:
                    return None
                idx += 1
            return node

        node: Optional[ParsedNode] = None
        for part in parts:
            if part == "..":
                node = node.parent if node is not None else current_node.parent
            elif node is None:
                node = self._get_child_node(current_node, part, current_node)
            else:
                node = self._get_child_node(node, part, current_node)

            if node is None:
                return None

        return node

    def _get_child_node(self, field_node: ParsedNode, child_name: str, context_node: ParsedNode) -> Optional[ParsedNode]:
        if field_node.node_type != "dynamic_field":
            for child in field_node.children:
                if child.name == child_name:
                    return child
            return None

        branch = context_node
        while branch is not None and branch.parent is not field_node:
            branch = branch.parent

        if branch is not None and branch.name == child_name:
            return branch

        for child in field_node.children:
            if child.name == child_name:
                return child
        return None


def _bytes_to_hex(data: bytes) -> str:
    return " ".join(f"{b:02X}" for b in data)


def parse_hex_input(text: str) -> bytes:
    raw = text.strip()
    if not raw:
        return b""

    cleaned = raw.replace("\n", " ").replace("\t", " ").replace(",", " ")
    cleaned = re.sub(r"0x", "", cleaned, flags=re.IGNORECASE)
    tokens = [t for t in cleaned.split(" ") if t]

    if len(tokens) == 1 and re.fullmatch(r"[0-9a-fA-F]+", tokens[0]) and len(tokens[0]) % 2 == 0:
        s = tokens[0]
        return bytes(int(s[i:i + 2], 16) for i in range(0, len(s), 2))

    out = bytearray()
    for t in tokens:
        if not re.fullmatch(r"[0-9a-fA-F]{1,2}", t):
            raise ParseError(f"非法字节: {t}")
        out.append(int(t, 16))
    return bytes(out)


def parse_config_json(text: str) -> Dict[str, Any]:
    if not text.strip():
        raise ParseError("请输入配置 JSON")
    try:
        cfg = json.loads(text)
    except json.JSONDecodeError as exc:
        raise ParseError(f"配置 JSON 解析失败: {exc.msg}", line=exc.lineno) from exc

    if not isinstance(cfg, dict):
        raise ParseError("配置根节点必须是 JSON 对象")
    if cfg.get("type") not in ("static_field", "dynamic_field"):
        raise ParseError("配置根节点 type 必须是 static_field 或 dynamic_field")
    if "name" not in cfg:
        raise ParseError("配置根节点缺少 name")
    return cfg


def maybe_build_head_flag_bytes(cfg: Dict[str, Any]) -> Optional[bytes]:
    children = cfg.get("children")
    if not isinstance(children, list):
        return None

    for c in children:
        if not isinstance(c, dict):
            continue
        if c.get("name") != "head_flag":
            continue
        node_type = c.get("type")
        value = c.get("value")
        if node_type not in ATOM_FIXED_SIZE:
            return None
        size = ATOM_FIXED_SIZE[node_type]
        if value is None:
            return None

        if isinstance(value, int):
            signed = node_type.startswith("s")
            try:
                return int(value).to_bytes(size, "big", signed=signed)
            except OverflowError:
                return None

        if isinstance(value, str):
            vv = value.strip()
            vv = re.sub(r"\s+", "", vv)
            vv = re.sub(r"^0x", "", vv, flags=re.IGNORECASE)
            if re.fullmatch(r"[0-9a-fA-F]+", vv):
                if len(vv) % 2 == 1:
                    vv = "0" + vv
                raw = bytes.fromhex(vv)
                if len(raw) == size:
                    return raw
                if len(raw) < size:
                    return (b"\x00" * (size - len(raw))) + raw
                return raw[-size:]

            raw = value.encode("utf-8", errors="ignore")
            if len(raw) == size:
                return raw
            if len(raw) < size:
                return (b"\x00" * (size - len(raw))) + raw
            return raw[-size:]

    return None


def _strip_schema_meta(node: Dict[str, Any]) -> Dict[str, Any]:
    out: Dict[str, Any] = {}
    for key, val in node.items():
        if key.startswith("__"):
            continue
        if key == "children" and isinstance(val, list):
            out[key] = [_strip_schema_meta(v) for v in val if isinstance(v, dict)]
            continue
        if key == "child_template" and isinstance(val, dict):
            out[key] = _strip_schema_meta(val)
            continue
        out[key] = val
    return out


class VisualSchemaBuilder:
    def __init__(self, parent: "PropShowApp"):
        self.parent = parent
        self.win = tk.Toplevel(parent.root)
        self.win.title("可视化配置面板")
        self.win.geometry("1120x760")
        self.win.minsize(960, 620)
        self.win.configure(bg="#eef2f7")

        self.schema: Dict[str, Any] = self._default_root()
        self._uid_seed = 0
        self._uid_to_node: Dict[str, Dict[str, Any]] = {}
        self._updating_props = False
        self.current_file_path: str = ""
        self._lt_two_rows: Optional[bool] = None

        self.name_var = tk.StringVar(value="")
        self.type_var = tk.StringVar(value="u8")
        self.value_var = tk.StringVar(value="")
        self.len_ref_var = tk.StringVar(value="")
        self.len_mode_var = tk.StringVar(value="bytes")
        self.len_formula_var = tk.StringVar(value="")
        self.editor_status_var = tk.StringVar(value="点击字段即可编辑属性")

        self._build_ui()
        self.load_from_text(self.parent.config_text.get("1.0", tk.END), silent=True)
        self.win.protocol("WM_DELETE_WINDOW", self._on_close)

    def _default_root(self) -> Dict[str, Any]:
        return {
            "name": "root",
            "type": "static_field",
            "children": [],
        }

    def _on_close(self) -> None:
        self.parent.visual_builder = None
        self.win.destroy()

    def focus(self) -> None:
        self.win.deiconify()
        self.win.lift()
        self.win.focus_force()

    def _build_ui(self) -> None:
        # ── 主体：水平分割面板 ────────────────────────────────────────
        body = ttk.Panedwindow(self.win, orient=tk.HORIZONTAL)
        body.pack(fill=tk.BOTH, expand=True, padx=10, pady=(10, 8))

        # ── 左侧：配置/文件工具栏 + 协议结构树 ───────────────────────
        left_outer = ttk.Frame(body, style="App.TFrame")
        body.add(left_outer, weight=3)

        # 自动换行的配置工具栏（宽度不足时折成两行）
        self._left_toolbar_frame = ttk.Frame(left_outer, style="App.TFrame")
        self._left_toolbar_frame.pack(fill=tk.X, pady=(0, 6))
        self._left_btns: List[ttk.Button] = []
        for _text, _cmd, _style in [
            ("加载配置文件",   self.load_from_file,   "Toolbar.TButton"),
            ("从解析配置加载", self.load_from_parent, "Toolbar.TButton"),
            ("重置为空协议",   self.reset_root,       "Toolbar.TButton"),
        ]:
            self._left_btns.append(
                ttk.Button(self._left_toolbar_frame, text=_text, command=_cmd, style=_style)
            )
        self._left_toolbar_frame.bind("<Configure>", self._reflow_left_toolbar)
        self.win.after(80, self._reflow_left_toolbar)

        # 协议结构树
        left_lf = ttk.Labelframe(left_outer, text="协议结构", style="TLabelframe")
        left_lf.pack(fill=tk.BOTH, expand=True)
        self.schema_tree = ttk.Treeview(left_lf, columns=("type",), show="tree headings")
        self.schema_tree.heading("#0", text="字段名", anchor=tk.W)
        self.schema_tree.heading("type", text="类型", anchor=tk.W)
        self.schema_tree.column("#0", width=260, anchor=tk.W)
        self.schema_tree.column("type", width=140, anchor=tk.W)
        self.schema_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(6, 0), pady=6)
        left_scroll = ttk.Scrollbar(left_lf, orient=tk.VERTICAL, command=self.schema_tree.yview)
        self.schema_tree.configure(yscrollcommand=left_scroll.set)
        left_scroll.pack(side=tk.RIGHT, fill=tk.Y, padx=(0, 6), pady=6)
        self.schema_tree.bind("<<TreeviewSelect>>", self._on_tree_select)

        # ── 右侧：字段操作工具栏 + 属性面板 + JSON 预览 ──────────────
        right_outer = ttk.Frame(body, style="App.TFrame")
        body.add(right_outer, weight=2)

        # 字段编辑工具栏（始终左对齐，不换行）
        edit_toolbar = ttk.Frame(right_outer, style="App.TFrame")
        edit_toolbar.pack(fill=tk.X, pady=(0, 6), anchor=tk.W)
        for _text, _cmd in [
            ("添加子字段",   self.add_child),
            ("添加兄弟字段", self.add_sibling),
            ("删除字段",     self.delete_node),
        ]:
            ttk.Button(edit_toolbar, text=_text, command=_cmd, style="Toolbar.TButton").pack(side=tk.LEFT, padx=(0, 4), pady=2)
        ttk.Separator(edit_toolbar, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=8)
        ttk.Button(edit_toolbar, text="上移", command=lambda: self.move_node(-1), style="Toolbar.TButton").pack(side=tk.LEFT, padx=(0, 4), pady=2)
        ttk.Button(edit_toolbar, text="下移", command=lambda: self.move_node(1),  style="Toolbar.TButton").pack(side=tk.LEFT, pady=2)

        # 垂直分割：字段属性 + JSON 预览
        right_paned = ttk.Panedwindow(right_outer, orient=tk.VERTICAL)
        right_paned.pack(fill=tk.BOTH, expand=True)
        prop_panel = ttk.Labelframe(right_paned, text="字段属性", style="TLabelframe")
        right_paned.add(prop_panel, weight=1)
        self._build_property_panel(prop_panel)
        preview_panel = ttk.Labelframe(right_paned, text="实时 JSON 预览", style="TLabelframe")
        right_paned.add(preview_panel, weight=2)
        self.preview_text = tk.Text(preview_panel, wrap=tk.NONE, font=("Consolas", 10))
        self.preview_text.pack(fill=tk.BOTH, expand=True, padx=6, pady=6)
        self.preview_text.configure(background="#fbfcfe", foreground="#102a43", relief=tk.FLAT)

        # ── 底部：状态条 + 保存/应用操作按鈕 ────────────────────────────
        bottom = ttk.Frame(self.win, style="App.TFrame")
        bottom.pack(fill=tk.X, padx=10, pady=(0, 10))
        ttk.Separator(bottom, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=(0, 6))
        bottom_row = ttk.Frame(bottom, style="App.TFrame")
        bottom_row.pack(fill=tk.X)
        ttk.Label(bottom_row, textvariable=self.editor_status_var, style="Status.TLabel", anchor=tk.W).pack(side=tk.LEFT, fill=tk.X, expand=True)
        ttk.Button(bottom_row, text="应用到解析配置", command=self.apply_to_parent, style="Primary.TButton").pack(side=tk.RIGHT)
        ttk.Separator(bottom_row, orient=tk.VERTICAL).pack(side=tk.RIGHT, fill=tk.Y, padx=8)
        ttk.Button(bottom_row, text="另存为", command=self.save_as_file, style="Toolbar.TButton").pack(side=tk.RIGHT)
        ttk.Button(bottom_row, text="保存", command=self.save_to_file, style="Toolbar.TButton").pack(side=tk.RIGHT, padx=(0, 6))

    def _reflow_left_toolbar(self, event=None) -> None:
        """在宽度不足时将左侧配置工具栏折叠为两行，否则保持单行。"""
        if not hasattr(self, "_left_btns") or not self._left_btns:
            return

        frame = self._left_toolbar_frame
        frame.update_idletasks()
        avail_w = frame.winfo_width()
        if avail_w <= 1:
            # 窗口尚未完全渲染，稍后重试
            self.win.after(80, self._reflow_left_toolbar)
            return

        for btn in self._left_btns:
            btn.update_idletasks()

        gap = 4
        total_w = sum(btn.winfo_reqwidth() for btn in self._left_btns) + gap * len(self._left_btns)
        want_two = total_w > avail_w

        if want_two == self._lt_two_rows:
            return  # 布局无需变化

        self._lt_two_rows = want_two

        # 清除旧布局
        for btn in self._left_btns:
            btn.grid_forget()

        if not want_two:
            # 单行：全部排在第 0 行
            for col, btn in enumerate(self._left_btns):
                btn.grid(row=0, column=col, padx=(0, gap), pady=2, sticky="w")
        else:
            # 两行：前 2 个在第 0 行，第 3 个在第 1 行
            for col, btn in enumerate(self._left_btns[:2]):
                btn.grid(row=0, column=col, padx=(0, gap), pady=2, sticky="w")
            for col, btn in enumerate(self._left_btns[2:]):
                btn.grid(row=1, column=col, padx=(0, gap), pady=(4, 2), sticky="w")

    def _build_property_panel(self, parent: ttk.Labelframe) -> None:
        wrap = ttk.Frame(parent, style="Panel.TFrame")
        wrap.pack(fill=tk.BOTH, expand=True, padx=8, pady=8)

        for i in range(2):
            wrap.columnconfigure(i, weight=1 if i == 1 else 0)

        row = 0
        ttk.Label(wrap, text="name", style="Status.TLabel").grid(row=row, column=0, sticky="w", pady=4)
        self.name_entry = ttk.Entry(wrap, textvariable=self.name_var)
        self.name_entry.grid(row=row, column=1, sticky="ew", pady=4)

        row += 1
        ttk.Label(wrap, text="type", style="Status.TLabel").grid(row=row, column=0, sticky="w", pady=4)
        self.type_combo = ttk.Combobox(
            wrap,
            textvariable=self.type_var,
            values=["static_field", "dynamic_field"] + ATOM_TYPES,
            state="readonly",
        )
        self.type_combo.grid(row=row, column=1, sticky="ew", pady=4)

        row += 1
        self.value_label = ttk.Label(wrap, text="value(可选)", style="Status.TLabel")
        self.value_label.grid(row=row, column=0, sticky="w", pady=4)
        self.value_entry = ttk.Entry(wrap, textvariable=self.value_var)
        self.value_entry.grid(row=row, column=1, sticky="ew", pady=4)

        row += 1
        self.len_ref_label = ttk.Label(wrap, text="len_ref", style="Status.TLabel")
        self.len_ref_label.grid(row=row, column=0, sticky="w", pady=4)
        self.len_ref_entry = ttk.Entry(wrap, textvariable=self.len_ref_var)
        self.len_ref_entry.grid(row=row, column=1, sticky="ew", pady=4)

        row += 1
        self.len_mode_label = ttk.Label(wrap, text="len_mode", style="Status.TLabel")
        self.len_mode_label.grid(row=row, column=0, sticky="w", pady=4)
        self.len_mode_combo = ttk.Combobox(wrap, textvariable=self.len_mode_var, values=LEN_MODES, state="readonly")
        self.len_mode_combo.grid(row=row, column=1, sticky="ew", pady=4)

        row += 1
        self.len_formula_label = ttk.Label(wrap, text="len_formula", style="Status.TLabel")
        self.len_formula_label.grid(row=row, column=0, sticky="w", pady=4)
        self.len_formula_entry = ttk.Entry(wrap, textvariable=self.len_formula_var)
        self.len_formula_entry.grid(row=row, column=1, sticky="ew", pady=4)

        row += 1
        ttk.Button(wrap, text="应用属性", command=self._apply_properties_to_selected, style="Toolbar.TButton").grid(row=row, column=1, sticky="e", pady=(8, 0))

        self.name_entry.bind("<KeyRelease>", lambda _e: self._apply_properties_to_selected())
        self.value_entry.bind("<KeyRelease>", lambda _e: self._apply_properties_to_selected())
        self.len_ref_entry.bind("<KeyRelease>", lambda _e: self._apply_properties_to_selected())
        self.len_formula_entry.bind("<KeyRelease>", lambda _e: self._apply_properties_to_selected())
        self.type_combo.bind("<<ComboboxSelected>>", lambda _e: self._apply_properties_to_selected())
        self.len_mode_combo.bind("<<ComboboxSelected>>", lambda _e: self._apply_properties_to_selected())

    def _assign_uid_recursive(self, node: Dict[str, Any]) -> None:
        if "__uid" not in node:
            self._uid_seed += 1
            node["__uid"] = f"n{self._uid_seed}"

        children = node.get("children", [])
        if isinstance(children, list):
            for c in children:
                if isinstance(c, dict):
                    self._assign_uid_recursive(c)

        template = node.get("child_template")
        if isinstance(template, dict):
            self._assign_uid_recursive(template)

    def _selected_uid(self) -> Optional[str]:
        selected = self.schema_tree.selection()
        return selected[0] if selected else None

    def _find_node(self, uid: str, cur: Optional[Dict[str, Any]] = None) -> Optional[Dict[str, Any]]:
        node = cur if cur is not None else self.schema
        if node.get("__uid") == uid:
            return node

        children = node.get("children", [])
        if isinstance(children, list):
            for c in children:
                if isinstance(c, dict):
                    found = self._find_node(uid, c)
                    if found is not None:
                        return found

        template = node.get("child_template")
        if isinstance(template, dict):
            found = self._find_node(uid, template)
            if found is not None:
                return found
        return None

    def _find_parent(
        self,
        uid: str,
        cur: Optional[Dict[str, Any]] = None,
    ) -> Tuple[Optional[Dict[str, Any]], Optional[str], int]:
        node = cur if cur is not None else self.schema

        children = node.get("children", [])
        if isinstance(children, list):
            for idx, c in enumerate(children):
                if isinstance(c, dict) and c.get("__uid") == uid:
                    return node, "children", idx
                if isinstance(c, dict):
                    p, rel, i = self._find_parent(uid, c)
                    if p is not None:
                        return p, rel, i

        template = node.get("child_template")
        if isinstance(template, dict):
            if template.get("__uid") == uid:
                return node, "child_template", 0
            p, rel, i = self._find_parent(uid, template)
            if p is not None:
                return p, rel, i

        return None, None, -1

    def _new_atom_node(self, name: str = "field", node_type: str = "u8") -> Dict[str, Any]:
        self._uid_seed += 1
        return {
            "__uid": f"n{self._uid_seed}",
            "name": name,
            "type": node_type,
        }

    def _refresh_tree(self, select_uid: Optional[str] = None) -> None:
        self._uid_to_node.clear()
        for item in self.schema_tree.get_children():
            self.schema_tree.delete(item)

        def insert_rec(parent_item: str, node: Dict[str, Any], is_template: bool = False) -> None:
            uid = node.get("__uid", "")
            if not uid:
                return
            self._uid_to_node[uid] = node
            name = node.get("name", "")
            label = f"[template] {name}" if is_template else name
            item = self.schema_tree.insert(parent_item, tk.END, iid=uid, text=label, values=(node.get("type", ""),), open=True)

            children = node.get("children", [])
            if isinstance(children, list):
                for c in children:
                    if isinstance(c, dict):
                        insert_rec(item, c, False)

            template = node.get("child_template")
            if isinstance(template, dict):
                insert_rec(item, template, True)

        insert_rec("", self.schema)

        chosen = select_uid if select_uid and self.schema_tree.exists(select_uid) else self.schema.get("__uid")
        if chosen and self.schema_tree.exists(chosen):
            self.schema_tree.selection_set(chosen)
            self.schema_tree.focus(chosen)
            self.schema_tree.see(chosen)

        self._refresh_preview()

    def _refresh_preview(self) -> None:
        data = _strip_schema_meta(self.schema)
        text = json.dumps(data, indent=4, ensure_ascii=False)
        self.preview_text.configure(state=tk.NORMAL)
        self.preview_text.delete("1.0", tk.END)
        self.preview_text.insert("1.0", text)
        self.preview_text.configure(state=tk.DISABLED)

    def _on_tree_select(self, _event=None) -> None:
        uid = self._selected_uid()
        if not uid:
            return
        node = self._find_node(uid)
        if not node:
            return

        self._updating_props = True
        self.name_var.set(str(node.get("name", "")))
        self.type_var.set(str(node.get("type", "u8")))
        self.value_var.set(str(node.get("value", "")) if "value" in node else "")
        self.len_ref_var.set(str(node.get("len_ref", "")))
        self.len_mode_var.set(str(node.get("len_mode", "bytes")))
        self.len_formula_var.set(str(node.get("len_formula", "")) if "len_formula" in node else "")
        self._updating_props = False

        self._refresh_property_visibility(node)

    def _refresh_property_visibility(self, node: Dict[str, Any]) -> None:
        node_type = node.get("type", "")
        is_dynamic = node_type == "dynamic_field"
        is_atom = node_type in ATOM_TYPES

        def set_row(widget: Any, show: bool) -> None:
            if show:
                widget.grid()
            else:
                widget.grid_remove()

        set_row(self.value_label, is_atom)
        set_row(self.value_entry, is_atom)
        set_row(self.len_ref_label, is_dynamic)
        set_row(self.len_ref_entry, is_dynamic)
        set_row(self.len_mode_label, is_dynamic)
        set_row(self.len_mode_combo, is_dynamic)
        set_row(self.len_formula_label, is_dynamic)
        set_row(self.len_formula_entry, is_dynamic)

    def _apply_properties_to_selected(self) -> None:
        if self._updating_props:
            return

        uid = self._selected_uid()
        if not uid:
            return
        node = self._find_node(uid)
        if not node:
            return

        node["name"] = self.name_var.get().strip() or "field"
        new_type = self.type_var.get().strip() or "u8"
        old_type = node.get("type", "")
        node["type"] = new_type

        if new_type == "static_field":
            node.pop("child_template", None)
            node.pop("len_ref", None)
            node.pop("len_mode", None)
            node.pop("len_formula", None)
            node.pop("value", None)
            if not isinstance(node.get("children"), list):
                node["children"] = []

        elif new_type == "dynamic_field":
            node.pop("children", None)
            node.pop("value", None)
            node["len_ref"] = self.len_ref_var.get().strip() or "fixed_1"
            node["len_mode"] = self.len_mode_var.get().strip() or "bytes"
            if self.len_formula_var.get().strip():
                node["len_formula"] = self.len_formula_var.get().strip()
            else:
                node.pop("len_formula", None)

            if not isinstance(node.get("child_template"), dict):
                node["child_template"] = self._new_atom_node("item", "u8")

        else:
            node.pop("children", None)
            node.pop("child_template", None)
            node.pop("len_ref", None)
            node.pop("len_mode", None)
            node.pop("len_formula", None)
            value_text = self.value_var.get().strip()
            if value_text:
                node["value"] = value_text
            else:
                node.pop("value", None)

        # 从 dynamic 切换回原子/静态时，避免属性残留
        if old_type == "dynamic_field" and new_type != "dynamic_field":
            node.pop("len_ref", None)
            node.pop("len_mode", None)
            node.pop("len_formula", None)

        self._refresh_tree(select_uid=uid)
        self._on_tree_select()

    def add_child(self) -> None:
        uid = self._selected_uid()
        node = self._find_node(uid) if uid else self.schema
        if not node:
            return

        node_type = node.get("type", "")
        new_node = self._new_atom_node("field", "u8")

        if node_type == "static_field":
            children = node.setdefault("children", [])
            if isinstance(children, list):
                children.append(new_node)
        elif node_type == "dynamic_field":
            node["child_template"] = new_node
        else:
            messagebox.showinfo("提示", "当前字段不是容器类型，请先把 type 改为 static_field 或 dynamic_field")
            return

        self._refresh_tree(select_uid=new_node.get("__uid"))
        self.editor_status_var.set("已添加子字段")

    def add_sibling(self) -> None:
        uid = self._selected_uid()
        if not uid:
            return

        parent, relation, idx = self._find_parent(uid)
        if parent is None or relation is None:
            messagebox.showinfo("提示", "根节点不能添加兄弟字段")
            return

        if relation != "children":
            messagebox.showinfo("提示", "模板字段不能添加兄弟，请在父节点下操作")
            return

        children = parent.get("children", [])
        if not isinstance(children, list):
            return

        new_node = self._new_atom_node("field", "u8")
        children.insert(idx + 1, new_node)
        self._refresh_tree(select_uid=new_node.get("__uid"))
        self.editor_status_var.set("已添加兄弟字段")

    def delete_node(self) -> None:
        uid = self._selected_uid()
        if not uid:
            return
        if uid == self.schema.get("__uid"):
            messagebox.showinfo("提示", "根节点不允许删除")
            return

        parent, relation, idx = self._find_parent(uid)
        if parent is None or relation is None:
            return

        if relation == "children":
            children = parent.get("children", [])
            if isinstance(children, list) and 0 <= idx < len(children):
                children.pop(idx)
        else:
            parent.pop("child_template", None)

        self._refresh_tree(select_uid=parent.get("__uid"))
        self.editor_status_var.set("已删除字段")

    def move_node(self, direction: int) -> None:
        uid = self._selected_uid()
        if not uid:
            return

        parent, relation, idx = self._find_parent(uid)
        if parent is None or relation != "children":
            messagebox.showinfo("提示", "当前节点不支持移动")
            return

        children = parent.get("children", [])
        if not isinstance(children, list):
            return

        new_idx = idx + direction
        if new_idx < 0 or new_idx >= len(children):
            return

        children[idx], children[new_idx] = children[new_idx], children[idx]
        self._refresh_tree(select_uid=uid)
        self.editor_status_var.set("已调整字段顺序")

    def reset_root(self) -> None:
        self.schema = self._default_root()
        self._uid_seed = 0
        self._assign_uid_recursive(self.schema)
        self._refresh_tree(select_uid=self.schema.get("__uid"))
        self._on_tree_select()
        self.editor_status_var.set("已重置为空协议")

    def _load_schema_dict(self, cfg: Dict[str, Any], silent: bool = False) -> None:
        self.schema = copy.deepcopy(cfg)
        self._uid_seed = 0
        self._assign_uid_recursive(self.schema)
        self._refresh_tree(select_uid=self.schema.get("__uid"))
        self._on_tree_select()
        if not silent:
            self.editor_status_var.set("已加载配置")

    def load_from_text(self, text: str, silent: bool = False) -> None:
        try:
            cfg = parse_config_json(text)
        except ParseError as exc:
            if not silent:
                messagebox.showerror("加载失败", str(exc))
            if silent:
                self.reset_root()
            return
        self._load_schema_dict(cfg, silent=silent)

    def load_from_parent(self) -> None:
        self.load_from_text(self.parent.config_text.get("1.0", tk.END), silent=False)
        self.current_file_path = ""
        self.focus()

    def load_from_file(self) -> None:
        path = filedialog.askopenfilename(
            title="选择配置 JSON",
            filetypes=[("JSON", "*.json"), ("All Files", "*.*")],
        )
        if not path:
            return

        try:
            with open(path, "r", encoding="utf-8") as f:
                text = f.read()
            self.load_from_text(text, silent=False)
            self.current_file_path = path
            self.editor_status_var.set(f"已加载文件配置: {path}")
            self.focus()
        except Exception as exc:
            messagebox.showerror("加载失败", str(exc))

    def save_to_file(self) -> None:
        if not self.current_file_path:
            self.save_as_file()
            return

        try:
            cfg = _strip_schema_meta(self.schema)
            with open(self.current_file_path, "w", encoding="utf-8") as f:
                f.write(json.dumps(cfg, indent=4, ensure_ascii=False))
                f.write("\n")
            self.editor_status_var.set(f"已保存配置: {self.current_file_path}")
        except Exception as exc:
            messagebox.showerror("保存失败", str(exc))

    def save_as_file(self) -> None:
        path = filedialog.asksaveasfilename(
            title="另存为配置 JSON",
            defaultextension=".json",
            filetypes=[("JSON", "*.json"), ("All Files", "*.*")],
            initialfile="propshow_config.json",
        )
        if not path:
            return

        try:
            cfg = _strip_schema_meta(self.schema)
            with open(path, "w", encoding="utf-8") as f:
                f.write(json.dumps(cfg, indent=4, ensure_ascii=False))
                f.write("\n")
            self.current_file_path = path
            self.editor_status_var.set(f"已另存配置: {path}")
        except Exception as exc:
            messagebox.showerror("另存失败", str(exc))

    def apply_to_parent(self) -> None:
        cfg = _strip_schema_meta(self.schema)
        text = json.dumps(cfg, indent=4, ensure_ascii=False)
        self.parent.config_text.delete("1.0", tk.END)
        self.parent.config_text.insert("1.0", text)
        self.parent._refresh_config_gutter()
        self.parent.status_var.set("已从可视化面板同步配置 JSON")
        self.editor_status_var.set("已应用到解析配置")


def decode_frames_with_header(
    cfg: Dict[str, Any],
    payload: bytes,
    header: Optional[bytes],
) -> List[Tuple[int, int, ParsedNode, List[FieldRow], int, int]]:
    results: List[Tuple[int, int, ParsedNode, List[FieldRow], int, int]] = []

    if not header:
        decoder = CodecXDecoder(cfg, payload)
        root, consumed, rows = decoder.decode_one(payload)
        results.append((0, 0, root, rows, consumed, len(payload)))
        return results

    frame_idx = 0
    cursor = 0
    hlen = len(header)
    while cursor < len(payload):
        pos = payload.find(header, cursor)
        if pos < 0:
            break

        frame_input = payload[pos:]
        decoder = CodecXDecoder(cfg, frame_input)
        try:
            root, consumed, rows = decoder.decode_one(frame_input)
        except ParseError:
            cursor = pos + hlen
            continue

        if consumed <= 0 or consumed > len(frame_input):
            cursor = pos + hlen
            continue

        results.append((frame_idx, pos, root, rows, consumed, len(frame_input)))
        frame_idx += 1
        cursor = pos + consumed

    if not results:
        decoder = CodecXDecoder(cfg, payload)
        root, consumed, rows = decoder.decode_one(payload)
        results.append((0, 0, root, rows, consumed, len(payload)))

    return results


class PropShowApp:
    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("PropShow")
        self.root.geometry("1480x860")
        self.root.minsize(980, 640)
        self._setup_styles()

        self.auto_split_var = tk.BooleanVar(value=True)
        self.bytes_view_var = tk.StringVar(value="bytes")
        self.result_view_var = tk.StringVar(value="protocol")
        self.status_var = tk.StringVar(value="准备就绪")
        self.bytes_view_mode = "bytes"
        self.visual_builder: Optional[VisualSchemaBuilder] = None

        self._build_ui()

    def _setup_styles(self) -> None:
        self.root.configure(bg="#eef2f7")

        style = ttk.Style(self.root)
        if "clam" in style.theme_names():
            style.theme_use("clam")

        style.configure("App.TFrame", background="#eef2f7")
        style.configure("Panel.TFrame", background="#f7f9fc")
        style.configure("Header.TFrame", background="#17324d")
        style.configure("HeaderTitle.TLabel", background="#17324d", foreground="#ffffff", font=("Segoe UI", 14, "bold"))
        style.configure("HeaderSub.TLabel", background="#17324d", foreground="#c8d5e4", font=("Segoe UI", 10))
        style.configure("Status.TLabel", background="#eef2f7", foreground="#2c3e50", font=("Segoe UI", 10))

        style.configure("TLabelframe", background="#f7f9fc", borderwidth=1)
        style.configure("TLabelframe.Label", background="#f7f9fc", foreground="#17324d", font=("Segoe UI", 10, "bold"))

        style.configure("Toolbar.TButton", padding=(10, 6), font=("Segoe UI", 10))
        style.configure("Primary.TButton", padding=(12, 6), font=("Segoe UI", 10, "bold"))
        style.configure("Builder.TButton", padding=(10, 6), font=("Segoe UI", 10))
        style.map(
            "Primary.TButton",
            background=[("active", "#1f5c8b"), ("!disabled", "#2d79b5")],
            foreground=[("!disabled", "#ffffff")],
        )
        style.map(
            "Builder.TButton",
            background=[("active", "#1a6b5a"), ("!disabled", "#2a8a74")],
            foreground=[("!disabled", "#ffffff")],
        )
        style.map(
            "Toolbar.TButton",
            background=[("active", "#d8e3ef"), ("!disabled", "#e9eff6")],
        )

        style.configure("TCheckbutton", background="#eef2f7", font=("Segoe UI", 10), foreground="#22313f")
        style.configure("TRadiobutton", background="#eef2f7", font=("Segoe UI", 10), foreground="#22313f")
        style.configure("Treeview", rowheight=24, font=("Consolas", 10))
        style.configure("Treeview.Heading", font=("Segoe UI", 10, "bold"), background="#d8e3ef", foreground="#17324d")
        style.map("Treeview", background=[("selected", "#c9def0")], foreground=[("selected", "#102a43")])

    def _build_ui(self) -> None:
        root_wrap = ttk.Frame(self.root, style="App.TFrame")
        root_wrap.pack(fill=tk.BOTH, expand=True)

        header = ttk.Frame(root_wrap, style="Header.TFrame")
        header.pack(fill=tk.X, padx=12, pady=(10, 8))
        ttk.Label(header, text="PropShow", style="HeaderTitle.TLabel").pack(anchor=tk.W, padx=12, pady=(10, 2))
        ttk.Label(
            header,
            text="配置驱动协议解析工具 | 导入、解析、导出一体化",
            style="HeaderSub.TLabel",
        ).pack(anchor=tk.W, padx=12, pady=(0, 10))

        # 主工具栏（不放在任何输入框内），并与下方输入区分割线同步
        self.top_split = ttk.Panedwindow(root_wrap, orient=tk.HORIZONTAL)
        self.top_split.pack(fill=tk.X, padx=12, pady=(0, 8))

        left_top = ttk.Frame(self.top_split, style="App.TFrame")
        self.top_split.add(left_top, weight=1)
        cfg_group = ttk.Frame(left_top, style="App.TFrame")
        cfg_group.pack(side=tk.LEFT)
        ttk.Button(cfg_group, text="加载配置", command=self.load_config_file, style="Toolbar.TButton").pack(side=tk.LEFT)
        ttk.Button(cfg_group, text="导出配置", command=self.export_config_file, style="Toolbar.TButton").pack(side=tk.LEFT, padx=6)
        ttk.Button(cfg_group, text="示例", command=self.fill_sample, style="Toolbar.TButton").pack(side=tk.LEFT, padx=(10, 0))
        ttk.Button(cfg_group, text="可视化配置 ▶", command=self.open_visual_builder, style="Builder.TButton").pack(side=tk.LEFT, padx=6)

        right_top = ttk.Frame(self.top_split, style="App.TFrame")
        self.top_split.add(right_top, weight=1)
        option_group = ttk.Frame(right_top, style="App.TFrame")
        option_group.pack(side=tk.LEFT)
        ttk.Label(option_group, text="显示:", style="Status.TLabel").pack(side=tk.LEFT)
        ttk.Radiobutton(option_group, text="字节流", variable=self.bytes_view_var, value="bytes", command=self.on_bytes_view_change).pack(side=tk.LEFT, padx=(6, 0))
        ttk.Radiobutton(option_group, text="配置说明", variable=self.bytes_view_var, value="guide", command=self.on_bytes_view_change).pack(side=tk.LEFT, padx=(8, 0))
        ttk.Separator(option_group, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=12)
        ttk.Label(option_group, text="自动分帧:", style="Status.TLabel").pack(side=tk.LEFT)
        ttk.Radiobutton(option_group, text="开", variable=self.auto_split_var, value=True).pack(side=tk.LEFT, padx=(6, 0))
        ttk.Radiobutton(option_group, text="关", variable=self.auto_split_var, value=False).pack(side=tk.LEFT, padx=(8, 0))

        action_group = ttk.Frame(right_top, style="App.TFrame")
        action_group.pack(side=tk.RIGHT)
        ttk.Button(action_group, text="解析", command=self.parse_now, style="Primary.TButton").pack(side=tk.RIGHT)
        ttk.Button(action_group, text="清空结果", command=self.clear_result, style="Toolbar.TButton").pack(side=tk.RIGHT, padx=8)

        paned = ttk.Panedwindow(root_wrap, orient=tk.VERTICAL)
        paned.pack(fill=tk.BOTH, expand=True, padx=12, pady=(0, 8))

        input_wrap = ttk.Panedwindow(paned, orient=tk.HORIZONTAL)
        self.input_wrap = input_wrap
        paned.add(input_wrap, weight=3)

        self.root.after(50, self._sync_top_split_with_input)
        self.input_wrap.bind("<Configure>", self._sync_top_split_with_input)

        config_frame = ttk.Labelframe(input_wrap, text="配置 JSON", style="TLabelframe")
        bytes_frame = ttk.Labelframe(input_wrap, text="协议字节流 (Hex)", style="TLabelframe")
        input_wrap.add(config_frame, weight=1)
        input_wrap.add(bytes_frame, weight=1)

        config_editor = ttk.Frame(config_frame, style="Panel.TFrame")
        config_editor.pack(fill=tk.BOTH, expand=True, padx=6, pady=6)
        config_editor.rowconfigure(0, weight=1)
        config_editor.columnconfigure(1, weight=1)

        self.config_gutter = tk.Canvas(config_editor, width=52, highlightthickness=0, background="#eef2f7")
        self.config_gutter.grid(row=0, column=0, sticky="ns")
        self.config_gutter.bind("<Button-1>", lambda _event: None)

        self.config_font = tkfont.Font(family="Consolas", size=11) if tkfont else None

        self.config_text = tk.Text(config_editor, wrap=tk.NONE, font=("Consolas", 11))
        self.config_text.grid(row=0, column=1, sticky="nsew")
        self.config_text.configure(background="#fbfcfe", foreground="#102a43", insertbackground="#102a43", relief=tk.FLAT)

        self.config_scrollbar = ttk.Scrollbar(config_editor, orient=tk.VERTICAL)
        self.config_scrollbar.grid(row=0, column=2, sticky="ns")
        self.config_scrollbar.configure(command=self._sync_config_scroll)
        self.config_text.configure(yscrollcommand=self._on_config_text_scroll)
        self.config_text.bind("<KeyRelease>", lambda _event: self._refresh_config_gutter())
        self.config_text.bind("<Configure>", lambda _event: self._refresh_config_gutter())

        self.bytes_stack = ttk.Frame(bytes_frame)
        self.bytes_stack.pack(fill=tk.BOTH, expand=True, padx=6, pady=6)
        self.bytes_stack.rowconfigure(0, weight=1)
        self.bytes_stack.columnconfigure(0, weight=1)

        self.bytes_editor_frame = ttk.Frame(self.bytes_stack)
        self.bytes_editor_frame.grid(row=0, column=0, sticky="nsew")
        self.bytes_text = tk.Text(self.bytes_editor_frame, wrap=tk.WORD, font=("Consolas", 11))
        self.bytes_text.pack(fill=tk.BOTH, expand=True)
        self.bytes_text.configure(background="#fbfcfe", foreground="#102a43", insertbackground="#102a43", relief=tk.FLAT)

        self.guide_editor_frame = ttk.Frame(self.bytes_stack)
        self.guide_editor_frame.grid(row=0, column=0, sticky="nsew")
        self.guide_text = tk.Text(self.guide_editor_frame, wrap=tk.WORD, font=("Consolas", 10))
        self.guide_text.pack(fill=tk.BOTH, expand=True)
        self.guide_text.configure(background="#f4f7fb", foreground="#203040", relief=tk.FLAT)
        self.guide_text.insert("1.0", CONFIG_GUIDE_TEXT)
        self.guide_text.configure(state=tk.DISABLED)

        self.show_bytes_editor()
        self._refresh_config_gutter()

        result_panel = ttk.Frame(paned, style="Panel.TFrame")
        paned.add(result_panel, weight=2)

        result_toolbar = ttk.Frame(result_panel, style="Panel.TFrame")
        result_toolbar.pack(fill=tk.X, padx=6, pady=(6, 0))
        ttk.Label(result_toolbar, text="结果视图:", style="Status.TLabel").pack(side=tk.LEFT)
        ttk.Radiobutton(result_toolbar, text="协议视图", variable=self.result_view_var, value="protocol", command=self.on_result_view_change).pack(side=tk.LEFT, padx=(8, 0))
        ttk.Radiobutton(result_toolbar, text="解析树", variable=self.result_view_var, value="tree", command=self.on_result_view_change).pack(side=tk.LEFT, padx=(10, 0))
        ttk.Button(result_toolbar, text="导出协议视图", command=self.export_protocol_view, style="Toolbar.TButton").pack(side=tk.RIGHT)
        ttk.Button(result_toolbar, text="导出字节流", command=self.export_bytes_file, style="Toolbar.TButton").pack(side=tk.RIGHT, padx=6)
        ttk.Button(result_toolbar, text="加载字节流", command=self.load_bytes_file, style="Toolbar.TButton").pack(side=tk.RIGHT)

        result_stack = ttk.Frame(result_panel, style="Panel.TFrame")
        result_stack.pack(fill=tk.BOTH, expand=True, padx=0, pady=0)
        result_stack.rowconfigure(0, weight=1)
        result_stack.columnconfigure(0, weight=1)

        tree_frame = ttk.Frame(result_stack, style="Panel.TFrame")
        tree_frame.grid(row=0, column=0, sticky="nsew")
        table_frame = ttk.Frame(result_stack, style="Panel.TFrame")
        table_frame.grid(row=0, column=0, sticky="nsew")
        self.tree_frame = tree_frame
        self.table_frame = table_frame

        self.tree_text = tk.Text(tree_frame, wrap=tk.NONE, font=("Consolas", 10))
        self.tree_text.pack(fill=tk.BOTH, expand=True, padx=6, pady=6)
        self.tree_text.configure(background="#fbfcfe", foreground="#102a43", relief=tk.FLAT)

        cols = ("value", "offset", "ascii", "int")
        self.table = ttk.Treeview(table_frame, columns=cols, show="tree headings", height=12)
        self.table.heading("#0", text="字段描述", anchor=tk.W)
        self.table.heading("value", text="值", anchor=tk.W)
        self.table.heading("offset", text="偏移", anchor=tk.W)
        self.table.heading("ascii", text="ASCII", anchor=tk.W)
        self.table.heading("int", text="int值", anchor=tk.W)
        self.table.column("#0", width=380, anchor=tk.W)
        self.table.column("value", width=420, anchor=tk.W)
        self.table.column("offset", width=120, anchor=tk.W)
        self.table.column("ascii", width=180, anchor=tk.W)
        self.table.column("int", width=140, anchor=tk.W)

        table_scroll = ttk.Scrollbar(table_frame, orient=tk.VERTICAL, command=self.table.yview)
        self.table.configure(yscrollcommand=table_scroll.set)
        self.table.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(6, 0), pady=6)
        table_scroll.pack(side=tk.RIGHT, fill=tk.Y, padx=(0, 6), pady=6)

        self.on_result_view_change()

        bottom = ttk.Frame(root_wrap, style="App.TFrame")
        bottom.pack(fill=tk.X, padx=12, pady=(0, 10))
        ttk.Separator(bottom, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=(0, 6))
        ttk.Label(bottom, textvariable=self.status_var, anchor=tk.W, style="Status.TLabel").pack(fill=tk.X)

    def show_bytes_editor(self) -> None:
        self.bytes_view_mode = "bytes"
        self.bytes_editor_frame.tkraise()

    def show_guide_editor(self) -> None:
        self.bytes_view_mode = "guide"
        self.guide_editor_frame.tkraise()

    def on_bytes_view_change(self) -> None:
        if self.bytes_view_var.get() == "guide":
            self.show_guide_editor()
        else:
            self.show_bytes_editor()

    def _sync_top_split_with_input(self, _event=None) -> None:
        """让顶部工具栏分割线与输入区分割线保持一致。"""
        if not hasattr(self, "top_split") or not hasattr(self, "input_wrap"):
            return

        try:
            sash_x = self.input_wrap.sashpos(0)
        except Exception:
            return

        try:
            self.top_split.sashpos(0, sash_x)
        except Exception:
            pass

    def _line_count(self) -> int:
        try:
            return max(1, int(self.config_text.index("end-1c").split(".")[0]))
        except Exception:
            return 1

    def _refresh_config_gutter(self, error_line: Optional[int] = None) -> None:
        self.config_gutter.delete("all")
        total_lines = self._line_count()
        if total_lines <= 0:
            return

        try:
            line_height = max(18, self.config_font.metrics("linespace") + 4 if self.config_font else 18)
        except Exception:
            line_height = 18

        for i in range(1, total_lines + 1):
            bbox = self.config_text.dlineinfo(f"{i}.0")
            if not bbox:
                continue

            y = bbox[1]
            num_text = f"{i}"
            self.config_gutter.create_text(
                40,
                y + line_height // 2,
                text=num_text,
                fill="#8a94a3",
                font=("Consolas", 11),
                anchor="e",
            )

            if error_line == i:
                self._draw_error_badge(8, y + 3, 20, y + 15)

    def _draw_error_badge(self, x1: int, y1: int, x2: int, y2: int) -> None:
        r = 3
        points = [
            x1 + r, y1,
            x2 - r, y1,
            x2, y1,
            x2, y1 + r,
            x2, y2 - r,
            x2, y2,
            x2 - r, y2,
            x1 + r, y2,
            x1, y2,
            x1, y2 - r,
            x1, y1 + r,
            x1, y1,
        ]
        self.config_gutter.create_polygon(
            points,
            smooth=True,
            splinesteps=16,
            fill="#e34d4d",
            outline="#c83a3a",
            width=1,
        )
        self.config_gutter.create_line(
            x1 + 4,
            y1 + 4,
            x2 - 4,
            y2 - 4,
            fill="#ffffff",
            width=1.8,
            capstyle=tk.ROUND,
            joinstyle=tk.ROUND,
        )
        self.config_gutter.create_line(
            x1 + 4,
            y2 - 4,
            x2 - 4,
            y1 + 4,
            fill="#ffffff",
            width=1.8,
            capstyle=tk.ROUND,
            joinstyle=tk.ROUND,
        )

    def _on_config_text_scroll(self, first: str, last: str) -> None:
        self.config_scrollbar.set(first, last)
        self._refresh_config_gutter()

    def _sync_config_scroll(self, *args: str) -> None:
        self.config_text.yview(*args)
        self._refresh_config_gutter()

    def on_result_view_change(self) -> None:
        if self.result_view_var.get() == "tree":
            self.tree_frame.tkraise()
        else:
            self.table_frame.tkraise()

    def load_config_file(self) -> None:
        path = filedialog.askopenfilename(
            title="选择配置 JSON",
            filetypes=[("JSON", "*.json"), ("All Files", "*.*")],
        )
        if not path:
            return
        try:
            with open(path, "r", encoding="utf-8") as f:
                text = f.read()
            self.config_text.delete("1.0", tk.END)
            self.config_text.insert("1.0", text)
            self._refresh_config_gutter()
            self.status_var.set(f"已加载配置: {path}")
        except Exception as exc:
            messagebox.showerror("加载失败", str(exc))

    def load_bytes_file(self) -> None:
        path = filedialog.askopenfilename(
            title="选择字节流文件",
            filetypes=[("Text", "*.txt;*.log;*.hex"), ("All Files", "*.*")],
        )
        if not path:
            return
        try:
            with open(path, "r", encoding="utf-8", errors="ignore") as f:
                text = f.read()
            self.bytes_text.delete("1.0", tk.END)
            self.bytes_text.insert("1.0", text)
            self.status_var.set(f"已加载字节流: {path}")
        except Exception as exc:
            messagebox.showerror("加载失败", str(exc))

    def export_config_file(self) -> None:
        text = self.config_text.get("1.0", tk.END).strip()
        if not text:
            messagebox.showerror("导出失败", "配置为空，无法导出")
            return

        try:
            cfg = parse_config_json(text)
        except ParseError as exc:
            messagebox.showerror("导出失败", f"配置格式错误，无法导出: {exc}")
            return

        path = filedialog.asksaveasfilename(
            title="导出配置 JSON",
            defaultextension=".json",
            filetypes=[("JSON", "*.json"), ("All Files", "*.*")],
            initialfile="propshow_config.json",
        )
        if not path:
            return

        try:
            with open(path, "w", encoding="utf-8") as f:
                f.write(json.dumps(cfg, indent=4, ensure_ascii=False))
            self.status_var.set(f"已导出配置: {path}")
        except Exception as exc:
            messagebox.showerror("导出失败", str(exc))

    def export_bytes_file(self) -> None:
        text = self.bytes_text.get("1.0", tk.END)
        try:
            payload = parse_hex_input(text)
        except ParseError as exc:
            messagebox.showerror("导出失败", f"字节流格式错误，无法导出: {exc}")
            return

        path = filedialog.asksaveasfilename(
            title="导出字节流文本",
            defaultextension=".txt",
            filetypes=[("Text", "*.txt"), ("All Files", "*.*")],
            initialfile="propshow_bytes.txt",
        )
        if not path:
            return

        try:
            grouped = []
            hex_pairs = [f"{b:02X}" for b in payload]
            for i in range(0, len(hex_pairs), 32):
                grouped.append(" ".join(hex_pairs[i:i + 32]))

            with open(path, "w", encoding="utf-8") as f:
                f.write("\n".join(grouped))
                if grouped:
                    f.write("\n")
            self.status_var.set(f"已导出字节流: {path}")
        except Exception as exc:
            messagebox.showerror("导出失败", str(exc))

    def _build_schema_only_tree(self, schema: Dict[str, Any], parent: Optional[ParsedNode] = None) -> ParsedNode:
        node = ParsedNode(
            name=schema.get("name", ""),
            node_type=schema.get("type", ""),
            schema=schema,
            parent=parent,
            start=0,
            end=-1,
        )

        if node.node_type == "static_field":
            children = schema.get("children", [])
            if isinstance(children, list):
                for child_schema in children:
                    child = self._build_schema_only_tree(child_schema, node)
                    node.children.append(child)
        elif node.node_type == "dynamic_field":
            template = schema.get("child_template")
            if isinstance(template, dict):
                child = self._build_schema_only_tree(template, node)
                node.children.append(child)

        return node

    def export_protocol_view(self) -> None:
        if not self.table.get_children():
            messagebox.showerror("导出失败", "协议视图为空，请先解析")
            return

        ts = datetime.now().strftime("%Y%m%d_%H%M%S")
        path = filedialog.asksaveasfilename(
            title="导出协议视图",
            defaultextension=".txt",
            filetypes=[("Text", "*.txt"), ("All Files", "*.*")],
            initialfile=f"propshow_view_{ts}.txt",
        )
        if not path:
            return

        try:
            lines: List[str] = []
            lines.append("Protocol View Export")
            lines.append(f"Export Time: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
            lines.append("=" * 96)
            lines.append(f"{'FIELD':<40} {'VALUE':<34} {'OFFSET':<12} {'ASCII':<10} {'INT':<10}")
            lines.append("-" * 96)

            def walk(item_id: str, depth: int) -> None:
                text = self.table.item(item_id, "text")
                values = self.table.item(item_id, "values")
                value = values[0] if len(values) > 0 else ""
                offset = values[1] if len(values) > 1 else ""
                ascii_val = values[2] if len(values) > 2 else ""
                int_val = values[3] if len(values) > 3 else ""

                field_text = ("  " * depth) + text
                lines.append(
                    f"{field_text[:40]:<40} {str(value)[:34]:<34} {str(offset)[:12]:<12} {str(ascii_val)[:10]:<10} {str(int_val)[:10]:<10}"
                )
                for child in self.table.get_children(item_id):
                    walk(child, depth + 1)

            for root_item in self.table.get_children(""):
                walk(root_item, 0)

            lines.append("=" * 96)
            with open(path, "w", encoding="utf-8") as f:
                f.write("\n".join(lines) + "\n")
            self.status_var.set(f"已导出协议视图: {path}")
        except Exception as exc:
            messagebox.showerror("导出失败", str(exc))

    def fill_sample(self) -> None:
        sample_cfg = {
            "name": "simple_device",
            "type": "static_field",
            "children": [
                {"name": "head_flag", "type": "u16", "value": 43605},
                {"name": "dev_type", "type": "u8"},
                {"name": "sensor_cnt", "type": "u8"},
                {
                    "name": "sensor_list",
                    "type": "dynamic_field",
                    "len_ref": "../sensor_cnt",
                    "len_mode": "count",
                    "child_template": {
                        "name": "sensor",
                        "type": "static_field",
                        "children": [
                            {"name": "id", "type": "u8"},
                            {"name": "data_len", "type": "u8"},
                            {
                                "name": "data",
                                "type": "dynamic_field",
                                "len_ref": "../data_len",
                                "len_mode": "bytes",
                                "child_template": {"name": "byte", "type": "u8"},
                            },
                        ],
                    },
                },
            ],
        }
        self.config_text.delete("1.0", tk.END)
        self.config_text.insert("1.0", json.dumps(sample_cfg, indent=4, ensure_ascii=False))
        self._refresh_config_gutter()
        self.bytes_text.delete("1.0", tk.END)
        self.bytes_text.insert("1.0", "AA 55 01 02 10 03 11 22 33 20 02 FE EF")
        self.status_var.set("已填充示例配置和字节流")

    def open_visual_builder(self) -> None:
        if self.visual_builder and self.visual_builder.win.winfo_exists():
            self.visual_builder.focus()
            return
        self.visual_builder = VisualSchemaBuilder(self)

    def clear_result(self) -> None:
        self.tree_text.delete("1.0", tk.END)
        for item in self.table.get_children():
            self.table.delete(item)
        self.status_var.set("结果已清空")

    def _check_tree_error(self, node: ParsedNode) -> bool:
        if node.error:
            return True
        for child in node.children:
            if self._check_tree_error(child):
                return True
        return False

    def _display_frames(self, frame_results, payload) -> None:
        self.clear_result()

        if not frame_results:
            self.status_var.set("没有可显示的帧")
            return

        tree_lines = []
        has_error = False
        total_consumed = 0

        for frame_idx, global_offset, root, rows, consumed, frame_input_len in frame_results:
            total_consumed += consumed
            if self._check_tree_error(root):
                has_error = True

            tree_lines.append(f"Frame {frame_idx}: offset={global_offset}, consumed={consumed}/{frame_input_len}")
            self._append_tree(root, tree_lines, "")
            tree_lines.append("")

            frame_item = self.table.insert(
                "",
                tk.END,
                text=f"Frame {frame_idx}",
                values=("", f"global:{global_offset}", f"{consumed}/{frame_input_len} bytes", ""),
                open=True,
            )
            self._insert_protocol_node(frame_item, root)

        if has_error:
            # 弹出警告框（仅在存在字节流输入时）
            if payload:
                messagebox.showwarning("解析警告", "解析完成，但存在错误，请查看带 ❌ 标记的字段")
            tree_lines.insert(0, "⚠ 解析过程中存在错误，请查看带 ❌ 标记的字段")
            tree_lines.append("")

        self.tree_text.insert("1.0", "\n".join(tree_lines))
        if payload:
            status_text = f"解析完成: {len(frame_results)} 帧, 输入 {len(payload)} bytes, 累计消费 {total_consumed} bytes"
            if has_error:
                status_text += " (有错误)"
        else:
            status_text = "解析完成: 未输入字节流，仅显示配置结构"
        self.status_var.set(status_text)

    def _append_tree(self, node: ParsedNode, out: List[str], indent: str) -> None:
        if node.error:
            rng = f"[{node.start}]" if node.end < node.start else f"[{node.start}-{node.end}]"
            out.append(f"{indent}{rng} {node.display_name()} ({node.node_type}) ❌ ERROR")
            out.append(f"{indent}    原因: {node.error_msg}")
            return

        if node.node_type in ("static_field", "dynamic_field"):
            rng = f"[{node.start}-{node.end}]" if node.end >= node.start else f"[{node.start}]"
            out.append(f"{indent}{rng} {node.display_name()} ({node.node_type})")
            for c in node.children:
                self._append_tree(c, out, indent + "    ")
            return

        int_text = ""
        try:
            if node.node_type == "leb128":
                iv = 0
                shift = 0
                for b in node.value:
                    iv |= (b & 0x7F) << shift
                    shift += 7
                int_text = str(iv)
            else:
                int_text = str(int.from_bytes(node.value, "big", signed=node.node_type.startswith("s")))
        except Exception:
            int_text = ""

        out.append(
            f"{indent}[{node.start}-{node.end}] {node.display_name()} ({node.node_type}) = {_bytes_to_hex(node.value)}"
            + (f" | {int_text}" if int_text else "")
        )

    def _node_range_text(self, node: ParsedNode) -> str:
        if node.error:
            return f"{node.start}(ERR)"
        return f"{node.start}-{node.end}" if node.end >= node.start else str(node.start)

    def _node_desc_text(self, node: ParsedNode) -> str:
        return node.display_name()

    def _is_bytes_field(self, node: ParsedNode) -> bool:
        if node.node_type != "dynamic_field":
            return False
        return node.schema.get("len_mode") == "bytes"

    def _collect_field_bytes(self, node: ParsedNode) -> bytes:
        if node.node_type not in ("static_field", "dynamic_field"):
            return node.value

        out = bytearray()
        for child in node.children:
            out.extend(self._collect_field_bytes(child))
        return bytes(out)

    def _node_int_text(self, node: ParsedNode) -> str:
        if self._is_bytes_field(node):
            return "-"

        if node.node_type == "leb128":
            iv = 0
            shift = 0
            for b in node.value:
                iv |= (b & 0x7F) << shift
                shift += 7
            return str(iv)

        if node.node_type in ATOM_FIXED_SIZE:
            return str(int.from_bytes(node.value, "big", signed=node.node_type.startswith("s")))

        return ""

    def _node_value_text(self, node: ParsedNode) -> str:
        if self._is_bytes_field(node):
            return _bytes_to_hex(self._collect_field_bytes(node))

        if node.node_type in ("static_field", "dynamic_field"):
            return ""

        hex_text = _bytes_to_hex(node.value)
        return hex_text

    def _node_ascii_text(self, node: ParsedNode) -> str:
        if self._is_bytes_field(node):
            raw = self._collect_field_bytes(node)
            chars: List[str] = []
            for b in raw:
                if 32 <= b <= 126:
                    chars.append(chr(b))
                else:
                    chars.append(".")
            return "".join(chars)

        if node.node_type in ("static_field", "dynamic_field"):
            return ""

        chars: List[str] = []
        for b in node.value:
            if 32 <= b <= 126:
                chars.append(chr(b))
            else:
                chars.append(".")
        return "".join(chars)

    def _insert_protocol_node(self, parent_item: str, node: ParsedNode) -> None:
        item = self.table.insert(
            parent_item,
            tk.END,
            text=self._node_desc_text(node),
            values=(
                self._node_value_text(node),
                self._node_range_text(node),
                self._node_ascii_text(node),
                self._node_int_text(node),
            ),
            open=True,
        )

        if node.error:
            self.table.insert(
                item,
                tk.END,
                text="⚠ " + node.error_msg,
                values=("", "", "", ""),
                tags=("error_tag",),
            )
            self.table.tag_configure("error_tag", foreground="#d32f2f", font=("Consolas", 10, "bold"))
            self.table.tag_configure("err_node", foreground="#d32f2f")
            self.table.item(item, tags=("err_node",))

        if self._is_bytes_field(node):
            return

        for child in node.children:
            self._insert_protocol_node(item, child)

    def parse_now(self) -> None:
        cfg_text = self.config_text.get("1.0", tk.END)
        bytes_text = self.bytes_text.get("1.0", tk.END)

        try:
            cfg = parse_config_json(cfg_text)
            self._refresh_config_gutter()
            payload = parse_hex_input(bytes_text)

            if not payload:
                schema_root = self._build_schema_only_tree(cfg)
                frame_results = [(0, 0, schema_root, [], 0, 0)]
            else:
                header = maybe_build_head_flag_bytes(cfg) if self.auto_split_var.get() else None
                frame_results = decode_frames_with_header(cfg, payload, header)

            self._display_frames(frame_results, payload)

        except ParseError as exc:
            self._refresh_config_gutter(getattr(exc, "line", None))
            self.status_var.set("解析失败")
            messagebox.showerror("解析失败", str(exc))
        except Exception as exc:
            self._refresh_config_gutter()
            self.status_var.set("解析失败")
            messagebox.showerror("异常", str(exc))


def main() -> None:
    if not TK_AVAILABLE:
        raise SystemExit("tkinter is not available. On Linux install python3-tk; on Windows use standard Python installer.")

    root = tk.Tk()
    PropShowApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
