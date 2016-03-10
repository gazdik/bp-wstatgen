/*
 * statistic.h
 *
 *  Created on: Feb 23, 2016
 *      Author: gazdik
 */

#ifndef SRC_STATISTIC_H_
#define SRC_STATISTIC_H_

#include <cstdint>

#include <string>
#include <vector>
#include <fstream>


const unsigned CHARSET_SIZE = 256;
const unsigned MIN_PASS_LENGTH = 1;
const unsigned MAX_PASS_LENGTH = 64;

struct StatEntry
{
	uint8_t key;
	uint64_t frequency;
};

int compareStatEntry(const void *p1, const void *p2);

/**
 * Base class for statistic
 */
class Statistic
{
public:
	virtual ~Statistic();

	virtual void MakeStatistic(const std::string &input_file) = 0;
	virtual void Output(std::ofstream &ofs) = 0;

protected:
	Statistic();

private:
};

/**
 * TODO Traverse all subcommands and performs actions on each of them
 */
class StatisticGroup : public Statistic
{
public:
	StatisticGroup();
	virtual ~StatisticGroup();

	void Add(const std::string &name);
	void Add(Statistic *statistic);

	virtual void MakeStatistic(const std::string &input_file);
	virtual void Output(std::ofstream &ofs);

private:
	std::vector<Statistic *> _statistics;
};

/**
 * Create statistic for 1st order Markov model
 */
class MarkovStatistic : public Statistic
{
public:
	MarkovStatistic();
	virtual ~MarkovStatistic();

	virtual void MakeStatistic(const std::string &input_file);
	virtual void Output(std::ofstream &ofs);
private:
	void adjustProbabilities();
	unsigned getLetterFrequency(uint8_t letter);

	uint64_t *_markov_abs_buffer;
	uint64_t *_markov_abs_stats[CHARSET_SIZE];
	StatEntry *_letter_frequency;
	const uint8_t _TYPE = 1;
	const uint32_t _LENGTH = CHARSET_SIZE * CHARSET_SIZE * sizeof(uint16_t);
};

/**
 * Create statistic for Layered Markov model
 */
class LayeredMarkovStatistic : public Statistic
{
public:
	LayeredMarkovStatistic();
	virtual ~LayeredMarkovStatistic();

	virtual void MakeStatistic(const std::string &input_file);
	virtual void Output(std::ofstream &ofs);

private:
	uint64_t *_markov_abs_buffer;
	uint64_t *_markov_abs_stats[MAX_PASS_LENGTH][CHARSET_SIZE];
	const uint8_t _TYPE = 2;
};

/**
 * Create statistic for Variable-order Markov model
 */
class ContextMarkov : public Statistic
{
public:
	ContextMarkov();
	virtual ~ContextMarkov();

	virtual void MakeStatistic(const std::string &input_file);
	virtual void Output(std::ofstream &ofs);
};



#endif /* SRC_STATISTIC_H_ */
