#include "ring_buffer.h"

#include <stdexcept>

namespace bacon
{
	template <typename T>
	RingBuffer<T>::RingBuffer(size_t max)
	{
		this->count = 0;
		this->capacity = max;
		this->head = 0;
		this->tail = 0;

		this->buffer = (T*)malloc(sizeof(T) * max);
		memset(this->buffer, 0, sizeof(T) * max);
	}

	template <typename T>
	RingBuffer<T>::~RingBuffer()
	{
		free(buffer);
	}

	template <typename T>
	void RingBuffer<T>::insert(const T& item)
	{
		this->buffer[this->head] = item;
		this->head = (this->head + 1) % this->capacity;

		if (this->count < this->capacity)
		{
			this->count++;
		}
		else
		{
			this->tail = (this->tail + 1) % this->capacity;
		}
	}

	template <typename T>
	T& RingBuffer<T>::front() const
	{
		if (this->count > 0)
		{
			return this->buffer[this->tail];
		}
		else
		{
			throw std::runtime_error("Buffer is empty");
		}
	}

	template <typename T>
	void RingBuffer<T>::pop()
	{
		if (this->count > 0)
		{
			this->tail = (this->tail + 1) % this->capacity;
			this->count--;
		}
	}

	template <typename T>
	T& RingBuffer<T>::get(size_t index) const
	{
		if (index > this->capacity || index < 0)
		{
			throw std::runtime_error("Index " + std::to_string(index) +
									 " is out of range");
		}

		if (this->count <= 0)
		{
			throw std::runtime_error("Buffer is empty");
		}

		return this->buffer[index];
	}

	template <typename T>
	size_t RingBuffer<T>::size() const
	{
		return this->count;
	}
} // namespace bacon
