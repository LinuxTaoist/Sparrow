# BinderManagerSrv

## 1. 这是做什么的
BinderManagerSrv 提供类 Android Binder 的 RPC 基础能力，负责服务注册与查询，维护 `服务名 -> (key, shmName)` 映射。

内部请求仅 3 类：
- AddService：注册服务并返回 key。
- GetService：按服务名返回 shmName 和 key。
- RemoveService：注销服务。

## 2. 什么时候会用到
当系统需要做跨进程 RPC 通信时，就会用到它。

## 3. 怎么用
建立 Binder 通信时，外部业务模块通常对 BinderManagerSrv 无感知。

它作为底层注册与查询中心运行，业务方不需要直接与它交互。

接口使用文档请看：
- [Binder](../../PrivateAPIs/Binder.md)

相关实现入口：
- [main_binder.cpp](../../../../Components/Basics/BinderManagerSrv/main_binder.cpp)
- [BinderManager.h](../../../../Components/Basics/BinderManagerSrv/BinderManager.h)
- [BinderManager.cpp](../../../../Components/Basics/BinderManagerSrv/BinderManager.cpp)
- [BindCommon.h](../../../../PrivateAPIs/Binder/BindCommon.h)
- [BindInterface.cpp](../../../../PrivateAPIs/Binder/BindInterface.cpp)

## 4. 要注意什么
- 映射保存在内存 `mBinderMap`，进程退出后不保留。
- 同名 AddService 会覆盖旧值。
- key 由随机数生成，当前未做冲突检测。
- `BinderInfo.refCount` 目前未在该模块内使用。
- 进程通过 `GENERAL_CMD_EXE_EXIT` 触发主循环退出。
