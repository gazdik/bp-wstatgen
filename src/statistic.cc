/*
 * statistic.cc
 *
 *  Created on: Feb 23, 2016
 *      Author: gazdik
 */

#include "statistic.h"

#include <cstdlib>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>

#include <iostream>

using namespace std;

const unsigned BUFFER_SIZE = 256;

int compareStatEntry(const void *p1, const void *p2)
{
	const StatEntry *e1 = static_cast<const StatEntry *>(p1);
	const StatEntry *e2 = static_cast<const StatEntry *>(p2);

	return (e2->frequency - e1->frequency);
}

void StatisticGroup::Add(const std::string& name)
{
	if (name == "markov-classic")
		_statistics.push_back(new MarkovStatistic);

	if (name == "layered-markov")
		_statistics.push_back(new LayeredMarkovStatistic);

	if (name == "context-markov")
		_statistics.push_back(new ContextMarkov);
}

StatisticGroup::StatisticGroup()
{
}

StatisticGroup::~StatisticGroup()
{
}

void StatisticGroup::Add(Statistic* statistic)
{
	_statistics.push_back(statistic);
}

MarkovStatistic::MarkovStatistic()
{
	const int buffer_size = CHARSET_SIZE * CHARSET_SIZE;

	_markov_abs_buffer = new uint64_t[buffer_size];
	memset(_markov_abs_buffer, 0, buffer_size);

	auto markov_tmp_ptr = _markov_abs_buffer;

	for (int i = 0; i < CHARSET_SIZE; i++)
	{
		_markov_abs_stats[i] = markov_tmp_ptr;
		markov_tmp_ptr += CHARSET_SIZE;
	}

	_letter_frequency = new StatEntry[CHARSET_SIZE];
	memset(_letter_frequency, 0, CHARSET_SIZE);

	for (int i = 0; i < CHARSET_SIZE; i++)
	{
		_letter_frequency[i].key = static_cast<uint8_t>(i);
	}
}

MarkovStatistic::~MarkovStatistic()
{
	delete[] _markov_abs_buffer;
	delete[] _letter_frequency;
}

Statistic::~Statistic()
{
}

Statistic::Statistic()
{
}

void StatisticGroup::MakeStatistic(const std::string& input_file)
{
	for (auto i : _statistics)
		i->MakeStatistic(input_file);
}

void StatisticGroup::Output(std::ofstream& ofs)
{
	for (auto i : _statistics)
		i->Output(ofs);
}

void MarkovStatistic::MakeStatistic(const std::string& input_file)
{
	ifstream input { input_file, std::ifstream::in };

	char *line_buffer = new char[BUFFER_SIZE];
	uint8_t s0, s1;

	// Make frequency analysis of letters
	while (input)
	{
		input.getline(line_buffer, BUFFER_SIZE);

		unsigned line_length = strlen(line_buffer);

		if (line_length < MIN_PASS_LENGTH || line_length > MAX_PASS_LENGTH)
			continue;

		s1 = line_buffer[0];
//		_letter_frequency[s1].frequency++;
		_markov_abs_stats[0][s1]++;

		for (unsigned position = 0; position < (line_length - 1); position++)
		{
			s0 = line_buffer[position + 0];
			s1 = line_buffer[position + 1];

			_letter_frequency[s0].frequency++;
			_markov_abs_stats[s0][s1]++;
		}

		_letter_frequency[s1].frequency++;
	}

	adjustProbabilities();
}

void MarkovStatistic::Output(std::ofstream& outfile)
{
	// Write type at the beginning
	outfile.write(reinterpret_cast<const char *>(&_TYPE), 1);

	// Write total length in bytes
	uint32_t length = htonl(_LENGTH);
	outfile.write(reinterpret_cast<char *>(&length), sizeof(uint32_t));

	for (unsigned i = 0; i < CHARSET_SIZE; i++)
	{
		uint64_t total = 0;
		for (unsigned j = 0; j < CHARSET_SIZE; j++)
		{
			total += _markov_abs_stats[i][j];
		}

		for (unsigned j = 0; j < CHARSET_SIZE; j++)
		{
			double rel_probability = _markov_abs_stats[i][j]
					/ static_cast<double>(total);
			uint16_t abs_probability = rel_probability * UINT16_MAX;

			// Convert to big endian
			abs_probability = htons(abs_probability);

			outfile.write(reinterpret_cast<char *>(&abs_probability),
					sizeof(abs_probability));
		}
	}
}

LayeredMarkovStatistic::LayeredMarkovStatistic()
{
	const int buffer_size = MAX_PASS_LENGTH * CHARSET_SIZE * CHARSET_SIZE;

	_markov_abs_buffer = new uint64_t[buffer_size];
	memset(_markov_abs_buffer, 0, buffer_size);

	auto markov_tmp_ptr = _markov_abs_buffer;

	for (unsigned i = 0; i < MAX_PASS_LENGTH; i++)
	{
		for (unsigned j = 0; j < CHARSET_SIZE; j++)
		{
			_markov_abs_stats[i][j] = markov_tmp_ptr;
			markov_tmp_ptr += CHARSET_SIZE;
		}
	}
}

LayeredMarkovStatistic::~LayeredMarkovStatistic()
{
	delete[] _markov_abs_buffer;
}

void LayeredMarkovStatistic::MakeStatistic(const std::string& input_file)
{
	ifstream input { input_file, std::ifstream::in };

	char *line_buffer = new char[BUFFER_SIZE];
	uint8_t s0, s1;

	while (input)
	{
		input.getline(line_buffer, BUFFER_SIZE);

		unsigned line_length = strlen(line_buffer);

		if (line_length < MIN_PASS_LENGTH || line_length > MAX_PASS_LENGTH)
			break;

		s1 = line_buffer[0];
		_markov_abs_stats[0][0][s1]++;

		for (unsigned position = 1; position < (line_length - 1); position++)
		{
			s0 = line_buffer[position + 0];
			s1 = line_buffer[position + 1];

			_markov_abs_stats[position][s0][s1]++;
		}
	}
}

void LayeredMarkovStatistic::Output(std::ofstream& outfile)
{
	// Write type at the beginning
	outfile.write(reinterpret_cast<const char *>(&_TYPE), 1);

	for (unsigned i = 0; i < MAX_PASS_LENGTH; i++)
	{
		for (unsigned j = 0; j < CHARSET_SIZE; j++)
		{
			uint64_t total = 0;
			for (unsigned k = 0; k < CHARSET_SIZE; k++)
			{
				total += _markov_abs_stats[i][j][k];
			}

			for (unsigned k = 0; k < CHARSET_SIZE; k++)
			{
				double rel_probability = _markov_abs_stats[i][j][k]
						/ static_cast<double>(total);
				uint16_t abs_probability = rel_probability * UINT16_MAX;

				// Convert to big endian
				abs_probability = htons(abs_probability);

				outfile.write(reinterpret_cast<char *>(&abs_probability),
						sizeof(abs_probability));
			}
		}
	}
}

ContextMarkov::ContextMarkov()
{
}

ContextMarkov::~ContextMarkov()
{
}

void ContextMarkov::MakeStatistic(const std::string& input_file)
{
}

void ContextMarkov::Output(std::ofstream& ofs)
{
}

void MarkovStatistic::adjustProbabilities()
{
	// Sort letter frequencies in descending order
	qsort(_letter_frequency, CHARSET_SIZE, sizeof(StatEntry), compareStatEntry);

	// Increase non zero Markov probabilities by charset size
	// and increase zero probabilities by letter
	// frequency (value from 0 to 255)
	for (int i = 0; i < CHARSET_SIZE; i++)
	{
		for (int j = 0; j < CHARSET_SIZE; j++)
		{
			if (_markov_abs_stats[i][j] != 0)
				_markov_abs_stats[i][j] += CHARSET_SIZE;
			else
				_markov_abs_stats[i][j] += getLetterFrequency(j);
		}
	}

}

unsigned MarkovStatistic::getLetterFrequency(uint8_t letter)
{
	for (int i = 0; i < CHARSET_SIZE; i++)
	{
		if (_letter_frequency[i].key == letter) {
			return ((CHARSET_SIZE - 1) - i);
		}
	}

	return (0);
}
