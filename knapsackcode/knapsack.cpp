#include "knapsack.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <memory>

/*This is a struct which is used to sort the items in non increasing order of valuePerWeight.
Sorting takes place in constructor of the knapsack.*/
class knapsackItem
{
public:
	int value;
	int weight;
	float valuePerWeight;

	knapsackItem(int value_, int weight_)
	{
		value = value_;
		weight = weight_;
		valuePerWeight = static_cast<float>(value_) / static_cast<float>(weight_);
	}
};

knapsack::~knapsack()
{
	delete values;
	delete weights;
}

knapsack::knapsack(std::ifstream& instFile_, size_t size_)
{
	m_size = size_;
	std::vector<knapsackItem> items;
	size_t itemCnt = size_;
	size_t cap;

	// Read in the size of the knapsack and the capacity
	instFile_ >> cap;

	// Allocate arrays for instance file
	m_capacity = cap;
	weights = new int[itemCnt + 1];
	values = new int[itemCnt + 1];
	weights[0] = 0;
	values[0] = 0;

	int i;
	// Read in all of the weights
	for (i = 1; i <= itemCnt; ++i)
	{
		int w;
		instFile_ >> w;
		weights[i] = w;
	}

	// Read in all of the values
	for (i = 1; i <= itemCnt; ++i)
	{
		int v;
		instFile_ >> v;
		values[i] = v;
	}

	// Put into vector to sort items
	for (i = 1; i <= itemCnt; ++i)
	{
		items.push_back(knapsackItem(values[i], weights[i]));
	}

	// Sort the list fractionally
	std::sort(items.begin(), items.end(), [](const knapsackItem& lhs, const knapsackItem& rhs)
	{
		return rhs.valuePerWeight < lhs.valuePerWeight;
	});

	// Store sorted items now
	i = 1;
	for (auto itm = items.begin(); itm != items.end(); ++itm, ++i)
	{
		weights[i] = itm->weight;
		values[i] = itm->value;
	}
}

knapsack::knapsack(size_t size_)
{
	m_size = size_;
	std::vector<knapsackItem> items;

	// Initialize values, weights array ptrs to size + 1
	weights = new int[size_ + 1];
	values = new int[size_ + 1];

	weights[0] = 0;
	values[0] = 0;

	// Generate weights for everyone and make value of each generated weight + 10 of it, use knapsackItem class and store inside of std::vector<knapsackItem>
	// Use push_back method in vector class to add things to it
	// in same loop, add up total weight, and set capacity to totalWeight / 2
	int totalWeight = 0;

	for (int i = 1; i <= size_; i++)
	{
		int weight = rand() % 100 + 1;
		int value = weight + 10;
		totalWeight += weight;
		items.push_back(knapsackItem(value, weight));
	}
	m_capacity = totalWeight / 2;

	// Sort the list fractionally
	std::sort(items.begin(), items.end(), [](const knapsackItem& lhs, const knapsackItem& rhs)
	{
		return rhs.valuePerWeight < lhs.valuePerWeight;
	});

	// Put back into values, weights arrays initialized earlier in order.
	int i = 1;
	for (auto item = items.begin(); item != items.end(); ++item, ++i)
	{
		weights[i] = item->weight;
		values[i] = item->value;
	}
}

int knapsack::capacity()
{
	return m_capacity;
}


size_t knapsack::size()
{
	return m_size;
}

int knapsack::value(int item)
{
	return values[item];
}

int knapsack::weight(int item)
{
	return weights[item];
}

const int* knapsack::weight_ptr()
{
	return weights;
}

const int* knapsack::value_ptr()
{
	return values;
}

void knapsack::print()
{
	std::cout << m_size << " " << m_capacity << std::endl;
	for (int i = 1; i <= m_size; ++i)
	{
		std::cout << weights[i] << " ";
	}

	std::cout << std::endl;
	for (int i = 1; i <= m_size; ++i)
	{
		std::cout << values[i] << " ";
	}
	std::cout << std::endl;
}


basic_solution::basic_solution(const std::shared_ptr<knapsack> inst_, bit_vector& otherSolutionVector_)
{
	// Create and initialize solution vector
  m_solutionVector = bit_vector(inst_->size() + 1);
	m_solutionVector = otherSolutionVector_;
	m_solutionVector[0] = false;
	m_size = inst_->size();

	// Add up weight and value of solution vector
	m_value = 0, m_weight = 0;
	for (int i = 1; i <= inst_->size(); ++i)
	{
		m_value += m_solutionVector[i] == true ? inst_->value(i) : 0;
		m_weight += m_solutionVector[i] == true ? inst_->weight(i) : 0;
		m_solutionVector[i] = m_solutionVector[i];
	}

	// See if solution is valid
	if (m_weight > inst_->capacity())
	{
		m_value = INVALID_VALUE;
	}
}

bool basic_solution::is_taken(int item)
{
	return m_solutionVector[item];
}

int basic_solution::value()
{
	return m_value;
}

int basic_solution::weight()
{
	return m_weight;
}

void basic_solution::print()
{
	for (size_t i = 1; i <= m_size; ++i)
	{
		if (m_solutionVector[i])
			std::cout << i << " ";
	}
	std::cout << std::endl;
}

void knapsack::save(const char* fileName)
{
	std::ofstream file(fileName, std::ios::out| std::ios::trunc);
	file << m_size << " "<< m_capacity << std::endl;
	for(int i=1; i <= m_size; i++)
	{
		file << weights[i] << " ";
	}
	file << std::endl;
	for (int i = 1; i <= m_size; i++)
	{
		file << values[i] << " ";
	}
}

