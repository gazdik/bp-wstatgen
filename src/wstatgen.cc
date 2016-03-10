/*
 * wstatgen.cc
 *
 *  Created on: Feb 22, 2016
 *      Author: gazdik
 */

#include <getopt.h>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "statistic.h"

using namespace std;

const string help_msg = "wstatgen [OPTIONS]\n\n"
		"Information:\n"
		"\t-h, --help\t\tprints this help\n"
		"\t-l, --list\t\tlist all known charachter sets\n\n"
		"Input/Output specification:\n"
		"\t-f, --file\t\tinput file with dictionary\n"
		"\t-o, --output\t\toutput file\n"
		"\t-e, --encoding\t\tencoding of input file\n"
		"\t-d, --description\tdescription of output file\n"
		"\t--markov-classic\tstatistic for Classic Markov model\n"
		"\t--layered-markov\tstatistic for Layered Markov model\n"
		"\t--context-markov\tstatistic for Variable-order Markov model\n";

struct Options
{
	bool help = false;
	bool list_encodings = false;
	string input_file;
	string output_file;
	// TODO add multi-encoding support
	string encoding;
	string description;
//	StatisticGroup statistics;
	int statistic_flag = false;
};

Options options;

struct option long_options[] = {
		{ "help", no_argument, 0, 'h' },
			{ "list", no_argument, 0, 'l' },
			{ "file", required_argument, 0, 'f' },
			{ "output", required_argument, 0, 'o' },
			{ "encoding", required_argument, 0, 'e' },
			{ "description", required_argument, 0, 'd' },
			{ "markov-classic", no_argument, &options.statistic_flag, true },
			{ "layered-markov", no_argument, &options.statistic_flag, true },
			{ "context-markov", no_argument, &options.statistic_flag, true },
			{ 0, 0, 0, 0 } };

int main(int argc, char *argv[])
{
	int c;
	int option_index = 0;

	StatisticGroup statistics;

	while (1)
	{
		c = getopt_long(argc, argv, "hlf:o:e:d:", long_options, &option_index);

		if (c == -1)
			break;

		switch (c)
		{
			case 0:
				statistics.Add(long_options[option_index].name);
				break;
			case 'h':
				options.help = true;
				break;
			case 'l':
				options.list_encodings = true;
				break;
			case 'f':
				options.input_file = optarg;
				break;
			case 'o':
				options.output_file = optarg;
				break;
			case 'e':
				options.encoding = optarg;
				break;
			case 'd':
				options.description = optarg;
				break;
			default:
				cerr << "Missing options" << endl;
				exit(EXIT_FAILURE);
				break;
		}
	}

	if (options.help)
	{
		cout << help_msg;
		exit(EXIT_SUCCESS);
	}

	if (options.list_encodings)
	{
		cout << "TODO" << endl;
		exit(EXIT_SUCCESS);
	}

	if (options.input_file.empty() or options.output_file.empty()
			or options.encoding.empty() or not options.statistic_flag)
	{
		cout << "Missing options" << endl;
		exit(EXIT_FAILURE);
	}

	// Open output file in text mode and write header
	ofstream ofs { options.output_file, ofstream::out };
	ofs << "%WSTAT-1.0%" << "\n" << "\\Encoding: " << options.encoding << "\n"
			<< "\\Description: " << options.description << "\n" << "\3"; // end of text
	ofs.close();

	ofs.open(options.output_file.c_str(),
			ofstream::out | ofstream::app | ofstream::binary);

	statistics.MakeStatistic(options.input_file);
	statistics.Output(ofs);
}
