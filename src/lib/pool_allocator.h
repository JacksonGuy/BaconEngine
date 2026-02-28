#pragma once

#include <cstddef>
#include <cstdlib>
#include <vector>
#include <cassert>
#include <stdexcept>

namespace bacon
{
	template <typename T>
	class PoolAllocator
	{
	public:
		typedef struct Block
		{
			T* data;
			size_t size;
			size_t capacity;
			Block* prev;
			Block* next;

			~Block()
			{
				free(data);
			}

			T* get(size_t index)
			{
				if (index > size || index > capacity || index < 0)
				{
					throw std::runtime_error("Index " + std::to_string(index) + " is out of range");
				}

				return (data + index);
			}

			T* new_item()
			{
				return (data + size++);
			}
		} Block;

		PoolAllocator(size_t block_size)
		{
			m_block_size = block_size;

			m_start = new_block();

			m_end = new Block();
			m_end->data = nullptr;
			m_end->size = 0;
			m_end->capacity = 0;
			m_end->next = nullptr;

			m_start->next = m_end;
			m_end->prev = m_start;

			m_current = m_start;

			m_free_list.reserve(block_size);
		}

		~PoolAllocator()
		{
			if (m_start == nullptr)
			{
				return;
			}

			Block* curr = m_start;
			while (curr != m_end)
			{
				// Free block
				Block* next = curr->next;
				delete curr;

				// Goto next block
				curr = next;
			}
		}

		T* allocate()
		{
			if (!m_free_list.empty())
			{
				T* ptr = m_free_list.back();
				m_free_list.pop_back();
				return ptr;
			}

			if (m_current->size >= m_current->capacity)
			{
				Block* block = new_block();
				block->prev = m_current;
				m_current->next = block;
				m_current = block;
				m_end->prev = m_current;
			}

			return m_current->new_item();
		}

		void deallocate(T* ptr)
		{
			m_free_list.push_back((T*)ptr);
		}

		Block* start() { return m_start; };
		Block* end() { return m_end; };

	private:
		size_t m_block_size;
		Block* m_start;
		Block* m_end;
		Block* m_current;
		std::vector<T*> m_free_list;

		Block* new_block()
		{
			Block* block = new Block;
			block->data = (T*)malloc(sizeof(T) * m_block_size);
			block->size = 0;
			block->capacity = m_block_size;
			block->prev = nullptr;
			block->next = nullptr;

			return block;
		}
	};
} // namespace bacon
