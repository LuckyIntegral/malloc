#include "malloc.h"

static t_zone *get_empty_zone(void);
static int extend_zones();
static void ft_bzero(uint8_t *ptr, size_t size);
static t_zone *alloc_zone(t_zone *zone, size_t size);
static uint32_t get_max_nbr_of_blocks(size_t size, uint32_t block_size);

t_zone *add_zone(size_t size)
{
    t_zone *zone = NULL;

    zone = get_empty_zone();
    if (zone == NULL)
    {
        if (extend_zones())
            return (NULL);

        zone = get_empty_zone();

        if (zone == NULL)
            return (NULL);
    }

    return (alloc_zone(zone, size));
}

static int extend_zones()
{
    const uint8_t isNotInitialized = storage.zones == NULL || storage.capacity == 0;
    if (isNotInitialized)
    {
        storage.page_size = getpagesize();
        storage.capacity = storage.page_size / sizeof(t_zone);
        storage.zones = alloc(storage.capacity * sizeof(t_zone));
        if (storage.zones == NULL)
            return (-1);
    }
    else
    {
        // Overflow protection of capacity
        const uint8_t wouldOverflow = storage.capacity > storage.capacity * 2;
        if (wouldOverflow)
            return (-1);

        storage.capacity = storage.capacity * 2;
        t_zone *new = alloc(storage.capacity * sizeof(t_zone));
        ft_bzero((uint8_t *)new, storage.capacity * sizeof(t_zone));

        for (size_t i = 0; i < storage.capacity; i++)
        {
            new[i] = storage.zones[i];
        }

        dealloc(storage.zones, sizeof(t_zone) * (storage.capacity / 2));
        storage.zones = new;
    }
    return (0);
}

static t_zone *alloc_zone(t_zone *zone, size_t size)
{
    uint32_t block_size = 0; // 0 is default for single allocation

    for (size_t i = 0; i < BLOCK_SIZES_LEN; i++)
    {
        const uint8_t isSet = block_size != 0;
        if (isSet)
            continue;
        if (storage.block_sizes[i] >= size)
            block_size = storage.block_sizes[i];
    }

    if (block_size == 0)
    {
        zone->mem = alloc(size);
        if (zone->mem == NULL)
            return (NULL);
        zone->type = ZONE_TYPE_SINGLE;
        zone->size = size;
    }
    else
    {
        const uint64_t needed_metadata_space = 16;
        const uint64_t alignment_buffer = 16;
        const uint64_t min_needed_bookkeeping_space = needed_metadata_space + (MIN_BLOCKS_IN_ZONE / 8) + 1 + alignment_buffer;
        const uint64_t min_needed_user_space = block_size * MIN_BLOCKS_IN_ZONE + min_needed_bookkeeping_space;

        const uint64_t needed_pages = (min_needed_user_space / storage.page_size) + 1; // +1 because when dividing it rounds down.

        const uint64_t needed_space = needed_pages * storage.page_size;

        zone->mem = alloc(needed_space);
        if (zone->mem == NULL)
            return (NULL);
        zone->type = ZONE_TYPE_BLOCK;
        zone->size = needed_space;

        ft_bzero(zone->mem, needed_space);

        const uint32_t max_nbr_of_blocks = get_max_nbr_of_blocks(needed_space, block_size);
        uint32_t start_of_user_memory = needed_metadata_space + (max_nbr_of_blocks / 8) + 1;
        const uint32_t needed_alignment = start_of_user_memory % alignment_buffer ? alignment_buffer - (start_of_user_memory % (uint32_t)16) : 0;
        start_of_user_memory += needed_alignment;

        t_metadata_block metadata = {0};
        metadata.nbr_of_used_blocks = 0;
        metadata.max_nbr_of_blocks = max_nbr_of_blocks;
        metadata.size_of_each_block = block_size;
        metadata.index_of_first_alloc = start_of_user_memory;
        write_metadata_block_from_array(zone->mem, metadata);
    }

    return (zone);
}

static uint32_t get_max_nbr_of_blocks(size_t size, uint32_t block_size)
{
    if (size <= (block_size + 17))
        return (0);

    uint32_t nbr_of_max_blocks = 0;
    size -= 16;
    size -= 16;
    uint8_t bits = 0;
    size -= 1; // Starting with one less byte for bookkeeping

    while (size > block_size)
    {
        nbr_of_max_blocks++;
        size -= block_size;
        bits++;
        if (bits >= 8)
        {
            bits = 0;
            size -= 1;
        }
    }

    return (nbr_of_max_blocks);
}

static t_zone *get_empty_zone(void)
{
    size_t i = 0;

    while (i < storage.capacity)
    {
        if (storage.zones[i].mem == NULL)
            return (storage.zones + i);
        i++;
    }
    return (NULL);
}

static void ft_bzero(uint8_t *ptr, size_t size)
{
    if (ptr == NULL)
        return;
    size_t i = 0;
    while (i < storage.page_size || i < size)
    {
        ptr[i] = 0;
        i++;
    }
}