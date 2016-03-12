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

#include <getopt.h>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <statistics.h>

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

	StatisticsGroup statistics;

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

	statistics.CreateStatistics(options.input_file);
	statistics.Output(options.output_file);
	statistics.Summary();
}
