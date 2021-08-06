/*
This program removes C-style comments from an input source
Also removes method bodies so maybe the name is misleading
*/

#define NEW_EXTENSION ".txt"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>

#define next_two_chars(two_chars) (std::memcmp(&line[i], two_chars, 2) == 0)

static bool process_file(const std::string &fname)
{
	std::ifstream f(fname);
	if (! f.good()) {
		std::cout << "input file '" << fname << "' could not be opened" << std::endl;
		return false;
	}
		
	const std::string ofname = fname + std::string(NEW_EXTENSION);
	std::ofstream o(ofname);
	if (! o.good()) {
		std::cout << "output file '" << ofname << "' could not be created" << std::endl;
		return false;
	}
	std::string line;
	bool inside_block = false;
	int brace_depth = 0;
	char last_nonwhitespace_char = ' ';
	while (std::getline(f, line)) {
		bool had_output = false;
		for (size_t i = 0; i < line.size(); ++i) {
			if (next_two_chars("*/")) {
				inside_block = false;
				++i;
				continue;
			} else if (next_two_chars("/*")) {
				inside_block = true;
			} else if (next_two_chars("//")) {
				break;
			}
			if (! inside_block) {
				if (line[i] > ' ') {
					last_nonwhitespace_char = line[i];
				}
				if (line[i] == '}') {
					--brace_depth;
				}
				if (brace_depth < 3 && line[i] != '\r' && line[i] != '\n') {
					if (line[i] > ' ') {
						had_output = true;
					}
					o << line[i];
				}
				if (line[i] == '{') {
					++brace_depth;
				}
			}
		}
		if (had_output && last_nonwhitespace_char != ',' && last_nonwhitespace_char != '(') {
			o << std::endl;
		}
	}
	return true;
}

int main(int argc, char **argv)
{
	if (argc <= 1) {
		std::cout << "Missing input file name" << std::endl;
		return 1;
	}
	const std::string fname = argv[1];
	return process_file(fname) ? EXIT_SUCCESS : EXIT_FAILURE;
}