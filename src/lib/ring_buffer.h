#pragma once

#include <cstddef>
#include <cstdlib>
#include <stdexcept>

namespace bacon
{
	template <typename T>
	class RingBuffer
	{
	public:
		RingBuffer(size_t max)
		{
			m_count = 0;
			m_capacity = max;
			m_head = 0;
			m_tail = 0;

			m_buffer = (T*)malloc(sizeof(T) * max);
			memset(m_buffer, 0, sizeof(T) * max);
		}

		~RingBuffer()
		{
			free(m_buffer);
		}

		void insert(const T& item)
		{
			m_buffer[m_head] = item;
			m_head = (m_head + 1) % m_capacity;

			if (m_count < m_capacity)
			{
				m_count++;
			}
			else
			{
				m_tail = (m_tail + 1) % m_capacity;
			}
		}

		T& front() const
		{
			if (m_count > 0)
			{
				return m_buffer[m_tail];
			}
			else
			{
				throw std::runtime_error("Buffer is empty");
			}
		}

		void pop()
		{
			if (m_count > 0)
			{
				m_tail = (m_tail + 1) % m_capacity;
				m_count--;
			}
		}

		T& get(size_t index) const
		{
			if (index > m_capacity || index < 0)
			{
				throw std::runtime_error("Index " + std::to_string(index) + " is out of range");
			}

			if (m_count <= 0)
			{
				throw std::runtime_error("Buffer is empty");
			}

			return m_buffer[index];
		}

		size_t size() const { return m_count; }
		size_t capacity() const { return m_capacity; }

	private:
		T* m_buffer;
		size_t m_head;
		size_t m_tail;
		size_t m_count;
		size_t m_capacity;
	};
} // namespace bacon
