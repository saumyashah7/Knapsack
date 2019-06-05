#include "bfs_alg.h"
#include <memory>
#include <queue>
#include <iostream>
#include <cassert>

// ************************* bfs_solver ******************************

std::unique_ptr<basic_solution> bfs_solver::solve(std::shared_ptr<knapsack> inst_)
{
	m_inst = inst_;

	// Create storage container for solutions and best solution
	std::unique_ptr<bfs_solution> best = std::make_unique<bfs_solution>(inst_);
	std::deque<std::unique_ptr<bfs_solution>> q;
	uint_t k = 0;

	// Initial solution where we take all of the most profitable items first is initial best also
	auto init = std::make_unique<bfs_solution>(inst_);
	init->update_bounds();
	q.push_back(std::move(init));

	while (++k <= m_inst->size() && !q.empty())
	{
		branch(q, k);
		bound(q, best, k);
		prune(q, best, k);
	}

	assert(best->solution()->value() == best->lb_value());

	return best->solution();
}

void bfs_solver::prune_nodes_dfs(std::deque<std::unique_ptr<bfs_solution>>& q_)
{
	// TODO: Port
}

void bfs_solver::branch(std::deque<std::unique_ptr<bfs_solution>>& q, uint_t k)
{
	// Go through complete queue and branch all of the nodes
	int qSize = q.size();
	for (int i = 0; i < qSize; ++i)
	{
		auto parentSoln = std::move(q.front());
		q.pop_front();

		// Make a copy of the solution
		auto sonSoln = std::make_unique<bfs_solution>(m_inst);
		sonSoln->copy(parentSoln);

		// Now do branch operation by taking/not taking items
		parentSoln->take(k);
		sonSoln->dont_take(k);

		q.push_back(std::move(parentSoln));
		q.push_back(std::move(sonSoln));
	}
}


void bfs_solver::bound(std::deque<std::unique_ptr<bfs_solution>>& q, std::unique_ptr<bfs_solution>& best, uint_t k)
{
	int qSize = q.size();

	for (int i = 0; i < qSize; ++i)
	{
		auto soln = std::move(q.front());
		q.pop_front();

		// Update bounds
		soln->update_bounds();

		// See if it is better than the best, if it is, update best
		if (soln->lb_value() > best->lb_value())
		{
#ifdef DEBUG_MODE_CPU
			std::cout << std::endl << "==================" << std::endl << "New best node values: " << std::endl;
			soln->print();
			std::cout << "==================" << std::endl;
#endif
			best->copy(soln);
		}

		q.push_back(std::move(soln));
	}

	if (k < best->slack_item())
	{
		// Means the solution was not finished
		for (int i = k + 1; i < best->slack_item(); ++i)
		{
			best->take(i);
		}

		// Take any other items that might fit after slack items
		int w = m_inst->capacity() - best->before_slack_value();
		for (int i = best->slack_item() + 1; i < m_inst->size(); ++i)
		{
			if (w >= m_inst->weight(i))
			{
				best->take(i);
				w -= m_inst->weight(i);
			}
		}
	}
}
void bfs_solver::prune(std::deque<std::unique_ptr<bfs_solution>>& q, std::unique_ptr<bfs_solution>& best, uint_t k)
{
	int qSize = q.size();
	std::deque<std::unique_ptr<bfs_solution>> historyTable;


	for (int i = 0; i < qSize; ++i)
	{
		auto soln = std::move(q.front());
		q.pop_front();
		int soln_ub_value = soln->ub_value();
		int soln_before_slack_value = soln->before_slack_value();
		int soln_before_slack_weight = soln->before_slack_weight();
		
		if (soln_ub_value > best->lb_value())/*If it is a valid solution*/
		{
			if (historyTable.size() == 0)
			{
				historyTable.push_back(std::move(soln));

			}
			else
			{
				/*comparing with the last element and placing at the last*/
				auto last = historyTable[historyTable.size()-1].get();
				if (last->before_slack_weight() < soln_before_slack_weight)
				{
					if (soln_before_slack_value <= last->before_slack_value())
					{
						break;
					}
					else
					{
						historyTable.push_back(std::move(soln));
					}
				}
				else
				{ /*checking if we can place at the beginning of the table*/
					auto first = historyTable[0].get();
					if (first->before_slack_weight() > soln_before_slack_weight)
					{
						if (soln_before_slack_value < first->before_slack_value())
						{
							historyTable.push_front(std::move(soln));
						}
						else
						{
							int index = 0;
							while (historyTable.at(index)->before_slack_value() <= soln_before_slack_value)
							{
								index++;
							}
							historyTable.erase(historyTable.begin(), historyTable.begin() + index - 1);
							historyTable.push_front(std::move(soln));
						}
					}
					else {
						int n = node_insert(historyTable, soln_before_slack_weight);
						if (historyTable.at(n)->before_slack_weight == soln_before_slack_weight)
						{
							auto equal = std::move(historyTable.at(n));
							auto equal_bsv = equal->before_slack_value();


							if (soln_before_slack_value <= equal_bsv)
							{
								std::swap(equal, historyTable.at(n));
								break;
							}

							else
							{
								/*std::unique_ptr<bfs_solution> m;
								std::swap(m, historyTable.at(mid));*/
								equal->copy(soln);
								std::swap(equal, historyTable.at(n));
								int index = n + 1;
								while (historyTable.at(index)->before_slack_value() <= soln_before_slack_value)
								{
									index++;
								}
								historyTable.erase(historyTable.begin() + n + 1, historyTable.begin() + index - 1);
							}

						}
						else
						{
							auto less = std::move(historyTable.at(n - 1));
							auto less_bsv = less->before_slack_value();
							if (soln_before_slack_value <= less_bsv)
							{
								std::swap(less, historyTable.at(n - 1));
								break;
							}
							else
							{
								auto high = std::move(historyTable.at(n));
								auto high_bsv = high->before_slack_value();
								if(soln_before_slack_value >= high_bsv)
								{
									high->copy(soln);
									std::swap(high, historyTable.at(n));
									int index = n;
									while (historyTable.at(index)->before_slack_value() <= soln_before_slack_value)
									{
										index++;
									}
									historyTable.erase(historyTable.begin() + n, historyTable.begin() + index - 1);

								}
								else
								{
									historyTable.insert(historyTable.begin() + n, std::move(soln));
								}
							}
						

						}
					

						}
					}
				}
			}
		}
		q = historyTable;

	}


int bfs_solver::node_insert(std::deque<std::unique_ptr<bfs_solution>>& historyTable, int weight)
{
		int lowerBound = 0;
		int upperBound = historyTable.size() - 1;
		int curIn = 0;
		while (true) {
			curIn = (upperBound + lowerBound) / 2;
			if (historyTable.at(curIn)->before_slack_weight == weight) {
				return curIn;
			}
			else if (historyTable.at(curIn)->before_slack_weight < weight) {
				lowerBound = curIn + 1; // its in the upper
				if (lowerBound > upperBound)
					return curIn + 1;
			}
			else {
				upperBound = curIn - 1; // its in the lower
				if (lowerBound > upperBound)
					return curIn;
			}
		}
	}





// ************************* end bfs_solver **************************
