#include "bfs_solution.h"
#include <iostream>

// ************************* bfs_solution ****************************


void bfs_solution::update_bounds()
{
	// If the node has been labeled as non-promising, dont update bounds
	if (m_beforeSlackValue < 0)
	{
		return;
	}

	while (m_slackItem <= m_inst->size())
	{
		int itemWght = m_inst->weight(m_slackItem);
		int itemVal = m_inst->value(m_slackItem);

		if (m_beforeSlackWeight + itemWght <= m_inst->capacity())
		{
			// We can fit this item in completely
			m_beforeSlackWeight += itemWght;
			m_beforeSlackValue += itemVal;

			// See if we can try to fit any more items from the knapsack
			m_slackItem++;
		}
		else
		{
			break;
		}
	}

	int lbItemIndex = m_slackItem + 1;
	m_lbValue = m_beforeSlackValue;
	int lbWeight = m_beforeSlackWeight;

	while (lbWeight < m_inst->capacity() && lbItemIndex <= m_inst->size())
	{
		// See if we can fit the item into the lower bound solution
		if (m_inst->weight(lbItemIndex) <= (m_inst->capacity() - lbWeight))
		{
			lbWeight += m_inst->weight(lbItemIndex);
			m_lbValue += m_inst->value(lbItemIndex);
		}

		lbItemIndex++;
	}
}

std::unique_ptr<basic_solution> bfs_solution::solution()
{
	return std::make_unique<basic_solution>(m_inst, m_isTaken);
}

bfs_solution::bfs_solution(std::shared_ptr<knapsack> inst_)
{
	m_inst = inst_;
	m_isTaken = bit_vector(m_inst->size() + 1);
#ifdef DEBUG_MODE
	m_lastAction = "Created";
#endif

	for (int i = 0; i <= inst_->size(); ++i)
	{
		m_isTaken[i] = false;
	}

	m_lbValue = 0;

	// Set slack item to first item
	m_slackItem = 1;

	// Initialze ub values/weights to 0
	m_beforeSlackValue = 0;
	m_beforeSlackWeight = 0;
}

bfs_solution::bfs_solution(std::shared_ptr<knapsack> inst_, int lbValue, int slackItem, int beforeSlackValue, int beforeSlackWeight, bit_vector solutionVector)
{
	m_inst = inst_;
	m_lbValue = lbValue;
	m_slackItem = slackItem;
	m_beforeSlackValue = beforeSlackValue;
	m_beforeSlackWeight = beforeSlackWeight;
	m_isTaken = solutionVector;
#ifdef DEBUG_MODE
	m_lastAction = "Created from GPU node";
#endif

}

bfs_solution::~bfs_solution()
{
	
}

void bfs_solution::copy(const std::unique_ptr<bfs_solution>& other_)
{
	m_inst = other_->m_inst;

	// Copy over solution vector
	m_isTaken = other_->m_isTaken;
	m_lbValue = other_->m_lbValue;
	m_slackItem = other_->m_slackItem;
	m_beforeSlackValue = other_->m_beforeSlackValue;
	m_beforeSlackWeight = other_->m_beforeSlackWeight;
}

void bfs_solution::dont_take(uint_t item)
{
	m_isTaken[item] = false;
#ifdef DEBUG_MODE
	m_lastAction = "Dont Take: " + std::to_string(item);
#endif
	if (m_slackItem > item)
	{
		// Do normal bounds manipulation
		m_beforeSlackWeight -= m_inst->weight(item);
		m_beforeSlackValue -= m_inst->value(item);
		if (m_beforeSlackValue < 0)
		{
			std::cout << "Something" << std::endl;
		}
	}
	else
	{
		m_slackItem++;
	}
}

void bfs_solution::take(uint_t item)
{
	if (m_slackItem > item)
	{
		m_isTaken[item] = true;
#ifdef DEBUG_MODE
		m_lastAction = "Take: " + std::to_string(item);
#endif
	}
	else
	{
		label_non_promising();
	}
}



std::shared_ptr<knapsack> bfs_solution::knapsack_instance()
{
	return m_inst;
}

void bfs_solution::print()
{
	std::cout << std::endl << "m_beforeSlackValue: " << m_beforeSlackValue << " m_beforeSlackWeight: " << m_beforeSlackWeight << std::endl;
	std::cout << "m_lbValue: " << m_lbValue << std::endl;
	std::cout << "m_slackItem: " << m_slackItem << " m_ubValue: " << ub_value() << std::endl;
	std::cout << "Current Solution: " << std::endl;
#ifdef DEBUG_MODE
	std::cout << "Last action: " << m_lastAction << std::endl;
#endif
	for (int i = 1; i <= m_inst->size(); ++i)
	{
		if (m_isTaken[i])
			std::cout << i << " ";
	}

	std::cout << std::endl;
}

void bfs_solution::label_non_promising()
{
	m_beforeSlackValue = INVALID_VALUE;
	m_beforeSlackWeight = INVALID_VALUE;
}

int bfs_solution::lb_value() const
{
	return m_lbValue;
}

int bfs_solution::slack_item() const
{
	return m_slackItem;
}

bool bfs_solution::is_taken(uint_t item)
{
	return m_isTaken[item];
}

int bfs_solution::ub_value() const
{
	// This means that we have taken upto and not including slack item, so we
	// Dont need to search down this subtree anymore, prune this node
	if (m_beforeSlackValue < 0)
	{
		return INVALID_VALUE;
	}

	if (m_slackItem <= m_inst->size()) {
		int residualCapacity = m_inst->capacity() - m_beforeSlackWeight;
		float p = static_cast<float>(residualCapacity)*static_cast<float>(m_inst->value(m_slackItem)) / static_cast<float>(m_inst->weight(m_slackItem));
		return m_beforeSlackValue + p;
	}

	return static_cast<int>(m_beforeSlackValue);
}

int bfs_solution::ub_value_gpu() const
{
	if (m_beforeSlackValue < 0)
	{
		return INVALID_VALUE;
	}

	if (m_slackItem <= m_inst->size())
	{
		int residualCapacity = m_inst->capacity() - m_beforeSlackWeight;
		float p = static_cast<float>(residualCapacity)*static_cast<float>(m_inst->value(m_slackItem)) / static_cast<float>(m_inst->weight(m_slackItem));
		return static_cast<int>(ceil(m_beforeSlackValue + p));
	}

	return m_beforeSlackValue;
}

int bfs_solution::before_slack_weight() const
{
	return m_beforeSlackWeight;
}

int bfs_solution::before_slack_value() const
{
	return m_beforeSlackValue;	
}

// ************************** end bfs_solution ************************