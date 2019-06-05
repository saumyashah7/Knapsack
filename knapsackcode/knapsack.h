#include <memory>
#ifndef _KNAPSACK_CUH_
#define _KNAPSACK_CUH_
#include <string>
#include "bit_vector.h"

#ifdef _DEBUG
#define DEBUG_MODE
//#define DEBUG_MODE_CPU
#define DEBUG_MODE_GPU
#endif

#define INVALID_VALUE -1

typedef unsigned int uint_t;
#define MAX_KNAPSACK_SIZE 511

class knapsack
{
private:
	int* weights;// weights of the knapsack instance
	int* values;// values of the knpasack instance
	int m_capacity; // capacity of the knapsack instance
	size_t m_size;//size of the knapsack instance
public:
	knapsack(size_t size_);
	knapsack(std::ifstream& file_, size_t size_); //to get the knapsack instance from a file that is passed as one of the arguments
	~knapsack();
	size_t size();// size of the knapsack
	int capacity();//capacity of the knapsack
	int value(int item);// to return the value of the particular item
	int weight(int item);// to return weight of the particular item
	const int* weight_ptr();// return all the weights
	const int* value_ptr();//return all the values
	void print();// This method prints knapsack instance--- size, capacity, all the weights, all the values.
	void save(const char* fileName);// This method saves the knapsack instance to a file so that it is easy to debug the specific instances.
};



class basic_solution
{

	/*A basic solution usually has a solution vector, total value that it gives,and total weight for the items it takes.*/
private:
	bit_vector m_solutionVector;
	size_t m_size;
	int m_value;
	int m_weight;
public:
	basic_solution(const std::shared_ptr<knapsack> inst_, bit_vector& otherSolutionVector_);
	
	int value();// to return the sum of the values of all the items taken in the solution vector
	int weight();// to return the sum of the weights of all the the items taken in solution vector
	bool is_taken(int item);// it returns 1 if the item is taken and 0 if the item is not taken
	void print();// it returns all the items that are taken
};

#endif
