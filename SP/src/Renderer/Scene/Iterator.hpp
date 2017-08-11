#ifndef _SP_ITERATOR_HPP_
#define _SP_ITERATOR_HPP_

#include <exception>

namespace SP {


	// Iterator interface
	class Iterator {

	public:

		Iterator() = default;

		virtual ~Iterator() = default;

		virtual void reset() = 0;

		virtual bool hasNext() const = 0;
		virtual const void* nextItem() = 0;

		// Disable copy and 
		Iterator(const Iterator&) = delete;
		Iterator& operator=(const Iterator&) = delete;
	};


	template <typename IteratorType>
	class GenericIterator : public Iterator {
	public:

		GenericIterator(IteratorType b, IteratorType e) : begin(b), end(e), current(b) {

		}


		virtual void reset() override {
			current = begin;
		}

		virtual bool hasNext() const override {
			return current != end;
		}

		virtual const void * nextItem() override {

			if (hasNext()) {
				return *(current++);
			}

			throw std::runtime_error("Iterator: No more NEXT");

		}

	private:
		IteratorType begin;
		IteratorType end;
		IteratorType current;
	};

}
















#endif

