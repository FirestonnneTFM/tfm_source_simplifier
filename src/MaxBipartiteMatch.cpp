// this is using Ford-Fulkerson
// Using DFS vs BFS makes more sense for Bartite graphs

#include "common.hpp"
#include "MaxBipartiteMatch.hpp"

MaxBipartiteMatch::MaxBipartiteMatch(int left_size, int right_size)
	: LeftSize(left_size),
	RightSize(right_size),
	NUM_VERTICES(left_size + right_size + 2),
	SOURCE(left_size + right_size + 1),
	SINK(left_size + right_size),
	flows(NUM_VERTICES, std::vector<int>(NUM_VERTICES))
{
	for (int i = 0; i < NUM_VERTICES; ++i) {
		for (int j = 0; j < NUM_VERTICES; ++j) {
			flows[i][j] = 0;
		}
	}
	for (int i = 0; i < LeftSize; ++i) {
		flows[SOURCE][i] = 1;
	}
	for (int i = 0; i < RightSize; ++i) {
		flows[i + LeftSize][SINK] = 1;
	}
}

void MaxBipartiteMatch::connect(int left, int right)
{
	if (left < 0 || left >= LeftSize) {
		trace(left);
		trace(LeftSize);
		fatal("invalid value for left");
	}
	if (right < 0 || right >= RightSize) {
		trace(right);
		trace(RightSize);
		fatal("invalid value for right");
	}
	flows[left][LeftSize + right] = 1;
}

bool MaxBipartiteMatch::aug_path_exists(int *const prev) const
{
	std::vector<bool>visited(NUM_VERTICES);
	std::vector<int> stack;
	stack.push_back(SOURCE);
	visited[SOURCE] = true;
	prev[SOURCE] = SOURCE;
	while (! stack.empty()) {
		const int v = stack.back();
		stack.pop_back();
		for (int i = 0; i < NUM_VERTICES; ++i) {
			if (visited[i])
				continue;
			if (flows[v][i] == 0)
				continue;
			prev[i] = v;
			if (i == SINK)
				return true;
			stack.push_back(i);
			visited[i] = true;
		}
	}
	return false;
}

int MaxBipartiteMatch::solve(std::vector<std::pair<int, int>> *results)
{
	constexpr int inf = 0x3f3f3f3f;
	std::vector<int> prev(NUM_VERTICES);
	int sum = 0;
	while (aug_path_exists(prev.data())) {
		int this_flow = inf;
		for (int i = SINK; i != SOURCE; i = prev[i]) {
			const int candidate = flows[prev[i]][i];
			if (candidate < this_flow)
				this_flow = candidate;
		}
		for (int i = SINK; i != SOURCE; i = prev[i]) {
			flows[prev[i]][i] -= this_flow;
			flows[i][prev[i]] += this_flow;
		}
		sum += this_flow;
	}
	for (int u = 0; u < LeftSize; ++u) {
		for (int v = 0; v < RightSize; ++v) {
			if (flows[v + LeftSize][u]) {
				results->emplace_back(std::make_pair(u, v));
			}
		}
	}
	return sum;
}