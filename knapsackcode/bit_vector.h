#ifndef BIT_VECTOR_CUH
#define BIT_VECTOR_CUH
#include <math.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cstring>

typedef unsigned int unit;

// Simply generate
// We create 3 methods.All return void except. They generate 1 million variables put them in a bit vector. 
class bit_vector {
public:
	// The actual integral type that is used to store the bits.
	struct bit;

	// Constructs a bit vector of a given length.
	bit_vector(int length = 0);
	// Deallocates the vector.
	virtual ~bit_vector();

	// Reconstructs the vector to hold a vector of the new length. All old data
	// is discarded.
	void construct(int length);

	// Returns the number of bits in the vector.
	int get_size() const;

	// Assigns the values from src to the vector. Both vectors must be of the
	// same size.
	bit_vector &operator=(const bit_vector &src);

	// Sets the bit at the given index to the given value.
	void set_bit(int index, bool value = true);
	// Returns the value of the bit at a given index.
	bool get_bit(int index) const;

	bit operator[](const int index);

protected:
	// The buffer in which the bits are stored.
	unit *vctr_;
	// The number of bits.
	int bitCnt_;
	// The number of units of the actual integer data type used.
	int unitCnt_;
	// The number of ones currently in the vector.
	int oneCnt_;

	// Gets a Unit-sized bitmask for a given bit, inverted if val = false.
	static unit get_mask(int bitNum, bool val);

	// The number of bits per storage unit.
	static const int BITS_IN_UNIT = sizeof(unit) * 8;
};

struct bit_vector::bit {
	bit_vector* bitVec;
	int index_;

	bit() {
		bitVec = nullptr;
		index_ = 0;
	}

	bit(bit_vector& ref, int index) {
		bitVec = &ref;
		index_ = index;
	}

	bit(const bit& rhs) : index_(rhs.index_), bitVec(rhs.bitVec) { }

	inline operator bool() const {
		return bitVec->get_bit(index_);
	}

	inline bit& operator=(bool value) {
		bitVec->set_bit(index_, value);
		return *this;
	}

	inline bit& operator=(const bit& value) {
		this->index_ = value.index_;
		this->bitVec = value.bitVec;
		return *this;
	}
};

inline bit_vector::~bit_vector() {
	if (vctr_ != NULL)
		delete[] vctr_;
}

inline bit_vector::bit_vector(int length) {
	bitCnt_ = 0;
	unitCnt_ = 0;
	oneCnt_ = 0;
	vctr_ = NULL;
	construct(length);
}

inline bit_vector &bit_vector::operator=(const bit_vector &src) {
  construct(src.get_size());
	int byteCnt = src.unitCnt_ * sizeof(unit);
	memcpy(vctr_, src.vctr_, byteCnt);
	oneCnt_ = src.oneCnt_;
	return *this;
}

inline void bit_vector::construct(int length) {
	bitCnt_ = length;
	unitCnt_ = (bitCnt_ + BITS_IN_UNIT - 1) / BITS_IN_UNIT;

	if (unitCnt_ == 0)
		return;

	if (vctr_)

		delete[] vctr_;
	vctr_ = new unit[unitCnt_];

	for (int i = 0; i < unitCnt_; i++) {
		vctr_[i] = 0;
	}

	oneCnt_ = 0;
}

inline unit bit_vector::get_mask(int bitNum, bool bitVal) {
	unit mask = ((unit)1) << bitNum;

	if (!bitVal) {
		// The mask for setting a bit to 0 is the inverse of the mask for setting a
		// bit to 1. E.g. ORing with the mask 0x0008 sets the fourth bit to 1 while
		// ANDing with the mask 0xfff7 sets the fourth bit to 0.
		mask = ~mask;
	}

	return mask;
}

inline bit_vector::bit bit_vector::operator[](const int index) {
	return bit(*this, index);
}

inline int bit_vector::get_size() const { return bitCnt_; }

inline void bit_vector::set_bit(int index, bool bitVal) {
	int unitNum = index / BITS_IN_UNIT;
	int bitNum = index - unitNum * BITS_IN_UNIT;
	unit mask = get_mask(bitNum, bitVal);

	if (bitVal) {
		if (get_bit(index) == false)
			oneCnt_++;
		vctr_[unitNum] |= mask;
	}
	else {
		if (get_bit(index) == true)
			oneCnt_--;
		vctr_[unitNum] &= mask;
	}
}

inline bool bit_vector::get_bit(int index) const {
	int unitNum = index / BITS_IN_UNIT;
	int bitNum = index - unitNum * BITS_IN_UNIT;
	return (vctr_[unitNum] & get_mask(bitNum, true)) != 0;
}

#endif