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
    uint8_t*            m_data;
    size_t              m_size;
    MemoryHeaderInfo*   m_current_info;
};

IMemoryPool::IMemoryPool(size_t size) : m_current_info(nullptr)
{
    m_data = new uint8_t[size];
    if (!m_data)
        throw AStdException(AStdError::NO_ENOUGH_MEMORY);
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

    size_t offset = reinterpret_cast<uint8_t*>(p) - m_data;
    MemoryHeaderInfo* iterator = m_current_info;
    do
    {
        // input address must equal to pos
        if (iterator->position + k_memory_header_size == offset)
        {
            if (size <= iterator->size)
            {
                iterator->offset = align_to(size, k_cache_alignment);
                iterator->data = data;
                return p;
            }
            else
            {
                // deallocate
                MemoryHeaderInfo* prev = iterator->prev;
                prev->size = prev->size + k_memory_header_size + iterator->size;
                prev->next = iterator->next;

                iterator->next->prev = prev;
                
                return allocate(size, data);
            }
        }
        iterator = iterator->next;
    } while (iterator != m_current_info);

    throw AStdException(AStdError::NO_VALID_PARAMETER);
}

void IMemoryPool::deallocate(void* p)
{
    if (p == nullptr)
        return;

    size_t offset = reinterpret_cast<uint8_t*>(p) - m_data;
    MemoryHeaderInfo* iterator = m_current_info;
    do
    {
        if (iterator->position + k_memory_header_size == offset)
        {
            MemoryHeaderInfo* prev = iterator->prev;
            prev->size = prev->size + k_memory_header_size + iterator->size;
            prev->next = iterator->next;

            iterator->next->prev = prev;

            m_current_info = prev;

            break;
        }

        iterator = iterator->next;
    } while (iterator != m_current_info);
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