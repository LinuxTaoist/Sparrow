/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LibgoAdapter.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/03/02
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/02 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "libgo.h"
#include "LibgoAdapter.h"

using namespace LibgoAdapter;

static co::AsyncCoroutinePool* pGoPool = co::AsyncCoroutinePool::Create();

GoPool::GoPool()
{
}

GoPool::~GoPool()
{
}

void GoPool::InitCoroutinePool(size_t maxCoroutineCount)
{
    if (pGoPool != nullptr)
    {
        pGoPool->InitCoroutinePool(maxCoroutineCount);
    }
}

void GoPool::Start(int minThreadNumber, int maxThreadNumber)
{
    if (pGoPool != nullptr)
    {
        pGoPool->Start(minThreadNumber, maxThreadNumber);
    }
}

void GoPool::Post(std::function<void()> const& func, std::function<void()>  const& callback)
{
    if (pGoPool != nullptr)
    {
        pGoPool->Post(func, callback);
    }
}

template <typename R>
void GoPool::Post(std::function<R()> const& func, std::function<void(R&)> const& callback)
{
    if (pGoPool != nullptr)
    {
        pGoPool->Post(func, callback);
    }
}

