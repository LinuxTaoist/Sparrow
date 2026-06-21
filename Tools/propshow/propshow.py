#!/usr/bin/env python3
"""
CodecX-compatible protocol dump tool with a simple GUI.

Features:
- Input JSON config and protocol byte stream in hex text.
- Parse by CodecX-like rules: static_field, dynamic_field, len_ref, len_mode, len_formula.
- Supports relative/absolute/fixed len_ref.
- Supports atom types: u8/u16/u32/u64/s8/s16/s32/s64/leb128.
- Optional frame split by root child head_flag.

This script uses only Python standard library so it can run on Windows directly.
"""

from __future__ import annotations

import ast
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
        root = self._decode_node(self.schema, None, data, 0)
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
                    cur = child.end + 1 if child.end >= child.start else child.start
                node.end = cur - 1 if cur > offset else offset - 1
            else:
                template = schema.get("child_template")
                if not isinstance(template, dict):
                    raise ParseError(f"{node.path()} child_template is required for dynamic_field")
                count = self._calc_dynamic_len(node, data, offset)
                cur = offset
                for _ in range(count):
                    child = self._decode_node(template, node, data, cur)
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

        value, size = self._decode_atom(node_type, data, offset, node.path())
        node.value = value
        node.end = offset + size - 1

        int_text = self._to_int_text(node_type, value)
        self.rows.append(
            FieldRow(
                path=node.path(),
                node_type=node_type,
                offset=f"{node.start}-{node.end}",
                hex_value=_bytes_to_hex(value),
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

    # Compatible forms:
    # 1) AA 55 01
    # 2) 0xAA,0x55,0x01
    # 3) AA5501
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


def decode_frames_with_header(
    cfg: Dict[str, Any],
    payload: bytes,
    header: Optional[bytes],
) -> List[Tuple[int, int, ParsedNode, List[FieldRow], int, int]]:
    # (frame_index, global_offset, root_node, rows, consumed_len, frame_input_len)
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
        self.root.geometry("1280x820")
        self.root.minsize(980, 640)
        self._setup_styles()

        self.auto_split_var = tk.BooleanVar(value=True)
        self.bytes_view_var = tk.StringVar(value="bytes")
        self.result_view_var = tk.StringVar(value="protocol")
        self.status_var = tk.StringVar(value="准备就绪")
        self.bytes_view_mode = "bytes"

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
        style.map(
            "Primary.TButton",
            background=[("active", "#1f5c8b"), ("!disabled", "#2d79b5")],
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

        top = ttk.Frame(root_wrap, style="App.TFrame")
        top.pack(fill=tk.X, padx=12, pady=(0, 8))

        io_group = ttk.Frame(top, style="App.TFrame")
        io_group.pack(side=tk.LEFT)
        ttk.Button(io_group, text="加载配置", command=self.load_config_file, style="Toolbar.TButton").pack(side=tk.LEFT)
        ttk.Button(io_group, text="导出配置", command=self.export_config_file, style="Toolbar.TButton").pack(side=tk.LEFT, padx=6)
        ttk.Button(io_group, text="加载字节流", command=self.load_bytes_file, style="Toolbar.TButton").pack(side=tk.LEFT, padx=(10, 0))
        ttk.Button(io_group, text="导出字节流", command=self.export_bytes_file, style="Toolbar.TButton").pack(side=tk.LEFT, padx=6)
        ttk.Button(io_group, text="导出协议视图", command=self.export_protocol_view, style="Toolbar.TButton").pack(side=tk.LEFT, padx=(10, 0))
        ttk.Button(io_group, text="示例", command=self.fill_sample, style="Toolbar.TButton").pack(side=tk.LEFT, padx=6)

        ttk.Separator(top, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=12)

        option_group = ttk.Frame(top, style="App.TFrame")
        option_group.pack(side=tk.LEFT)
        ttk.Label(option_group, text="显示:", style="Status.TLabel").pack(side=tk.LEFT)
        ttk.Radiobutton(option_group, text="字节流", variable=self.bytes_view_var, value="bytes", command=self.on_bytes_view_change).pack(side=tk.LEFT, padx=(6, 0))
        ttk.Radiobutton(option_group, text="配置说明", variable=self.bytes_view_var, value="guide", command=self.on_bytes_view_change).pack(side=tk.LEFT, padx=(8, 0))

        ttk.Separator(option_group, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=12)
        ttk.Label(option_group, text="自动分帧:", style="Status.TLabel").pack(side=tk.LEFT)
        ttk.Radiobutton(option_group, text="开", variable=self.auto_split_var, value=True).pack(side=tk.LEFT, padx=(6, 0))
        ttk.Radiobutton(option_group, text="关", variable=self.auto_split_var, value=False).pack(side=tk.LEFT, padx=(8, 0))

        action_group = ttk.Frame(top, style="App.TFrame")
        action_group.pack(side=tk.RIGHT)
        ttk.Button(action_group, text="解析", command=self.parse_now, style="Primary.TButton").pack(side=tk.RIGHT)
        ttk.Button(action_group, text="清空结果", command=self.clear_result, style="Toolbar.TButton").pack(side=tk.RIGHT, padx=8)

        paned = ttk.Panedwindow(root_wrap, orient=tk.VERTICAL)
        paned.pack(fill=tk.BOTH, expand=True, padx=12, pady=(0, 8))

        input_wrap = ttk.Panedwindow(paned, orient=tk.HORIZONTAL)
        paned.add(input_wrap, weight=3)

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

    def clear_result(self) -> None:
        self.tree_text.delete("1.0", tk.END)
        for item in self.table.get_children():
            self.table.delete(item)
        self.status_var.set("结果已清空")

    def parse_now(self) -> None:
        self.clear_result()
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

            frame_ok = 0
            consumed_total = 0
            tree_lines: List[str] = []

            for frame_idx, global_offset, root, rows, consumed, frame_input_len in frame_results:
                frame_ok += 1
                consumed_total += consumed
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

            if frame_ok == 0:
                raise ParseError("没有可成功解析的帧")

            self.tree_text.insert("1.0", "\n".join(tree_lines))
            if payload:
                status_text = f"解析完成: 成功帧 {frame_ok}, 输入 {len(payload)} bytes, 累计消费 {consumed_total} bytes"
            else:
                status_text = "解析完成: 未输入字节流，仅显示配置结构"
            self.status_var.set(status_text)
        except ParseError as exc:
            self._refresh_config_gutter(getattr(exc, "line", None))
            self.status_var.set("解析失败")
            messagebox.showerror("解析失败", str(exc))
        except Exception as exc:
            self._refresh_config_gutter()
            self.status_var.set("解析失败")
            messagebox.showerror("异常", str(exc))

    def _append_tree(self, node: ParsedNode, out: List[str], indent: str) -> None:
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

        if self._is_bytes_field(node):
            return

        for child in node.children:
            self._insert_protocol_node(item, child)


def main() -> None:
    if not TK_AVAILABLE:
        raise SystemExit("tkinter is not available. On Linux install python3-tk; on Windows use standard Python installer.")

    root = tk.Tk()
    PropShowApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
