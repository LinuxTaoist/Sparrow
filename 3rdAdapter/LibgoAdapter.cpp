/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LibgoAdapter.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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

static co::AsyncCoroutinePool* pCoPool = co::AsyncCoroutinePool::Create();

CoPool::CoPool()
{
}

CoPool::~CoPool()
{
}

void CoPool::InitCoroutinePool(size_t maxCoroutineCount)
{
    if (pCoPool != nullptr)
    {
        pCoPool->InitCoroutinePool(maxCoroutineCount);
    }
}

void CoPool::Start(int minThreadNumber, int maxThreadNumber)
{
    if (pCoPool != nullptr)
    {
        pCoPool->Start(minThreadNumber, maxThreadNumber);
    }
}

void CoPool::Post(std::function<void()> const& func, std::function<void()>  const& callback)
{
    if (pCoPool != nullptr)
    {
        pCoPool->Post(func, callback);
    }
}

template <typename R>
void CoPool::Post(std::function<R()> const& func, std::function<void(R&)> const& callback)
{
    if (pCoPool != nullptr)
    {
        pCoPool->Post(func, callback);
    }
}

