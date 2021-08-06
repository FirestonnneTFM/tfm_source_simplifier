#pragma once

#include "common.hpp"

// This is using Ford-Fulkerson

class MaxBipartiteMatch {
private:
	const int LeftSize;
	const int RightSize;
	const int NUM_VERTICES;
	const int SOURCE;
	const int SINK;
	std::vector<std::vector<int>> flows;
	bool aug_path_exists(int *const prev) const;
public:
	MaxBipartiteMatch(int left_size, int right_size);
	// zero indexed
	void connect(int left, int right);
	// results can be nullptr
	// returns number of matches
	int solve(std::vector<std::pair<int, int>> *results);
};
