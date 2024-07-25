/*
 * @Descripttion:
 * @Author: yubo
 * @Date: 2022-06-30 15:45:33
 * @LastEditTime: 2022-08-03 10:52:54
 */
#pragma once

#include <vector>
#include <string>
#include <map>
#include <algorithm>

#if _HAS_CXX17
#include <string_view>
#endif

namespace str
{
	bool StartsWith(const std::string &src, const std::string &sub);
	bool EndsWith(const std::string &src, const std::string &sub);

	std::string Lower(std::string src);
	std::string Upper(std::string src);

	std::string Lstrip(const std::string &src, char character = ' ');
	std::string Rstrip(const std::string &src, char character = ' ');
	/// @brief 去除两端的指定字符
	/// @param src 源字符串
	/// @param character 要去除的字符
	/// @return
	std::string Strip(const std::string &src, char character = ' ');
	std::string StripQuote(const std::string &src);

	/*切分后返回数据没有去空格
	 */
	std::vector<std::string> Split(const std::string &src, char separator);
	/*切分后返回数据去空格
	 */
	std::vector<std::string> SplitWithStrip(const std::string &src, char separator);

	std::string FoldSpace(const std::string &src);
	std::string CutFront(const std::string &src, int length);
	std::string Escape(std::string src, char op);

	std::string Right(const std::string &src, int length);
	std::string Left(const std::string &src, int length);

	std::string Remove(const std::string &src, char c);

	void ReplaceStr(std::string &src, const std::string &before, const std::string &after);
	std::string ReplaceChar(std::string str, char src, char dst);

	/*将分隔符n-1 到 n之间的内容 使用rpDst进行替换 （n必须大于等于1）
	 * 示例：
	 * 原字符串为 "123|456|||7|8|9||"
	 * 将第5个分隔符对应的内容使用 "0000" 替换，即n为5，
	 * 调用后，字符串变为 "123|456|||0000|8|9||"
	 */
	void ReplaceByDelim(std::string &src, char delim, size_t n, const std::string &rpDst);

	/*解析出 两个分隔符中间的内容
	 * 例 解析 "[errno][errmsg]" 获取 errno, errmsg
	 */
	std::vector<std::string> ParseIncludedStr(const std::string &src, char delim1, char delim2);

	/*解析出两个字符串中间的部分内容
	 */
	std::string ParseIncludedOneItem(const std::string &src, const std::string &lKey, const std::string &rKey);

	/*解析出标记mark后的第一个 两个分隔符之间的内容
	 * 例: 解析 "IP:192.168.9.23;MAC:ABCDEFG;" ParseIncludedStr(src, ':', ';', "MAC") -> ABCDEFG
	 */
	std::string ParseIncludedStrByMark(const std::string &src, char delim1, char delim2, const std::string &mark);

	std::string JoinWithDelim(const std::vector<std::string> &srcs, char delim);

#if _HAS_CXX17
	/*将格式如下的字符串解析成key-val对的形式:
	 * user=abcdef;pwd=123456;other=default
	 * key_1=val_1;key_2=val_2;key_3=val_3;
	 * 分隔符，作为入参
	 */
	std::map<std::string, std::string> ParseToPairs(std::string_view src, char delim1 = '=', char delim2 = ';');
#endif

	std::string Cast(char src);
}