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

#ifndef SRC_LAYEREDMARKOVSTATISTICS_H_
#define SRC_LAYEREDMARKOVSTATISTICS_H_

#include <statistics.h>

/**
 * Create statistics for Layered Markov model
 */
class LayeredMarkovStatistics : public Statistics
{
public:
	LayeredMarkovStatistics();
	virtual ~LayeredMarkovStatistics();

	virtual void CreateStatistics(const std::string & dictionary);
	virtual void Output(const std::string & output_file);

private:
	const uint8_t _TYPE = 2;
	const uint32_t _LENGTH = CHARSET_SIZE * CHARSET_SIZE * MAX_PASS_LENGTH
			* sizeof(uint16_t);

	uint64_t *_markov_stats_buffer;
	uint64_t *_markov_stats[MAX_PASS_LENGTH][CHARSET_SIZE];
};

#endif /* SRC_LAYEREDMARKOVSTATISTICS_H_ */
