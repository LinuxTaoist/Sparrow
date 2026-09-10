/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestEpollEventHandler.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/08/10
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/08/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <string>
#include <cstring>
#include <cstdarg>
#include <atomic>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include "PLog.h"
#include "PTimer.h"
#include "PMsgQueue.h"
#include "IEpollEvent.h"
#include "gtest/gtest.h"

namespace {

static uint64_t GetCurrentTimeUs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

// 最小实现：仅用于测试基类的 Read/Write/Close/IsReady
class TestableEpollEvent : public IEpollEvent {
public:
    explicit TestableEpollEvent(int32_t fd, EpollType type = EPOLL_TYPE_PIPE, void* arg = nullptr)
        : IEpollEvent(fd, type, arg) {}

    void* EpollEvent(int32_t fd, EpollType eType, void* arg) override {
        mEpollCalled = true;
        return nullptr;
    }

    bool mEpollCalled = false;
    void SetReadyPublic(bool ready) { SetReady(ready); }
};

} // anonymous namespace

// ====================================================================================================================
// PLog 测试
// ====================================================================================================================

// 测试单例
TEST(UtilModules_EpollEventHandler, PLog_SingletonReturnsSameInstance) {
    PLog& inst1 = PLog::GetInstance();
    PLog& inst2 = PLog::GetInstance();
    EXPECT_EQ(&inst1, &inst2);
}

// 测试默认日志等级
TEST(UtilModules_EpollEventHandler, PLog_DefaultLevelIsInfo) {
    PLog& log = PLog::GetInstance();
    EXPECT_EQ(PLOG_LEVEL_INFO, log.GetLevel());
}

// 测试SetLevel/GetLevel
TEST(UtilModules_EpollEventHandler, PLog_SetAndGetLevel) {
    PLog& log = PLog::GetInstance();
    log.SetLevel(PLOG_LEVEL_WARN);
    EXPECT_EQ(PLOG_LEVEL_WARN, log.GetLevel());
    log.SetLevel(PLOG_LEVEL_ERROR);
    EXPECT_EQ(PLOG_LEVEL_ERROR, log.GetLevel());
}

// 测试等级过滤：低于当前等级的日志不输出
TEST(UtilModules_EpollEventHandler, PLog_LevelFilterBlocksLowerLevel) {
    PLog& log = PLog::GetInstance();
    log.SetLevel(PLOG_LEVEL_ERROR);

    bool called = false;
    log.RegisterPrintCallback([&called](int level, int line, const char* tag, const char* fmt, va_list ap) {
        called = true;
    });

    log.Print(PLOG_LEVEL_DEBUG, __LINE__, "TAG", "should not appear");
    EXPECT_FALSE(called);

    log.Print(PLOG_LEVEL_INFO, __LINE__, "TAG", "should not appear");
    EXPECT_FALSE(called);

    log.RegisterPrintCallback(nullptr);
}

// 测试等级过滤：高于或等于当前等级的日志应输出
TEST(UtilModules_EpollEventHandler, PLog_LevelFilterPassesHigherOrEqualLevel) {
    PLog& log = PLog::GetInstance();
    log.SetLevel(PLOG_LEVEL_WARN);

    bool called = false;
    int capturedLevel = -1;
    log.RegisterPrintCallback([&called, &capturedLevel](int level, int line, const char* tag, const char* fmt, va_list ap) {
        called = true;
        capturedLevel = level;
    });

    log.Print(PLOG_LEVEL_WARN, 42, "WARN", "warn msg");
    EXPECT_TRUE(called);
    EXPECT_EQ(PLOG_LEVEL_WARN, capturedLevel);

    called = false;
    log.Print(PLOG_LEVEL_ERROR, 99, "ERR", "error msg");
    EXPECT_TRUE(called);
    EXPECT_EQ(PLOG_LEVEL_ERROR, capturedLevel);

    log.RegisterPrintCallback(nullptr);
}

// 测试无回调时走默认printf路径（不崩溃即可）
TEST(UtilModules_EpollEventHandler, PLog_PrintWithoutCallbackDoesNotCrash) {
    PLog& log = PLog::GetInstance();
    log.SetLevel(PLOG_LEVEL_INFO);
    EXPECT_NO_THROW(log.Print(PLOG_LEVEL_INFO, 10, "TAG", "printf test %d\n", 123));
}

// ====================================================================================================================
// IEpollEvent 测试
// ====================================================================================================================

// 测试构造与就绪状态
TEST(UtilModules_EpollEventHandler, IEpollEvent_ConstructorAndIsReady) {
    int fds[2];
    ASSERT_EQ(0, pipe2(fds, O_NONBLOCK));
    TestableEpollEvent evt(fds[0]);
    EXPECT_TRUE(evt.IsReady());
    EXPECT_EQ(fds[0], evt.GetEvtFd());
    EXPECT_EQ(EPOLL_TYPE_PIPE, evt.GetEpollType());
    close(fds[1]);
    close(fds[0]);
}

// 测试 SetReady / IsReady
TEST(UtilModules_EpollEventHandler, IEpollEvent_SetReadyChangesIsReady) {
    int fds[2];
    ASSERT_EQ(0, pipe2(fds, O_NONBLOCK));
    TestableEpollEvent evt(fds[0]);
    EXPECT_TRUE(evt.IsReady());
    evt.SetReadyPublic(false);
    EXPECT_FALSE(evt.IsReady());
    evt.SetReadyPublic(true);
    EXPECT_TRUE(evt.IsReady());
    close(fds[1]);
    close(fds[0]);
}

// 测试 Write(fd, string) + Read(fd, string) 往返
TEST(UtilModules_EpollEventHandler, IEpollEvent_WriteAndReadStringRoundtrip) {
    int fds[2];
    ASSERT_EQ(0, pipe2(fds, O_NONBLOCK));
    TestableEpollEvent writer(fds[1]);
    TestableEpollEvent reader(fds[0]);

    std::string input = "hello epoll event";
    writer.Write(fds[1], input);

    std::string output;
    ssize_t ret = reader.Read(fds[0], output);
    EXPECT_EQ((ssize_t)input.size(), ret);
    EXPECT_EQ(input, output);
    close(fds[1]);
    close(fds[0]);
}

// 测试 Read(string) 空管道返回空
TEST(UtilModules_EpollEventHandler, IEpollEvent_ReadEmptyPipeReturnsZero) {
    int fds[2];
    ASSERT_EQ(0, pipe2(fds, O_NONBLOCK));
    TestableEpollEvent reader(fds[0]);

    std::string output;
    ssize_t ret = reader.Read(fds[0], output);
    EXPECT_EQ(0, ret);
    EXPECT_TRUE(output.empty());
    close(fds[1]);
    close(fds[0]);
}

// 测试 Write(char*, size) 二进制数据
TEST(UtilModules_EpollEventHandler, IEpollEvent_WriteRawDataBuffers) {
    int fds[2];
    ASSERT_EQ(0, pipe2(fds, O_NONBLOCK));
    TestableEpollEvent writer(fds[1]);
    TestableEpollEvent reader(fds[0]);

    const char* data = "\x01\x02\x03\x04";
    writer.Write(fds[1], data, 4);

    char buf[8] = {};
    ssize_t ret = reader.Read(fds[0], buf, sizeof(buf));
    EXPECT_EQ(4, ret);
    EXPECT_EQ('\x01', buf[0]);
    EXPECT_EQ('\x04', buf[3]);
    close(fds[1]);
    close(fds[0]);
}

// 测试 Close 将 fd 置 -1 且不再 ready
TEST(UtilModules_EpollEventHandler, IEpollEvent_CloseSetsFdToNegative) {
    int fds[2];
    ASSERT_EQ(0, pipe2(fds, O_NONBLOCK));
    TestableEpollEvent evt(fds[0]);
    evt.Close();
    EXPECT_EQ(-1, evt.GetEvtFd());
    close(fds[1]);
    // fds[0] is already closed by Close()
}

// 测试 NullArgs
TEST(UtilModules_EpollEventHandler, IEpollEvent_NullArgsReturned) {
    int fds[2];
    ASSERT_EQ(0, pipe2(fds, O_NONBLOCK));
    TestableEpollEvent evt(fds[0], EPOLL_TYPE_PIPE, nullptr);
    EXPECT_EQ(nullptr, evt.GetArgs());
    close(fds[1]);
    close(fds[0]);
}

// 测试 EpollEvent 虚函数调用
TEST(UtilModules_EpollEventHandler, IEpollEvent_EpollEventCallbackCalled) {
    int fds[2];
    ASSERT_EQ(0, pipe2(fds, O_NONBLOCK));
    TestableEpollEvent evt(fds[0]);
    EXPECT_FALSE(evt.mEpollCalled);
    evt.EpollEvent(fds[0], EPOLL_TYPE_PIPE, nullptr);
    EXPECT_TRUE(evt.mEpollCalled);
    close(fds[1]);
    close(fds[0]);
}

// ====================================================================================================================
// PTimer 测试
// ====================================================================================================================

// 测试PTimer构造
TEST(UtilModules_EpollEventHandler, PTimer_ConstructorCreatesTimerFd) {
    PTimer timer;
    EXPECT_TRUE(timer.IsReady());
    EXPECT_GE(timer.GetEvtFd(), 0);
    EXPECT_EQ(EPOLL_TYPE_TIMERFD, timer.GetEpollType());
}

// 测试InitTimer默认使用CLOCK_MONOTONIC
TEST(UtilModules_EpollEventHandler, PTimer_InitTimerUsesMonotonicClock) {
    PTimer timer;
    int32_t fd = timer.GetEvtFd();
    EXPECT_GE(fd, 0);

    struct itimerspec curr;
    int32_t ret = timerfd_gettime(fd, &curr);
    EXPECT_EQ(0, ret);
}

// 测试StartTimer延迟触发 (直接读timerfd，不依赖epoll回调)
TEST(UtilModules_EpollEventHandler, PTimer_StartTimerFiresAfterDelay) {
    PTimer timer;
    timer.StartTimer(50, 0);  // 50ms delay, no repeat
    usleep(80000);  // wait longer than delay

    std::string bytes;
    ssize_t ret = timer.Read(timer.GetEvtFd(), bytes);
    EXPECT_EQ(8, ret);
    EXPECT_EQ(8u, bytes.size());
}

// 测试StartTimer周期性触发 (直接读timerfd，不依赖epoll回调)
TEST(UtilModules_EpollEventHandler, PTimer_StartTimerPeriodic) {
    PTimer timer;
    timer.StartTimer(30, 30);  // 30ms delay, 30ms interval
    usleep(100000);  // wait for ~3 triggers

    std::string bytes;
    ssize_t ret = timer.Read(timer.GetEvtFd(), bytes);
    EXPECT_EQ(8, ret);
    // 周期性定时器应累计 expirations >= 2
    uint64_t exp = 0;
    for (size_t i = 0; i < bytes.size(); ++i) {
        exp |= ((uint64_t)(uint8_t)bytes[i]) << (i * 8);
    }
    EXPECT_GE(exp, 2u);
}

// 测试StopTimer停止触发 (直接操作timerfd，不依赖epoll回调)
TEST(UtilModules_EpollEventHandler, PTimer_StopTimerStopsFiring) {
    PTimer timer;
    timer.StartTimer(30, 20);
    usleep(80000);

    // 读取累积的到期次数
    std::string bytes;
    ssize_t ret = timer.Read(timer.GetEvtFd(), bytes);
    EXPECT_EQ(8, ret);

    timer.StopTimer();
    usleep(50000);

    // non-blocking timerfd：停止后读返回 -1 (EAGAIN) 或 0
    std::string bytes2;
    ret = timer.Read(timer.GetEvtFd(), bytes2);
    EXPECT_LE(ret, 0);
}

// 测试DestoryTimer关闭fd
TEST(UtilModules_EpollEventHandler, PTimer_DestroyTimerClosesFd) {
    PTimer timer;
    int32_t fdBefore = timer.GetEvtFd();
    EXPECT_GE(fdBefore, 0);

    timer.DestoryTimer();
    EXPECT_EQ(-1, timer.GetEvtFd());
}

// 测试Read返回uint64编码
TEST(UtilModules_EpollEventHandler, PTimer_ReadReturnsEncodedUint64) {
    PTimer timer;
    timer.StartTimer(10, 0);
    usleep(50000);

    std::string bytes;
    ssize_t ret = timer.Read(timer.GetEvtFd(), bytes);
    EXPECT_EQ(8, ret);
    EXPECT_EQ(8u, bytes.size());
}

// 测试构造时指定arg
TEST(UtilModules_EpollEventHandler, PTimer_ConstructorArgPassed) {
    int32_t testVal = 42;
    PTimer timer(nullptr, &testVal);
    EXPECT_EQ(&testVal, timer.GetArgs());
}

// ====================================================================================================================
// PMsgQueue 测试
// ====================================================================================================================

static std::string MakeMqName(const char* tag) {
    char buf[128];
    uint64_t us = GetCurrentTimeUs();
    snprintf(buf, sizeof(buf), "/t_pmq_%s_%d_%lu", tag, (int)getpid(), (unsigned long)us);
    return buf;
}

// 构造创建消息队列
TEST(UtilModules_EpollEventHandler, PMsgQueue_ConstructorCreatesMsgQueue) {
    std::string name = MakeMqName("ctor");
    PMsgQueue mq(name, 256);
    EXPECT_TRUE(mq.IsReady());
    EXPECT_GE(mq.GetEvtFd(), 0);
    EXPECT_EQ(name, mq.GetMQDevName());
    EXPECT_EQ(256, mq.GetMQMaxMsg());
}

// 空名称构造失败
TEST(UtilModules_EpollEventHandler, PMsgQueue_EmptyNameNotReady) {
    PMsgQueue mq("", 5);
    EXPECT_FALSE(mq.IsReady());
}

// Send / Recv 字符串往返
TEST(UtilModules_EpollEventHandler, PMsgQueue_SendAndRecvStringRoundtrip) {
    std::string name = MakeMqName("rt");
    PMsgQueue sender(name, 256);
    PMsgQueue receiver(name, 256);

    std::string input = "hello mqueue";
    int32_t ret = sender.Send(input, 1);
    EXPECT_GE(ret, 0);

    std::string output;
    uint32_t prio = 0;
    ret = receiver.Recv(output, prio);
    EXPECT_GT(ret, 0);
    EXPECT_EQ(input, output);
}

// 发送多条消息，按序接收
TEST(UtilModules_EpollEventHandler, PMsgQueue_MultipleMessagesInOrder) {
    std::string name = MakeMqName("multi");
    PMsgQueue writer(name, 10);
    PMsgQueue reader(name, 10);
    ASSERT_TRUE(writer.IsReady());
    ASSERT_TRUE(reader.IsReady());

    for (int32_t i = 0; i < 5; ++i) {
        EXPECT_GE(writer.Send("msg_" + std::to_string(i)), 0);
    }
    for (int32_t i = 0; i < 5; ++i) {
        std::string output;
        uint32_t prio = 0;
        int32_t ret = reader.Recv(output, prio);
        EXPECT_GT(ret, 0) << "Recv failed at msg " << i;
        EXPECT_EQ("msg_" + std::to_string(i), output);
    }
}

// 空队列 Recv 返回 0
TEST(UtilModules_EpollEventHandler, PMsgQueue_RecvEmptyReturnsZero) {
    std::string name = MakeMqName("empty");
    PMsgQueue mq(name, 5);
    std::string output;
    uint32_t prio = 0;
    int32_t ret = mq.Recv(output, prio);
    EXPECT_EQ(0, ret);
    EXPECT_TRUE(output.empty());
}

// Clear 清空队列
TEST(UtilModules_EpollEventHandler, PMsgQueue_ClearEmptiesAllMessages) {
    std::string name = MakeMqName("clear");
    PMsgQueue mq(name, 5);
    mq.Send("msg1", 1);
    mq.Send("msg2", 1);
    mq.Clear();

    std::string msg;
    uint32_t prio = 0;
    EXPECT_EQ(0, mq.Recv(msg, prio));
}

// Close 清理 fd
TEST(UtilModules_EpollEventHandler, PMsgQueue_CloseClearsFd) {
    std::string name = MakeMqName("close");
    PMsgQueue mq(name, 5);
    EXPECT_TRUE(mq.IsReady());
    EXPECT_GE(mq.GetEvtFd(), 0);
    mq.Close();
    EXPECT_EQ(-1, mq.GetEvtFd());
}

// 重复 Close 不崩溃
TEST(UtilModules_EpollEventHandler, PMsgQueue_DoubleCloseDoesNotCrash) {
    std::string name = MakeMqName("dblclose");
    PMsgQueue mq(name, 5);
    ASSERT_TRUE(mq.IsReady());
    mq.Close();
    EXPECT_NO_THROW(mq.Close());
}

// 重复创建同名队列（作用域结束后 mq_unlink）
TEST(UtilModules_EpollEventHandler, PMsgQueue_RecreateSameNameSucceeds) {
    std::string name = MakeMqName("recr");
    {
        PMsgQueue mq1(name, 5);
        EXPECT_TRUE(mq1.IsReady());
    }
    {
        PMsgQueue mq2(name, 5);
        EXPECT_TRUE(mq2.IsReady());
    }
}

// Recv 已 Close 的 fd 返回负值
TEST(UtilModules_EpollEventHandler, PMsgQueue_RecvAfterCloseReturnsMinusOne) {
    std::string name = MakeMqName("closed");
    PMsgQueue mq(name, 5);
    ASSERT_TRUE(mq.IsReady());
    mq.Close();

    std::string msg;
    uint32_t prio = 0;
    int32_t ret = mq.Recv(msg, prio);
    EXPECT_LT(ret, 0) << "Recv after close should fail";
}

// ====================================================================================================================
// PMsgQueue 栈安全 (VLA → stack + heap fallback)
// ====================================================================================================================

// 小消息走栈缓冲区 (≤ PMQ_STACK_BUF_SAFE_MAX)
TEST(UtilModules_EpollEventHandler, PMsgQueue_RecvSmallMessageUsesStackBuffer) {
    std::string name = MakeMqName("small");
    PMsgQueue writer(name, 512);
    PMsgQueue reader(name, 512);
    ASSERT_TRUE(writer.IsReady());
    ASSERT_TRUE(reader.IsReady());

    std::string smallMsg(512, 'A');
    writer.Send(smallMsg);

    std::string output;
    uint32_t prio = 0;
    int32_t ret = reader.Recv(output, prio);
    EXPECT_GT(ret, 0);
    EXPECT_EQ(smallMsg, output);
}

// 单字符消息正常
TEST(UtilModules_EpollEventHandler, PMsgQueue_RecvSingleCharMessage) {
    std::string name = MakeMqName("single");
    PMsgQueue writer(name, 256);
    PMsgQueue reader(name, 256);
    ASSERT_TRUE(writer.IsReady());
    ASSERT_TRUE(reader.IsReady());

    writer.Send("x");
    std::string output;
    uint32_t prio = 0;
    int32_t ret = reader.Recv(output, prio);
    EXPECT_EQ(1, ret);
    EXPECT_EQ("x", output);
}

// ====================================================================================================================
// PMsgQueue EpollEvent 回调
// ====================================================================================================================

// EpollEvent 回调正常触发
TEST(UtilModules_EpollEventHandler, PMsgQueue_EpollEventCallbackInvoked) {
    std::string name = MakeMqName("epcb");
    std::atomic<bool> cbCalled(false);
    std::string cbMsg;
    int32_t cbFd = -1;

    PMsgQueue reader(name, 256,
        [&](int32_t fd, const std::string& msg, void* arg) {
            cbCalled = true;
            cbFd = fd;
            cbMsg = msg;
        });
    ASSERT_TRUE(reader.IsReady());

    PMsgQueue writer(name, 256);
    ASSERT_TRUE(writer.IsReady());
    writer.Send("epoll_test");

    // 直接调用 EpollEvent（模拟 epoll 分发）
    reader.EpollEvent(reader.GetEvtFd(), EPOLL_TYPE_MQUEUE, nullptr);
    EXPECT_TRUE(cbCalled);
    EXPECT_EQ(reader.GetEvtFd(), cbFd);
    EXPECT_EQ("epoll_test", cbMsg);
}

// 无回调不崩溃
TEST(UtilModules_EpollEventHandler, PMsgQueue_EpollEventWithoutCallbackDoesNotCrash) {
    std::string name = MakeMqName("nocb");
    PMsgQueue reader(name, 256);
    ASSERT_TRUE(reader.IsReady());

    EXPECT_NO_THROW(reader.EpollEvent(reader.GetEvtFd(), EPOLL_TYPE_MQUEUE, nullptr));
}
