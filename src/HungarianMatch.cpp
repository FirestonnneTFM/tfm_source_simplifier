// this is using Ford-Fulkerson
// Using DFS vs BFS makes more sense for Bartite graphs

#include "common.hpp"
#include "HungarianMatch.hpp"
#include <cstring>
#include <algorithm>
#include <utility>
#include "MaxBipartiteMatch.hpp"

#define PRETTY_PRINTS
#ifdef PRETTY_PRINTS
template <typename T>
static inline std::ostream &operator<< (std::ostream &o, const std::vector<std::vector<T>> &vec)
{
	o << "{" << std::endl;
	for (size_t i = 0; i < vec.size(); ++i) {
		for (size_t j = 0; j < vec[i].size(); ++j) {
			o << ";  " << vec[i][j];
		}
		o << std::endl;
	}
	o << "}";
	return o;
}

template <typename T>
static inline std::ostream &operator<< (std::ostream &o, const std::vector<T> &vec)
{
	o << "{";
	for (size_t i = 0; i < vec.size(); ++i) {
		o << " " << vec[i];
	}
	o << " }";
	return o;
}
#endif

static int min_in_col(const std::vector<std::vector<int>> &vec, int col)
{
	int ret = vec[0][col];
	for (size_t row = 1; row < vec.size(); ++row) {
		ret = std::min(ret, vec[row][col]);
	}
	return ret;
}

static int min_in_row(const std::vector<std::vector<int>> &vec, int row)
{
	int ret = vec[row][0];
	for (size_t col = 0; col < vec[row].size(); ++col) {
		ret = std::min(ret, vec[row][col]);
	}
	return ret;
}

static int count_if_true(const std::vector<bool> &vec)
{
	int ret = 0;
	for (auto item : vec) {
		if (item) {
			++ret;
		}
	}
	return ret;
}

#define FOR(i) for (int i = 0; i < dim; ++i)
#define FILL(container, value) std::fill((container).begin(), (container).end(), value)

HungarianMatch::HungarianMatch(int left_size, int right_size)
	: num_rows(left_size),
	num_cols(right_size),
	dim(std::max(left_size, right_size)),
	matrix(dim, std::vector<int>(dim))
{
	if (left_size <= 0 || right_size <= 0) {
		trace(left_size);
		trace(right_size);
		fatal("Requires positive size");
	}
	FOR(row) {
		std::memset(matrix[row].data(), 0x3f, dim * sizeof(int));
	}
}

void HungarianMatch::connect(int left, int right, int weight)
{
	if (left < 0 || left >= num_rows) {
		trace(left);
		trace(num_rows);
		fatal("invalid value for left");
	}
	if (right < 0 || right >= num_cols) {
		trace(right);
		trace(num_cols);
		fatal("invalid value for right");
	}
	matrix[left][right] = weight;
}

void HungarianMatch::cover_matrix(std::vector<bool> &marked_rows, std::vector<bool> &marked_cols)
{
	// assign as many zeros as possible
	// "cross out" all zeros in the same column once you assign one
	// (I am using marked_cols for this purpose, it gets cleared later)
	// mark all rows with no assignments
	FILL(marked_cols, false);
	FILL(marked_rows, true);
	std::vector<int> newly_marked_rows;
	FOR(row) {
		FOR(col) {
			if (matrix[row][col] == 0 && !marked_cols[col]) {
				marked_cols[col] = true;
				marked_rows[row] = false;
				break;
			}
		}
	}
	FOR(row) {
		if (marked_rows[row]) {
			newly_marked_rows.emplace_back(row);
		}
	}
	FILL(marked_cols, false);
	std::vector<int> newly_marked_cols;
	for ( ; ; ) {
		int num_updates = 0;
		// mark all columns having zeros in newly marked rows
		newly_marked_cols.clear();
		for (auto row : newly_marked_rows) {
			FOR(col) {
				if (matrix[row][col] == 0 && !marked_cols[col]) {
					marked_cols[col] = true;
					newly_marked_cols.emplace_back(col);
					++num_updates;
				}
			}
		}
		// mark all rows having zeros in newly marked cols
		newly_marked_rows.clear();
		for (auto col : newly_marked_cols) {
			FOR(row) {
				if (matrix[row][col] == 0 && !marked_rows[row]) {
					marked_rows[row] = true;
					newly_marked_rows.emplace_back(row);
					++num_updates;
				}
			}
		}
		// repeat until no new updates are made
		if (num_updates == 0) {
			break;
		}
	}
	// lines go through all marked cols, and UNMARKED rows
	FOR(row) {
		marked_rows[row] = ! marked_rows[row];
	}
}

std::vector<std::pair<int, int>> HungarianMatch::solve()
{
	FOR(row) {
		const int smallest = min_in_row(matrix, row);
		if (smallest != 0) {
			FOR(col) {
				matrix[row][col] -= smallest;
			}
		}
	}
	FOR(col) {
		const int smallest = min_in_col(matrix, col);
		if (smallest != 0) {
			FOR(row) {
				matrix[row][col] -= smallest;
			}
		}
	}
	std::vector<bool> marked_rows(dim);
	std::vector<bool> marked_cols(dim);
	constexpr int inf = 0x3f3f3f3f;
	for ( ; ;) {
		cover_matrix(marked_rows, marked_cols);
		if (count_if_true(marked_rows) + count_if_true(marked_cols) == dim) {
			break;
		}
		int smallest = inf;
		FOR(row) {
			FOR(col) {
				if (!marked_rows[row] && !marked_cols[col]) {
					smallest = std::min(smallest, matrix[row][col]);
				}
			}
		}
		FOR(row) {
			FOR(col) {
				if (!marked_rows[row] && !marked_cols[col]) {
					matrix[row][col] -= smallest;
				} else if (marked_rows[row] && marked_cols[col]) {
					matrix[row][col] += smallest;
				}
			}
		}
	}
	MaxBipartiteMatch mbm(num_rows, num_cols);
	for (int row = 0; row < num_rows; ++row) {
		for (int col = 0; col < num_cols; ++col) {
			if (matrix[row][col] == 0) {
				mbm.connect(row, col);
			}
		}
	}
	std::vector<std::pair<int, int>> results;
	mbm.solve(&results);
	return results;
}