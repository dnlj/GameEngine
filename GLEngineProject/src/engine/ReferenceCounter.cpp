// STD
#include <iostream>
#include <vector>

// Engine
#include <engine/ReferenceCounter.hpp>


namespace engine {
	ReferenceCounter::ReferenceCounter(CounterType init) {
		refCount = new CounterType;
		*refCount = init;
	};

	ReferenceCounter::ReferenceCounter(CounterType* refPtr) {
		refCount = refPtr;
	}

	ReferenceCounter::~ReferenceCounter() {
		decrement();
		if (*refCount == 0) {
			delete refCount;
		}
	};

	ReferenceCounter::ReferenceCounter(const ReferenceCounter &other) {
		refCount = other.refCount;
		increment();
	}

	ReferenceCounter::ReferenceCounter(const ReferenceCounter &&other) {
		refCount = other.refCount;
		increment();
	}

	ReferenceCounter& ReferenceCounter::ReferenceCounter::operator=(const ReferenceCounter &other) {
		decrement();

		refCount = other.refCount;

		increment();
		return *this;
	}

	ReferenceCounter& ReferenceCounter::ReferenceCounter::operator=(const ReferenceCounter &&other) {
		decrement();
	
		refCount = other.refCount;
	
		increment();

		return *this;
	}

	ReferenceCounter::CounterType ReferenceCounter::getCount() const {
		return *refCount;
	}

	void ReferenceCounter::increment() {
		*refCount += 1;
	}

	void ReferenceCounter::decrement() {
		if (*refCount > 0) {
			*refCount -= 1;
		}
	}
}