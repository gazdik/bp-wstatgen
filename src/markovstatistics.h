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

#ifndef SRC_MARKOVSTATISTICS_H_
#define SRC_MARKOVSTATISTICS_H_

#include <statistics.h>

/**
 * Create statistics for 1st order Markov model
 */
class MarkovStatistics : public Statistics
{
public:
	MarkovStatistics();
	virtual ~MarkovStatistics();

	virtual void CreateStatistics(const std::string & dictionary);
	virtual void Output(const std::string & output_file);

private:
	struct StatEntry
	{
		uint8_t key;
		uint64_t frequency;
	};

	const uint8_t _TYPE = 1;
	const uint32_t _LENGTH = CHARSET_SIZE * CHARSET_SIZE * sizeof(uint16_t);

	/**
	 * Compare two stat entries
	 * @return <0 if first entry is bigger than second
	 * 0 if entries are equal
	 * >0 otherwise
	 */
	static int compareStatEntry(const void *p1, const void *p2);

	/**
	 * Sort zero Markov probabilities based on letter frequencies in dictionary
	 */
	void adjustProbabilities();

	/**
	 * Get frequency of letter in dictionary
	 * @param letter 8bit value of letter
	 * @return Letter frequency in the range of 0 (lowest) to 255 (highest)
	 */
	unsigned getLetterFrequency(uint8_t letter);

	uint64_t *_markov_stats_buffer;
	uint64_t *_markov_stats[CHARSET_SIZE];
	StatEntry *_letter_frequencies;
	char *_line_buffer;
	uint64_t _cnt_valid_lines = 0;
	uint64_t _cnt_total_lines = 0;
};

#endif /* SRC_MARKOVSTATISTICS_H_ */
