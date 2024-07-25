#include "StringFunc.h"

using namespace std;

namespace str
{
    bool StartsWith(const std::string &src, const std::string &sub)
    {
        return src.find(sub) == 0;
    }

    bool EndsWith(const std::string &src, const std::string &sub)
    {
        return src.rfind(sub) == (src.length() - sub.length());
    }

    std::string Lower(std::string src)
    {
        std::transform(src.begin(), src.end(), src.begin(),
                       [](unsigned char elem)
                       {
                           return std::tolower(elem);
                       });

        return src;
    }

    std::string Upper(std::string src)
    {
        std::transform(src.begin(), src.end(), src.begin(),
                       [](unsigned char elem)
                       {
                           return std::toupper(elem);
                       });

        return src;
    }

    std::string Lstrip(const std::string &src, char character)
    {
        size_t index = src.find_first_not_of(character);
        if (index == src.npos)
            return "";

        if (index == 0)
            return src;

        return src.substr(index, src.length() - index);
    }

    std::string Rstrip(const std::string &src, char character)
    {
        size_t index = src.find_last_not_of(character);
        if (index == src.npos)
            return "";

        if (index == src.length() - 1)
            return src;

        return src.substr(0, index + 1);
    }

    std::string Strip(const std::string &src, char character)
    {
        return Rstrip(Lstrip(src, character), character);
    }

    std::string StripQuote(const std::string &src)
    {
        if (StartsWith(src, "\"") && EndsWith(src, "\""))
        {
            return src.substr(1, src.size() - 2);
        }
        return src;
    }

    std::vector<std::string> Split(const std::string &src, char separator)
    {
        std::vector<std::string> result;

        size_t pos = 0;
        do
        {
            auto separatePos = src.find_first_of(separator, pos);
            if (separatePos == src.npos)
            {
                auto last = src.substr(pos);
                if (!last.empty())
                    result.emplace_back(std::move(last));

                break;
            }

            result.emplace_back(src.substr(pos, separatePos - pos));
            pos = separatePos + 1;
        } while (true);

        return result;
    }

    std::vector<std::string> SplitWithStrip(const std::string &src, char separator)
    {
        std::vector<std::string> result;

        size_t pos = 0;
        do
        {
            auto separatePos = src.find_first_of(separator, pos);
            if (separatePos == src.npos)
            {
                auto last = Strip(src.substr(pos));
                if (!last.empty())
                {
                    result.emplace_back(std::move(last));
                }

                break;
            }

            auto item = Strip(src.substr(pos, separatePos - pos));
            result.emplace_back(std::move(item));

            pos = separatePos + 1;
        } while (true);

        return result;
    }

    std::string FoldSpace(const std::string &src)
    {
        string result = "";
        char lastItem = '\0';
        for (auto it = src.begin(); it != src.end(); it++)
        {
            if (lastItem == ' ' && *it == ' ')
            {
                continue;
            }
            result.push_back(*it);
            lastItem = *it;
        }
        return Strip(result);
    }

    std::string CutFront(const std::string &src, int length)
    {
        if ((int)src.length() <= length)
        {
            return src;
        }
        return src.substr(src.length() - length, length);
    }

    std::string Escape(std::string src, char op)
    {
        size_t index = 0;
        while ((index = src.find(op, index)) != string::npos)
        {
            src.insert(index, 1, '\\');
            index += 2;
        }
        return src;
    }

    std::string Right(const std::string &src, int length)
    {
        if ((int)src.length() <= length)
        {
            return src;
        }
        return src.substr(src.length() - length, length);
    }

    std::string Left(const std::string &src, int length)
    {
        if ((int)src.length() <= length)
        {
            return src;
        }
        return src.substr(0, length);
    }

    std::string Remove(const std::string &src, char c)
    {
        string result;
        result.reserve(src.size());

        for (auto it : src)
        {
            if (it != c)
                result.push_back(it);
        }

        return result;
    }

    void ReplaceStr(std::string &src, const std::string &before, const std::string &after)
    {
        if (src.empty())
            return;

        size_t index = 0;
        do
        {
            auto it = src.find(before, index);
            if (it == src.npos)
                break;
            src.replace(it, before.length(), after);

            index = it + 1;
        } while (true);
    }

    std::vector<std::string> ParseIncludedStr(const std::string &src, char delim1, char delim2)
    {
        std::vector<std::string> results;

        size_t index = 0;
        do
        {
            auto index_front = src.find_first_of(delim1, index);
            if (index_front == src.npos)
                break;

            auto index_back = src.find_first_of(delim2, index_front);
            if (index_back == src.npos)
            {
                results.emplace_back(src.substr(index_front + 1, src.length() - index_front - 1)); // 直接到结尾
                break;
            }

            results.emplace_back(src.substr(index_front + 1, index_back - index_front - 1));
            index = index_back + 1;
        } while (true);

        return results;
    }

    std::string ParseIncludedStrByMark(const std::string &src, char delim1, char delim2, const std::string &mark)
    {
        auto mark_index = src.find(mark);
        if (mark_index == src.npos)
        {
            return "";
        }

        size_t index_front = src.find_first_of(delim1, mark_index);
        if (index_front == src.npos)
        {
            return "";
        }

        size_t index_back = src.find_first_of(delim2, index_front);
        if (index_back == src.npos)
        {
            return src.substr(index_front + 1, src.length() - index_front - 1); // 直接到结尾
        }

        return src.substr(index_front + 1, index_back - index_front - 1);
    }

    std::string ParseIncludedOneItem(const std::string &src, const std::string &lKey, const std::string &rKey)
    {
        auto lhs = src.find(lKey);
        if (lhs == src.npos)
        {
            return "";
        }
        lhs += lKey.size();

        auto rhs = src.find(rKey, lhs);
        if (rhs == src.npos)
        {
            return "";
        }

        return src.substr(lhs, rhs - lhs);
    }

    std::string JoinWithDelim(const std::vector<std::string> &srcs, char delim)
    {
        if (srcs.empty())
            return {};

        std::string dst;
        for (size_t i = 0; i < srcs.size(); ++i)
        {
            dst += srcs[i];
            if (i != srcs.size() - 1)
                dst += delim;
        }
        return dst;
    }

#if _HAS_CXX17
    std::map<std::string, std::string> ParseToPairs(std::string_view src, char delim1, char delim2)
    {
        std::map<std::string, std::string> results;
        size_t currIndex = 0;
        do
        {
            size_t index1 = src.find_first_of(delim1, currIndex);
            if (index1 == src.npos)
            {
                break;
            }

            string_view k = src.substr(currIndex, index1 - currIndex);

            currIndex = index1 + 1;
            size_t index2 = src.find_first_of(delim2, currIndex);
            string_view v;
            if (index2 == src.npos)
            {
                v = src.substr(currIndex, src.size() - currIndex);
                results.emplace(k, v);
                break;
            }

            v = src.substr(currIndex, index2 - currIndex);
            results.emplace(k, v);
            currIndex = index2 + 1;
        } while (true);
        return results;
    }
#endif

    std::string ReplaceChar(std::string str, char src, char dst)
    {
        size_t index = 0;
        while ((index = str.find(src, index)) != string::npos)
        {
            str[index++] = dst;
        }
        return str;
    }

    void ReplaceByDelim(std::string &src, char delim, size_t n, const std::string &rpDst)
    {
        int firstPos = -1;
        size_t nextPos = src.length() - 1;
        for (size_t i = 0, pos = 0; i <= n - 1; ++i, ++pos)
        {
            pos = src.find(delim, pos);
            if (pos == string::npos)
                break;

            if (i == n - 1)
                nextPos = pos;
            else if (i == n - 2)
                firstPos = pos;
        }

        src.replace(firstPos + 1, nextPos - firstPos - 1, rpDst);
    }

    std::string Cast(char src)
    {
        if (src == '\0')
            return "";

        return std::string(1, src);
    }
}