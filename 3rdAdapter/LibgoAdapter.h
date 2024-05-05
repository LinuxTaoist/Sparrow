/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LibgoAdapter.h
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
#ifndef __LIBGO_ADAPTER_H__
#define __LIBGO_ADAPTER_H__

#include <functional>

namespace LibgoAdapter
{
    class CoPool
    {
    public:
        CoPool();
        ~CoPool();

        void InitCoroutinePool(size_t maxCoroutineCount);

        void Start(int minThreadNumber, int maxThreadNumber = 0);

        void Post(std::function<void()> const& func, std::function<void()> const& callback);

        template <typename R>
        void Post(std::function<R()> const& func, std::function<void(R&)> const& callback);
    };

}; // namespace LibgoAdapter


#endif // __LIBGO_ADAPTER_H__
