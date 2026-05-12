/*--------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CUtils.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/12/19
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/12/19 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <fstream>
#include <algorithm>
#include "CUtils.h"

namespace CUtils {

int32_t ReadFile(const std::string& path, std::string& str) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        return -1;
    }

    file.seekg(0, std::ios::end);
    int32_t size = static_cast<int32_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    str.resize(size);
    file.read(&str[0], size);
    if (!file.good() && !file.eof()) {
        file.close();
        return -1;
    }

    file.close();
    return size;
}

ssize_t ReadHexTextToHexVector(const std::string& path, std::vector<uint8_t>& out) {
    out.clear();
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) return -1;

    const auto fileSize = f.tellg();
    if (fileSize == 0) return 0;

    const size_t detectSize = std::min(static_cast<size_t>(fileSize), 1024UL);
    std::vector<char> buf(detectSize);
    f.seekg(0);
    if (!f.read(buf.data(), detectSize)) return -1;

    size_t hexCount = 0;
    for (unsigned char c : buf)
        if (isxdigit(c)) hexCount++;

    const bool isTextHex = (static_cast<double>(hexCount) / detectSize) >= 0.9;
    f.seekg(0);

    if (isTextHex) {
        std::string content((std::istreambuf_iterator<char>(f)), {});
        std::string hexStr;
        hexStr.reserve(content.size());

        for (unsigned char c : content)
            if (isxdigit(c)) hexStr += c;

        out.reserve(hexStr.size() / 2);
        for (size_t i = 0; i + 1 < hexStr.size(); i += 2)
            out.push_back(static_cast<uint8_t>(std::stoi(hexStr.substr(i, 2), nullptr, 16)));

        return static_cast<ssize_t>(out.size());
    } else {
        out.resize(static_cast<size_t>(fileSize));
        return f.read(reinterpret_cast<char*>(out.data()), fileSize)
            ? static_cast<ssize_t>(fileSize)
            : -1;
    }
}

// 通用四则运算计算器（纯函数，无副作用）
// 完全遵循数学运算规则：括号 > 乘除 > 加减
// 支持任意深度嵌套括号，自动忽略所有空格
// 错误码：0=成功，-1=空表达式，-2=无效字符/表达式，-3=除零错误，-4=括号不匹配
int32_t CalculateFromString(const std::string& expr, int32_t& result) {
    result = 0;
    if (expr.empty()) {
        return -1;
    }

    std::vector<int32_t> numStack;
    std::vector<char> opStack;
    int32_t current = 0;
    bool hasNumber = false;
    int bracketCount = 0;

    // 运算符优先级表
    auto getPriority = [](char op) -> int {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        if (op == '(') return 0; // 左括号优先级最低
        return -1;
    };

    // 计算栈顶两个数
    auto calculateTop = [&]() -> int32_t {
        if (numStack.size() < 2 || opStack.empty()) {
            return -2;
        }

        int32_t b = numStack.back();
        numStack.pop_back();
        int32_t a = numStack.back();
        numStack.pop_back();
        char op = opStack.back();
        opStack.pop_back();

        switch (op) {
            case '+': numStack.push_back(a + b); break;
            case '-': numStack.push_back(a - b); break;
            case '*': numStack.push_back(a * b); break;
            case '/':
                if (b == 0) return -3;
                numStack.push_back(a / b);
                break;
            default: return -2;
        }
        return 0;
    };

    for (char c : expr) {
        if (isspace(c)) {
            continue; // 自动忽略所有空格
        }

        if (isdigit(c)) {
            current = current * 10 + (c - '0');
            hasNumber = true;
            continue;
        }

        if (c == '(') {
            if (hasNumber) {
                numStack.push_back(current);
                current = 0;
                hasNumber = false;
            }
            opStack.push_back('(');
            bracketCount++;
            continue;
        }

        if (c == ')') {
            if (hasNumber) {
                numStack.push_back(current);
                current = 0;
                hasNumber = false;
            }
            bracketCount--;
            if (bracketCount < 0) {
                return -4; // 右括号多于左括号
            }

            // 计算括号内的所有运算
            while (!opStack.empty() && opStack.back() != '(') {
                int32_t ret = calculateTop();
                if (ret != 0) {
                    return ret;
                }
            }

            if (!opStack.empty()) {
                opStack.pop_back(); // 弹出左括号
            }

            // ===================== 核心修复 =====================
            // 括号计算结果作为当前数字，使后续运算符能正常识别
            if (!numStack.empty()) {
                current = numStack.back();
                numStack.pop_back();
                hasNumber = true;
            }
            continue;
        }

        if (c == '+' || c == '-' || c == '*' || c == '/') {
            // 处理开头负号和括号后的负号
            if (!hasNumber && c == '-') {
                numStack.push_back(0);
                opStack.push_back('-');
                continue;
            }

            if (!hasNumber) {
                return -2; // 无效表达式：运算符前无数字
            }

            numStack.push_back(current);
            current = 0;
            hasNumber = false;

            // 计算所有优先级高于或等于当前运算符的运算
            while (!opStack.empty() && getPriority(opStack.back()) >= getPriority(c)) {
                int32_t ret = calculateTop();
                if (ret != 0) {
                    return ret;
                }
            }

            opStack.push_back(c);
            continue;
        }

        return -2; // 无效字符
    }

    // 处理最后一个数字
    if (hasNumber) {
        numStack.push_back(current);
    }

    if (bracketCount != 0) {
        return -4; // 括号不匹配
    }

    // 计算剩余的所有运算
    while (!opStack.empty()) {
        int32_t ret = calculateTop();
        if (ret != 0) {
            return ret;
        }
    }

    if (numStack.size() != 1) {
        return -2; // 无效表达式
    }

    result = numStack[0];
    return 0;
}

} // namespace CUtils
