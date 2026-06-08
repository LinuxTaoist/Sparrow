/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CAtom.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/12/19
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/12/19 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __CATOM_H__
#define __CATOM_H__

#include <string>
#include "CNode.h"
#include "CUtils.h"

class CAtom : public CNode {
public:
    explicit CAtom(const std::shared_ptr<CNode>& parent);
    CAtom(const CAtom& atom);
    CAtom& operator = (const CAtom& atom);
    CAtom(const std::shared_ptr<CNode>& parent, const std::string& name, const std::vector<uint8_t>& value);
    ~CAtom();

    int32_t SetValue(const std::vector<uint8_t>& value);
    int32_t GetValue(std::vector<uint8_t>& value);
    int32_t SetValue(const std::string& name, const std::vector<uint8_t>& value) override;
    int32_t GetValue(const std::string& name, std::vector<uint8_t>& value) override;
    int32_t Decode(const std::vector<uint8_t>& bytes) override;
    int32_t Encode(std::vector<uint8_t>& bytes) override;
    std::shared_ptr<CNode> Clone() override;
    std::shared_ptr<CNode> GetNodeByPath(const std::string& path) override;

    std::string DumpHexValue();

private:
    std::vector<uint8_t> mValue;
};

#endif // __CATOM_H__
