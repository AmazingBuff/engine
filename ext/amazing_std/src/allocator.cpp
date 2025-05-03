#include <astd/base/util.h>
#include <astd/base/except.h>
#include <astd/memory/allocator.h>

AMAZING_NAMESPACE_BEGIN


struct MemoryHeaderInfo
{
    size_t position;
    size_t size;        // exclude header
    size_t offset;      // available memory offset

    MemoryHeaderInfo* next;
    MemoryHeaderInfo* prev;

    void* data;         // for user data
};

constexpr static size_t k_memory_header_size = align_to(sizeof(MemoryHeaderInfo), k_cache_alignment);


class IMemoryPool
{
public:
    explicit IMemoryPool(size_t size = k_local_memory_size);
    ~IMemoryPool();

    void* allocate(size_t size, void* data);
    void* allocate(void* p, size_t size, void* data);
    void deallocate(void* p);
private:
    uint8_t* m_data;
    size_t m_size;
    MemoryHeaderInfo* m_current_info;
};

IMemoryPool::IMemoryPool(size_t size) : m_current_info(nullptr)
{
    m_data = new uint8_t[size];
    if (!m_data)
        throw AStdException(AStdError::NO_ENOUGH_MEMORY);
    memset(m_data, 0, size);
    m_size = size;
}

IMemoryPool::~IMemoryPool()
{
    delete[] m_data;
    m_data = nullptr;
}

void* IMemoryPool::allocate(size_t size, void* data)
{
    size_t align_size = align_to(size, k_cache_alignment);
    if (m_current_info)
    {
        MemoryHeaderInfo* iterator = m_current_info;
        do
        {
            if (iterator->offset + align_size + k_memory_header_size <= iterator->size)
            {
                MemoryHeaderInfo* header = new (m_data + iterator->position + k_memory_header_size + iterator->offset) MemoryHeaderInfo;
                header->prev = iterator;
                header->next = iterator->next;
                header->size = iterator->size - iterator->offset - k_memory_header_size;
                header->offset = align_size;
                header->position = iterator->position + k_memory_header_size + iterator->offset;
                header->data = data;

                iterator->size = iterator->offset;
                iterator->next->prev = header;
                iterator->next = header;

                m_current_info = header;

                break;
            }
            iterator = iterator->next;
        } while (iterator != m_current_info);

        if (m_current_info == iterator)
            throw AStdException(AStdError::NO_APPLICABLE_POSITION);
    }
    else
    {
        if (m_size < align_size + k_memory_header_size)
            throw AStdException(AStdError::NO_ENOUGH_MEMORY);

        m_current_info = new (m_data) MemoryHeaderInfo;
        m_current_info->prev = m_current_info;
        m_current_info->next = m_current_info;
        m_current_info->offset = align_size;
        m_current_info->position = 0;
        m_current_info->size = m_size - k_memory_header_size;
        m_current_info->data = data;
    }

    return m_data + m_current_info->position + k_memory_header_size;
}

void* IMemoryPool::allocate(void* p, size_t size, void* data)
{
    if (p == nullptr)
        return allocate(size, data);

    MemoryHeaderInfo* header = reinterpret_cast<MemoryHeaderInfo*>(reinterpret_cast<uint8_t*>(p) - k_memory_header_size);
    if (size <= header->size)
    {
        header->offset = align_to(size, k_cache_alignment);
        header->data = data;
        return p;
    }

    // free and find appropriate position
    MemoryHeaderInfo* prev = header->prev;
    prev->size = prev->size + k_memory_header_size + header->size;
    prev->next = header->next;

    header->next->prev = prev;

    return allocate(size, data);
}

void IMemoryPool::deallocate(void* p)
{
    if (p == nullptr)
        return;

    MemoryHeaderInfo* header = reinterpret_cast<MemoryHeaderInfo*>(reinterpret_cast<uint8_t*>(p) - k_memory_header_size);
    if (header->position != 0)
    {
        MemoryHeaderInfo* prev = header->prev;
        prev->size = prev->size + k_memory_header_size + header->size;
        prev->next = header->next;

        header->next->prev = prev;
        if (m_current_info == header)
            m_current_info = prev;
        std::memset(header, 0, k_memory_header_size + header->offset);
    }
    else
        header->offset = 0;
}


static thread_local IMemoryPool t_local_pool;


void* allocate(size_t size, void* data)
{
    return t_local_pool.allocate(size, data);
}

void* allocate(void* p, size_t size, void* data)
{
    return t_local_pool.allocate(p, size, data);
}

void deallocate(void* p)
{
    t_local_pool.deallocate(p);
}


AMAZING_NAMESPACE_END