# SprDebugNode

## 1. 这是做什么的
SprDebugNode 是 Sparrow 的进程内调试节点，基于命名管道（PPipe）接收调试命令并分发执行。

它主要做两件事：
- 初始化一个命名管道作为调试入口，监听外部写入的命令字符串。
- 按命令名将输入分发到对应的处理函数，支持内置命令和业务模块自定义注册的命令。

内置命令（无需注册，进程启动后即可使用）：

| 命令        | 说明                    |
|-------------|-------------------------|
| `help`      | 列出所有已注册命令       |
| `version`   | 打印编译版本和头文件版本 |
| `proc`      | 打印进程名、日志级别、运行时长、调试路径 |
| `threadpool`| 打印线程池当前状态       |
| `loglevel`  | 动态设置当前进程日志级别 |

## 2. 什么时候会用到
当需要在进程运行期间动态查询状态或触发操作时会用到，适用于线上问题排查、状态监控、日志级别动态调节等场景，无需重启进程。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `SprDebugNode.h` <br> `SprProcPrepare.h`| 调试节点接口 <br> 进程初始化接口 |
| 库文件 | `libsprcore.so` | 包含核心功能的共享库 |

**典型流程**

1. 初始化调试节点（在 `main()` 中调用一次）
	- 调用 `SprProcPrepare::GetInstance()->Init(procName)` 完成进程基础初始化，内部会自动调用 `InitPipeDebugNode` 创建并监听调试管道。

2. 注册自定义命令（模块初始化时调用）
	- 调用 `RegisterCmd(owner, cmd, desc, func)` 为本模块注册调试命令。
	- 模块退出时调用 `UnregisterCmd(owner)` 注销所有命令。

3. 触发命令（运行时通过 shell 输入）
	- 向调试管道写入命令字符串即可触发对应处理函数。
	- 命令支持带参数，参数之间以空格分隔。

**简单示例**

1. 进程初始化（`main()` 中）

    ```cpp
    // 初始化进程，内部自动创建调试管道 /tmp/<procName>
    SprProcPrepare::GetInstance()->Init(SRV_NAME_MY_MODULE);
    ```

2. 注册自定义命令

    ```cpp
    SprDebugNode* p = SprDebugNode::GetInstance();
    p->RegisterCmd(mModuleName, "DumpDetails", "Dump module details",
        std::bind(&MyModule::DebugDumpDetails, this, std::placeholders::_1));
    ```

3. 触发命令

    ```bash
    # 列出所有命令
    echo help > /tmp/<进程名>

    # 动态设置日志级别为 debug(4)
    echo loglevel 4 > /tmp/<进程名>

    # 触发自定义命令
    echo DumpDetails > /tmp/<进程名>
    ```

4. 注销命令

    ```cpp
    SprDebugNode* p = SprDebugNode::GetInstance();
    p->UnregisterCmd(mModuleName);
    ```

**参考代码**
- 接口定义: [SprDebugNode.h](../../Core/SprDebugNode.h)
- 接口实现: [SprDebugNode.cpp](../../Core/SprDebugNode.cpp)
- 进程初始化示例: [main_debug.cpp](../../Components/Basics/DebugSrv/main_debug.cpp)
- 注册示例: [DebugModule.cpp](../../Components/Basics/DebugSrv/DebugModule.cpp)

## 4. 要注意什么
- 命令分发按前缀匹配，输入字符串以第一个空格前的部分作为命令名与已注册命令对比。
- 注册命令数量上限是 200，参考 [SprDebugNode.cpp:36](../../Core/SprDebugNode.cpp#L36)

