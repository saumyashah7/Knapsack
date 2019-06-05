#ifndef _BFS_SOLUTION_CUH_
#define _BFS_SOLUTION_CUH_
#include "bit_vector.h"
#include "knapsack.h"
#include <memory>

class bfs_solution
{
private:
	// Solution vector for knapsack solution
	bit_vector m_isTaken;

	// Upperbound values/weights calculated using slack fractional method
	int m_beforeSlackValue;
	int m_beforeSlackWeight;

	// Lower bound values/weights calculated using slack fractional method
	int m_lbValue;

	// Storage of slack item, or partial item that is held in upper bound value
	int m_slackItem;

	// Storage of knapsack instance for usage in solution
	std::shared_ptr<knapsack> m_inst;
#ifdef DEBUG_MODE
	std::string m_lastAction;
#endif
public:
	bfs_solution(std::shared_ptr<knapsack> inst_);
	bfs_solution(std::shared_ptr<knapsack> inst_, int lbValue, int slackItem, int beforeSlackValue, int beforeSlackWeight, bit_vector solutionVector); // FIXME: Move solutionVector instead of copy
	~bfs_solution();

	// Copies one solution into this current instance
	void copy(const std::unique_ptr<bfs_solution>& other_);

	// Gets benchmark style solution from this bfs_solution
	std::unique_ptr<basic_solution> solution();

	void update_bounds();

	// Get upper bound value
	int ub_value() const;
	// Special case for GPU
	int ub_value_gpu() const;
	int before_slack_weight() const;
	int before_slack_value() const;

	// Get Lower bound value
	int lb_value() const;

	// Get what item the slack variable is
	int slack_item() const;
	bool is_taken(uint_t item);

	// Take/dont take methods for solution
	void take(uint_t item);
	void dont_take(uint_t item);

	// Labels the solution as not promising, as it will not have a better solution
	void label_non_promising();

	// Gets the knapsack instance used by this variable
	std::shared_ptr<knapsack> knapsack_instance();
	void print();

};

#endif