#pragma once

#include <cstddef>

namespace bacon
{
	typedef struct Chuck
	{
		Chuck* next;
	} Chunk;

	class PoolAllocator
	{
	public:
        PoolAllocator(size_t chunks_per_block);
		void* allocate(size_t size);
		void deallocate(void* ptr, size_t size);

	private:
        size_t m_chunks_per_block;
        size_t m_chuck_size;

		Chunk* m_alloc_ptr = nullptr;
        Chunk* alloc_block(size_t size);
	};
} // namespace bacon
