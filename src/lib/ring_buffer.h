#pragma once

#include <cstddef>
#include <cstdlib>

namespace bacon
{
	template <typename T> class RingBuffer
	{
	public:
		size_t count;
		size_t capacity;

		RingBuffer<T>(size_t max);
		~RingBuffer<T>();

		void insert(const T& item);
		T& front() const;
		void pop();
		T& get(size_t index) const;
		size_t size() const;

	private:
		T* buffer;
		size_t head;
		size_t tail;
	};
} // namespace bacon
