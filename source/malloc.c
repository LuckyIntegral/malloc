#include "malloc.h"

t_storage storage = {
    .zones = NULL,
    .capacity = 0,
    .page_size = 0,
    .block_sizes = BLOCK_SIZES,
    .total_allocated_memory = 0,
};

static void *get_block_memory(uint32_t block_type);
static void *get_single_memory(size_t size);
static uint8_t retrieve_page_size(void);

void *malloc(size_t size)
{
    if (size == 0)
        return (NULL);

    if (retrieve_page_size())
        return (NULL);

    for (size_t i = 0; i < BLOCK_SIZES_LEN; i++)
    {
        
        const uint32_t block_size = storage.block_sizes[i];
        if (size <= block_size)
        {
            void *res = get_block_memory(block_size);
            if (res != NULL)
                return (res);
            add_zone(size);
            return (get_block_memory(block_size));
        }
    }

    return (get_single_memory(size));
}

static void *get_block_memory(uint32_t block_type)
{
    for (size_t i = 0; i < storage.capacity; i++)
    {
        const t_zone *zone = storage.zones + i;
        if (zone->mem == NULL)
            continue;

        if (zone->type != ZONE_TYPE_BLOCK)
            continue;

        t_metadata_block metadata = read_metadata_block_from_array(zone->mem);
        if (metadata.size_of_each_block != block_type)
            continue;
        const uint32_t isEnoughSpace = metadata.nbr_of_used_blocks < metadata.max_nbr_of_blocks;
        if (!isEnoughSpace)
            continue;

        const uint32_t metadata_offset = 16;
        for (uint32_t i = 0; i < metadata.max_nbr_of_blocks; i++) // 16 is after the metadata and the start of the bookkeeping
        {
            uint32_t bit = i % 8;
            uint32_t byte = i / 8;

            const uint8_t isAlreadyUsed = get_bit(zone->mem[metadata_offset + byte], bit);
            if (isAlreadyUsed)
                continue;

            activate_bit(zone->mem + metadata_offset + byte, bit);
            metadata.nbr_of_used_blocks++;
            write_metadata_block_from_array(zone->mem, metadata);

            void *user_memory = zone->mem + metadata.index_of_first_alloc + (i * metadata.size_of_each_block);
            return (user_memory);
        }
    }

    return (NULL);
}

static void *get_single_memory(size_t size)
{
    t_zone *new = add_zone(size);
    if (new == NULL)
        return (NULL);
    return new->mem;
}

static uint8_t retrieve_page_size()
{
    if (storage.page_size == 0)
    {
        long page_size = sysconf(_SC_PAGESIZE);
        if (page_size == -1)
            return (1);
        storage.page_size = page_size;
    }
    return (0);
}