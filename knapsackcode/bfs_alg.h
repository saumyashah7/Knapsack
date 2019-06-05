#ifndef _BFS_ALG_CUH_
#define _BFS_ALG_CUH_

#include "knapsack.h"
#include "bfs_solution.h"
//#include "gpu_mem_pool.h"
#include <memory>
#include <deque>
#include <array>

class bfs_solver
{
protected:
	std::shared_ptr<knapsack> m_inst;

	// Prunes nodes when we hit memory threshold
	void prune_nodes_dfs(std::deque<std::unique_ptr<bfs_solution>>& q_);
	void branch(std::deque<std::unique_ptr<bfs_solution>>& q, uint_t k);
	void bound(std::deque<std::unique_ptr<bfs_solution>>& q, std::unique_ptr<bfs_solution>& best, uint_t k);
	void prune(std::deque<std::unique_ptr<bfs_solution>>& q, std::unique_ptr<bfs_solution>& best, uint_t k);
	int node_insert(std::deque<std::unique_ptr<bfs_solution>>& historyTable, int weight);
public:
	virtual std::unique_ptr<basic_solution> solve(std::shared_ptr<knapsack> inst_);
};

#endif