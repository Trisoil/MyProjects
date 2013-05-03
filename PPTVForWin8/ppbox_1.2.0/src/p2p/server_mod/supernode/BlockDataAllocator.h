//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_BLOCK_DATA_ALLOCATOR_H
#define SUPER_NODE_BLOCK_DATA_ALLOCATOR_H

namespace super_node
{
    class MemoryChunk
    {
        static const size_t BlocksPerChunk;
        static const size_t ChunkSize;

    public:
        MemoryChunk();
        ~MemoryChunk();

        bool Owns(void* memory);
        bool Free(void* memory);
        void* TryAllocate();

#ifdef USE_DIRECT_IO
        static size_t GetPageSize();
#endif

    private:
        char* blocks_;
        std::deque<char*> available_blocks_;
    };

    class BlockDataAllocator
    {
    private:
        BlockDataAllocator()
        {}

    public:
        static BlockDataAllocator& Instance()
        {
            static BlockDataAllocator allocator;
            return allocator;
        }

        void* Allocate();
        bool Free(void* buffer);

        void LoadConfig();

    private:
        std::vector<boost::shared_ptr<MemoryChunk> > chunks_;
        boost::mutex mutex_;
    };
}

#endif //SUPER_NODE_BLOCK_DATA_ALLOCATOR_H
