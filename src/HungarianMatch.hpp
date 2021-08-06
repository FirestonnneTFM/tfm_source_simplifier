#pragma once

#include "common.hpp"
// Given a Bipartide graph
// Where the left side can only match one on the right
// Provide the matching such that the minmum total cost is achieved
class HungarianMatch {
private:
	const int num_rows;
	const int num_cols;
	const int dim;
	std::vector<std::vector<int>> matrix;
	void cover_matrix(std::vector<bool> &marked_rows, std::vector<bool> &marked_cols);
public:
	HungarianMatch(int left_size, int right_size);
	// zero indexed
	void connect(int left, int right, int weight);
	std::vector<std::pair<int, int>> solve();
};
