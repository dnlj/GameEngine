#pragma once


// TODO: we would need to use a int counter localized because what if we had two calls to loadModel then we would have two ref counts from caching.
// Sinze we are caching this it actualy should be able to be handled. line above may be wrong
namespace engine {
	class ReferenceCounter {
		public:
			using CounterType = unsigned int;
	
			// ctor
			ReferenceCounter(CounterType init = 1);
			ReferenceCounter(CounterType* refPtr);

			// dtor
			~ReferenceCounter();

			// Copy constructor
			ReferenceCounter(const ReferenceCounter &other);

			// Move
			ReferenceCounter(const ReferenceCounter &&other);

			// Copy assign
			ReferenceCounter& ReferenceCounter::operator=(const ReferenceCounter &other);

			// Move assign
			ReferenceCounter& ReferenceCounter::operator=(const ReferenceCounter &&other);

			void increment();

			void decrement();

			CounterType getCount() const;

		private:
			CounterType* refCount;
	};
}