#include "malloc.h"

static int check_alloc_size(size_t size)
{
    struct rlimit limit = {0};

    if (getrlimit(RLIMIT_AS, &limit))
        return (1);
    if (storage.total_allocated_memory + size > limit.rlim_cur)
        return (2);

    return (0);
}

void *alloc(size_t size)
{
    if (size % storage.page_size != 0)
        size = ((size / storage.page_size) + 1) * storage.page_size;

    size_t *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED)
        return (NULL);
    if (check_alloc_size(size))
        return (NULL);
    storage.total_allocated_memory += size;
    return ptr;
}

void dealloc(void *ptr, size_t size)
{
    if (size == 0)
        return;
    if (size % storage.page_size != 0)
        size = ((size / storage.page_size) + 1) * storage.page_size;
        
    if (ptr != NULL && size > 0)
    {
        if (munmap(ptr, size))
            return;
        storage.total_allocated_memory -= size;
    }
}

void write_u32_to_array(uint8_t array[], uint32_t value)
{
    array[0] = (value >> 24) & 0xFF; // Most significant byte
    array[1] = (value >> 16) & 0xFF;
    array[2] = (value >> 8) & 0xFF;
    array[3] = value & 0xFF; // Least significant byte
}

uint32_t read_u32_from_array(const uint8_t array[])
{
    uint32_t value = 0;
    value |= (uint32_t)array[0] << 24; // Most significant byte
    value |= (uint32_t)array[1] << 16;
    value |= (uint32_t)array[2] << 8;
    value |= (uint32_t)array[3]; // Least significant byte
    return value;
}

t_metadata_block read_metadata_block_from_array(uint8_t *ptr)
{
    t_metadata_block data = {0};

    data.nbr_of_used_blocks = read_u32_from_array(ptr);
    data.max_nbr_of_blocks = read_u32_from_array(ptr + 4);
    data.size_of_each_block = read_u32_from_array(ptr + 8);
    data.index_of_first_alloc = read_u32_from_array(ptr + 12);

    return (data);
}
void write_metadata_block_from_array(uint8_t *ptr, t_metadata_block data)
{
    write_u32_to_array(ptr, data.nbr_of_used_blocks);
    write_u32_to_array(ptr + 4, data.max_nbr_of_blocks);
    write_u32_to_array(ptr + 8, data.size_of_each_block);
    write_u32_to_array(ptr + 12, data.index_of_first_alloc);
}

uint8_t get_bit(uint8_t value, uint8_t pos)
{
    return ((value & (1 << (7 - pos))) != 0);
}

void activate_bit(uint8_t *ptr, uint8_t index)
{
    *ptr = *ptr | (0x1 << (0x7 - index));
}

void clear_bit(uint8_t *ptr, int bit)
{
    if (ptr == NULL)
    {
        return;
    }

    if (bit < 0 || bit > 7)
    {
        return;
    }

    uint8_t mask = ~(1 << (0x7 - bit));
    *ptr &= mask;
}