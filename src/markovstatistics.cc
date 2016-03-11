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

#include <markovstatistics.h>

#include <arpa/inet.h> 	// htonl
#include <cstring>			// memset, strlen

#include <iostream>

using namespace std;

namespace {

const unsigned BUFFER_SIZE = 65536;

} // namespace

int MarkovStatistics::compareStatEntry(const void *p1, const void *p2)
{
	const StatEntry *e1 = static_cast<const StatEntry *>(p1);
	const StatEntry *e2 = static_cast<const StatEntry *>(p2);

	return (e2->frequency - e1->frequency);
}

MarkovStatistics::MarkovStatistics()
{
	const int buffer_size = CHARSET_SIZE * CHARSET_SIZE;

	_markov_stats_buffer = new uint64_t[buffer_size];
	memset(_markov_stats_buffer, 0, buffer_size);

	auto markov_tmp_ptr = _markov_stats_buffer;

	for (int i = 0; i < CHARSET_SIZE; i++)
	{
		_markov_stats[i] = markov_tmp_ptr;
		markov_tmp_ptr += CHARSET_SIZE;
	}

	_letter_frequencies = new StatEntry[CHARSET_SIZE];
	memset(_letter_frequencies, 0, CHARSET_SIZE);

	for (int i = 0; i < CHARSET_SIZE; i++)
	{
		_letter_frequencies[i].key = static_cast<uint8_t>(i);
	}

	_line_buffer = new char[BUFFER_SIZE];
}

MarkovStatistics::~MarkovStatistics()
{
	cout << "Total lines: " << _cnt_total_lines << endl;
	cout << "Valid lines: " << _cnt_valid_lines << endl;

	delete[] _markov_stats_buffer;
	delete[] _letter_frequencies;
	delete[] _line_buffer;
}

void MarkovStatistics::CreateStatistics(const std::string & dictionary)
{
	ifstream input { dictionary, std::ifstream::in };

	uint8_t s0, s1;

	// Make frequency analysis of letters
	while (input)
	{
		input.getline(_line_buffer, BUFFER_SIZE);
		_cnt_total_lines++;

		unsigned line_length = strlen(_line_buffer);

		if (line_length < MIN_PASS_LENGTH || line_length > MAX_PASS_LENGTH)
			continue;

		_cnt_valid_lines++;

		s1 = _line_buffer[0];
		_markov_stats[0][s1]++;

		for (unsigned position = 0; position < (line_length - 1); position++)
		{
			s0 = _line_buffer[position + 0];
			s1 = _line_buffer[position + 1];

			_letter_frequencies[s0].frequency++;
			_markov_stats[s0][s1]++;
		}

		_letter_frequencies[s1].frequency++;

	}

	adjustProbabilities();

}

void MarkovStatistics::adjustProbabilities()
{
	// Sort letter frequencies in descending order
	qsort(_letter_frequencies, CHARSET_SIZE, sizeof(StatEntry), compareStatEntry);

	// Increase non zero Markov probabilities by charset size
	// and increase zero probabilities by letter
	// frequency (value from 0 to 255)
	for (int i = 0; i < CHARSET_SIZE; i++)
	{
		for (int j = 0; j < CHARSET_SIZE; j++)
		{
			if (_markov_stats[i][j] != 0)
				_markov_stats[i][j] += CHARSET_SIZE;
			else
				_markov_stats[i][j] += getLetterFrequency(j);
		}
	}

}

unsigned MarkovStatistics::getLetterFrequency(uint8_t letter)
{
	for (int i = 0; i < CHARSET_SIZE; i++)
	{
		if (_letter_frequencies[i].key == letter)
		{
			return ((CHARSET_SIZE - 1) - i);
		}
	}

	return (0);
}

void MarkovStatistics::Output(const std::string& output_file)
{
	ofstream output { output_file, ofstream::out | ofstream::app
			| ofstream::binary };

	// Write type at the beginning
	output.write(reinterpret_cast<const char *>(&_TYPE), 1);

	// Write total length in bytes
	uint32_t length = htonl(_LENGTH);
	output.write(reinterpret_cast<char *>(&length), sizeof(uint32_t));

	for (unsigned i = 0; i < CHARSET_SIZE; i++)
	{
		uint64_t total = 0;
		for (unsigned j = 0; j < CHARSET_SIZE; j++)
		{
			total += _markov_stats[i][j];
		}

		for (unsigned j = 0; j < CHARSET_SIZE; j++)
		{
			double rel_probability = _markov_stats[i][j] / static_cast<double>(total);
			uint16_t abs_probability = rel_probability * UINT16_MAX;

			// Convert to big endian
			abs_probability = htons(abs_probability);

			output.write(reinterpret_cast<char *>(&abs_probability),
					sizeof(abs_probability));
		}
	}
}
