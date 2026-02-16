#include "pool_allocator.h"

#include <cstdint>
#include <cstdlib>

namespace bacon
{
	PoolAllocator::PoolAllocator(size_t chunks_per_block)
	{
		m_chunks_per_block = chunks_per_block;
    }

	void* PoolAllocator::allocate(size_t size)
	{
		if (m_alloc_ptr == nullptr)
		{
			m_alloc_ptr = alloc_block(size);
		}


		Chunk* free_chunk = m_alloc_ptr;

		m_alloc_ptr = m_alloc_ptr->next;

		return free_chunk;
	}

	void PoolAllocator::deallocate(void* chunk, size_t size)
	{
		((Chunk*)chunk)->next = m_alloc_ptr;

		m_alloc_ptr = (Chunk*)chunk;
	}

	Chuck* PoolAllocator::alloc_block(size_t chunk_size)
	{
		size_t block_size = chunk_size * m_chunks_per_block;

		Chunk* block_begin = (Chunk*)malloc(block_size);

		Chunk* chunk = block_begin;
		for (size_t i = 0; i < m_chunks_per_block - 1; i++)
		{
			chunk->next = (Chunk*)(((uint8_t*)chunk) + chunk_size);
			chunk = chunk->next;
		}

		chunk->next = nullptr;

		return block_begin;
	}
} // namespace bacon
