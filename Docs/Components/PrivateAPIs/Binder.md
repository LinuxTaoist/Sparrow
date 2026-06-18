# Binder

## 1. 这是做什么的
PrivateAPIs/Binder 提供跨进程 RPC 的底层通信接口。

它主要做两件事：
- 服务侧注册服务并创建通信通道。
- 客户侧按服务名发现服务并建立通信通道。

核心入口是 BindInterface：
- InitializeServiceBinder：给服务侧用。
- InitializeClientBinder：给客户端用。

## 2. 什么时候会用到
当模块之间需要跨进程 RPC 通信时会用到。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `#include "BindInterface.h"` <br> `#include "Parcel.h"` | 跨进程通信接口 <br> 数据打包接口 |
| 库文件 | 内部服务：`libsprcore.so` <br> 外部服务：`libbinder.so` | Sparrow 核心库 <br> 独立 Binder 库 |

**典型流程**

1. 服务侧初始化 Binder
	- 启动时调用 `InitializeServiceBinder(serviceName, reqParcel, rspParcel)` 注册服务。
	- 进入请求循环，通过 `Parcel::Wait()` 等待请求，处理命令后通过 `Parcel::Post()` 响应。

2. 客户侧初始化并发送请求
	- 调用 `InitializeClientBinder(serviceName, reqParcel, rspParcel)` 建立连接。
	- 写入请求数据通过 `Parcel::Post()` 发送，通过 `Parcel::TimedWait()` 等待响应。

**简单示例**

服务端主要代码（参考[06_DebugBinder.cc](../../../Examples/06_DebugBinder.cc)）

```cpp
#define SERVICE_NAME "DebugBinder"

enum TestBinderCmd {
	CMD_TEST = 0,
	CMD_SUM,
	CMD_CUMSUM,
	CMD_VEC,
	CMD_CUST_VEC,
	CMD_MAX
};

int Server()
{
	std::shared_ptr<Parcel> pReqParcel = nullptr;
	std::shared_ptr<Parcel> pRspParcel = nullptr;

	BindInterface::GetInstance()->InitializeServiceBinder(SERVICE_NAME, pReqParcel, pRspParcel);
	if (pReqParcel == nullptr || pRspParcel == nullptr) {
		return -1;
	}

	do {
		int cmd = 0;
		NONZERO_CHECK_RET(pReqParcel->Wait());
		NONZERO_CHECK_RET(pReqParcel->ReadInt(cmd));
		switch(cmd) {
			case CMD_TEST: {
				NONZERO_CHECK_RET(pRspParcel->WriteInt(0));
				NONZERO_CHECK_RET(pRspParcel->Post());
				break;
			}
			case CMD_SUM: {
				int a = 0, b = 0;
				NONZERO_CHECK_RET(pReqParcel->ReadInt(a));
				NONZERO_CHECK_RET(pReqParcel->ReadInt(b));

				int sum = a + b;
				NONZERO_CHECK_RET(pRspParcel->WriteInt(sum));
				NONZERO_CHECK_RET(pRspParcel->WriteInt(0));
				NONZERO_CHECK_RET(pRspParcel->Post());
				break;
			}
			default:
				break;
		}
	} while(1);

	return 0;
}
```

客户端主要代码（参考[06_DebugBinder.cc](../../../Examples/06_DebugBinder.cc)）

```cpp
int Client()
{
	std::shared_ptr<Parcel> pReqParcel = nullptr;
	std::shared_ptr<Parcel> pRspParcel = nullptr;

	BindInterface::GetInstance()->InitializeClientBinder(SERVICE_NAME, pReqParcel, pRspParcel);
	if (pReqParcel == nullptr || pRspParcel == nullptr) {
		return -1;
	}

	NONZERO_CHECK_RET(pReqParcel->WriteInt(CMD_SUM));
	NONZERO_CHECK_RET(pReqParcel->WriteInt(10));
	NONZERO_CHECK_RET(pReqParcel->WriteInt(20));
	NONZERO_CHECK_RET(pReqParcel->Post());

	int sum = 0, ret = 0;
	NONZERO_CHECK_RET(pRspParcel->TimedWait());
	NONZERO_CHECK_RET(pRspParcel->ReadInt(sum));
	NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));

	return (ret == 0) ? sum : -1;
}
```

参考代码：
- Binder接口定义: [BindInterface.h](../../../PrivateAPIs/Binder/BindInterface.h)
- 客户端示例: [PowerManagerInterface.cpp](../../../PublicAPIs/PowerManagerInterface.cpp)
- 服务端示例: [PowerManagerHub.cpp](../../../Components/Basics/PowerManagerSrv/PowerManagerHub.cpp)

## 4. 要注意什么
- 服务名必须一致：服务侧注册名与客户端查询名要完全相同。
- 请求响应要成对：每次请求都要有对应响应，避免调用端阻塞。
- 命令号要统一：客户端写入的 cmd 要与服务端处理分支一致。
- 该层是通信层，不放业务逻辑。

相关文档：
- [BinderManagerSrv.md](../Basics/BinderManagerSrv.md)
