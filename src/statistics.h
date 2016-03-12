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

#ifndef SRC_STATISTICS_H_
#define SRC_STATISTICS_H_

#include <cstdint>

#include <string>
#include <vector>
#include <fstream>

const unsigned CHARSET_SIZE = 256;
const unsigned MIN_PASS_LENGTH = 1;
const unsigned MAX_PASS_LENGTH = 64;

/**
 * Base class for statistics
 */
class Statistics
{
public:
	virtual ~Statistics();

	/**
	 * Create statistics from words in dictionary
	 * @param dictionary Dictionary with words or leaked passwords
	 */
	virtual void CreateStatistics(const std::string &dictionary) = 0;

	/**
	 * Write statistics to file
	 * @param output_file Path to output file
	 */
	virtual void Output(const std::string & output_file) = 0;

	/**
	 * Print short summary of created statistics (number of lines, ...)
	 * to standard output. It's not necessery to implement it.
	 */
	virtual void Summary();
protected:
	Statistics();
};

/**
 * Group more stat instances and perform actions on each of them
 */
class StatisticsGroup : public Statistics
{
public:
	StatisticsGroup();
	virtual ~StatisticsGroup();

	virtual void Output(const std::string & output_file);
	virtual void CreateStatistics(const std::string &dictionary);

	/**
	 * Create new stat intance based on name and add it into queue
	 * @param name Name of statistic
	 */
	void Add(const std::string &name);

	/**
	 * Add stat instance into queue
	 * @param statistic Pointer to existing instance
	 */
	void Add(Statistics * statistic);

	virtual void Summary();

private:
	std::vector<Statistics *> _statistics;
};

#endif /* SRC_STATISTICS_H_ */
