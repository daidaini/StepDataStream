/*
 * @Description: 字符串扩展函数，避免重定义冲突
 * @Author: yubo
 * @Date: 2025-01-24
 */
#pragma once

#include <string>

#ifndef STRING_EXTENSIONS_DEFINED
#define STRING_EXTENSIONS_DEFINED

namespace std
{
    inline std::string to_string(std::string &&src)
    {
        return std::forward<std::string>(src);
    }

    inline std::string to_string(const std::string &src)
    {
        return src;
    }

    inline std::string to_string(const char *src)
    {
        if (src == nullptr)
            return "";

        return src;
    }

    inline std::string to_string(char src)
    {
        if (src == '\0')
            return "";

        return std::string(1, src);
    }
}

#endif // STRING_EXTENSIONS_DEFINED
