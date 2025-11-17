/*
 * @Descripttion: 用以测试方法耗时
 * @Author: yubo
 * @Date: 2022-07-28 15:17:38
 * @LastEditTime: 2022-07-28 15:17:41
 */
#pragma once

#include <iostream>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

class TimerLog
{
	using llong = long long;
public:
	TimerLog(const string tag)
	{
		_begin = high_resolution_clock::now();

		_tag = tag;
	}

	~TimerLog()
	{
		auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - _begin).count();
		cout << "time{" << _tag << "} " <<  elapsed << " ms" <<endl;
	}

	llong Elapesed()
	{
		return duration_cast<milliseconds>(high_resolution_clock::now() - _begin).count();
	}

	void Reset()
	{
		_begin = high_resolution_clock::now();
	}


private:
	time_point<high_resolution_clock> _begin;

	string _tag;
};