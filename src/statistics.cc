/*
 * Copyright (C) 2016 Peter Gazdik
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <statistics.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <arpa/inet.h>     // ntohl, ntohs
#endif

#include <iostream>

#include "markovstatistics.h"
#include "layeredmarkovstatistics.h"

using namespace std;

namespace {

const unsigned BUFFER_SIZE = 65536;

} // namespace

void StatisticsGroup::Add(const std::string& name)
{
	if (name == "markov-classic")
		_statistics.push_back(new MarkovStatistics);

	if (name == "layered-markov")
		_statistics.push_back(new LayeredMarkovStatistics);
}

StatisticsGroup::StatisticsGroup()
{
}

StatisticsGroup::~StatisticsGroup()
{
	for (auto i : _statistics)
		delete i;

	_statistics.clear();
}

void StatisticsGroup::Add(Statistics* statistic)
{
	_statistics.push_back(statistic);
}

Statistics::~Statistics()
{
}

Statistics::Statistics()
{
}

void StatisticsGroup::CreateStatistics(const std::string & dictionary)
{
	for (auto i : _statistics)
		i->CreateStatistics(dictionary);
}

void StatisticsGroup::Output(const std::string& output_file)
{
	for (auto i : _statistics)
		i->Output(output_file);
}

void Statistics::Summary()
{
}

void StatisticsGroup::Summary()
{
	for (auto i : _statistics)
		i->Summary();
}
