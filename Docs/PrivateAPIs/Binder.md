# Binder

## 1. 这是做什么的
PrivateAPIs/Binder 提供 Sparrow 进程间 RPC 的底层通信能力。

它主要做两件事：
- 服务侧注册服务名并创建请求/响应通信通道。
- 客户侧按服务名查找服务并建立请求/响应通信通道。

核心入口是 `BindInterface`：
- `InitializeServiceBinder`：服务侧初始化。
- `InitializeClientBinder`：客户端初始化。

## 2. 什么时候会用到
当模块之间需要跨进程调用并传递结构化请求/响应数据时会用到。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `BindInterface.h` <br> `Parcel.h` | Binder 接口入口 <br> 请求响应数据容器 |
| 库文件 | `libsprbinder.so` | Binder 与 Event 聚合库 |

**典型流程**

1. 服务侧初始化 Binder
	- 调用 `InitializeServiceBinder(serviceName, reqParcel, rspParcel)` 注册服务并获取通道。
	- 进入请求循环：`Wait()` 等待请求，处理后 `Post()` 响应。

2. 客户侧初始化 Binder
	- 调用 `InitializeClientBinder(serviceName, reqParcel, rspParcel)` 连接目标服务。
	- 写入请求后 `Post()` 发送，`TimedWait()` 等待响应。

3. 处理返回值与错误
	- 初始化失败直接返回 false。
	- 请求/响应读写失败由调用方按返回值处理。

**简单示例**

```cpp
std::shared_ptr<Parcel> req = nullptr;
std::shared_ptr<Parcel> rsp = nullptr;

if (!BindInterface::GetInstance()->InitializeClientBinder("power_manager", req, rsp)) {
    return -1;
}

req->WriteInt(1001);
req->Post();
rsp->TimedWait();
```

**参考代码**
- 接口定义: [BindInterface.h](../../PrivateAPIs/Binder/BindInterface.h)
- 接口实现: [BindInterface.cpp](../../PrivateAPIs/Binder/BindInterface.cpp)
- 使用示例: [PowerManagerInterface.cpp](../../PublicAPIs/PowerManagerInterface.cpp)

## 4. 要注意什么
- Binder 管理请求使用固定管理通道 `IBinderM/BinderM` 与 key（`KEY_IBINDER_MANAGER`、`KEY_BINDER_MANAGER`），参考 [BindInterface.cpp:31](../../PrivateAPIs/Binder/BindInterface.cpp#L31)
- 服务侧初始化本质上是 `AddService` 后再 `GetParcel`，任一步失败都返回 false，参考 [BindInterface.cpp:49](../../PrivateAPIs/Binder/BindInterface.cpp#L49)
- 客户侧初始化本质上是 `GetService` 后再 `GetParcel`，服务不存在时返回 false，参考 [BindInterface.cpp:65](../../PrivateAPIs/Binder/BindInterface.cpp#L65)

