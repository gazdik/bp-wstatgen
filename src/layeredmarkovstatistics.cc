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

#include <layeredmarkovstatistics.h>

#include <arpa/inet.h>		// htonl
#include <cstring>				// memset, strlen

using namespace std;

namespace {

const unsigned BUFFER_SIZE = 65536;

} // namespace

LayeredMarkovStatistics::LayeredMarkovStatistics()
{
	const int buffer_size = MAX_PASS_LENGTH * CHARSET_SIZE * CHARSET_SIZE;

	_markov_stats_buffer = new uint64_t[buffer_size];
	memset(_markov_stats_buffer, 0, buffer_size);

	auto markov_tmp_ptr = _markov_stats_buffer;

	for (unsigned i = 0; i < MAX_PASS_LENGTH; i++)
	{
		for (unsigned j = 0; j < CHARSET_SIZE; j++)
		{
			_markov_stats[i][j] = markov_tmp_ptr;
			markov_tmp_ptr += CHARSET_SIZE;
		}
	}
}

LayeredMarkovStatistics::~LayeredMarkovStatistics()
{
	delete[] _markov_stats_buffer;
}

void LayeredMarkovStatistics::CreateStatistics(const std::string& dictionary)
{
	ifstream input { dictionary, std::ifstream::in };

	char *line_buffer = new char[BUFFER_SIZE];
	uint8_t s0, s1;

	while (input)
	{
		input.getline(line_buffer, BUFFER_SIZE);

		unsigned line_length = strlen(line_buffer);

		if (line_length < MIN_PASS_LENGTH || line_length > MAX_PASS_LENGTH)
			break;

		s1 = line_buffer[0];
		_markov_stats[0][0][s1]++;

		for (unsigned position = 1; position < (line_length - 1); position++)
		{
			s0 = line_buffer[position + 0];
			s1 = line_buffer[position + 1];

			_markov_stats[position][s0][s1]++;
		}
	}
}

void LayeredMarkovStatistics::Output(const std::string& output_file)
{
	ofstream output { output_file, ofstream::out | ofstream::app
			| ofstream::binary };

	// Write type at the beginning
	output.write(reinterpret_cast<const char *>(&_TYPE), 1);

	// Write total length in bytes
	uint32_t length = htonl(_LENGTH);
	output.write(reinterpret_cast<char *>(&length), sizeof(uint32_t));

	// Calc relative frequency and map it into 16 bit value
	for (unsigned i = 0; i < MAX_PASS_LENGTH; i++)
	{
		for (unsigned j = 0; j < CHARSET_SIZE; j++)
		{
			uint64_t total = 0;
			for (unsigned k = 0; k < CHARSET_SIZE; k++)
			{
				total += _markov_stats[i][j][k];
			}

			for (unsigned k = 0; k < CHARSET_SIZE; k++)
			{
				double rel_probability = _markov_stats[i][j][k]
						/ static_cast<double>(total);
				uint16_t abs_probability = rel_probability * UINT16_MAX;

				abs_probability = htons(abs_probability);

				output.write(reinterpret_cast<char *>(&abs_probability),
						sizeof(abs_probability));
			}
		}
	}
}
