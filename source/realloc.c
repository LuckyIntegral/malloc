#include "malloc.h"

t_zone *get_zone(uint8_t *ptr);

static uint32_t get_size_of_zone(t_zone *zone)
{
    uint32_t previous_size = 0;

    if (zone->type == ZONE_TYPE_BLOCK)
    {
        t_metadata_block metadata = read_metadata_block_from_array(zone->mem);
        previous_size = metadata.size_of_each_block;
    }
    else if (zone->type == ZONE_TYPE_SINGLE)
    {
        previous_size = zone->size;
    }

    return (previous_size);
}

void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
        return (malloc(size));

    t_zone *zone = get_zone(ptr);
    if (zone == NULL)
        return (malloc(size));

    uint32_t previous_size = get_size_of_zone(zone);

    if (previous_size >= size)
        return (ptr);

    uint8_t *new_ptr = malloc(size);
    if (new_ptr == NULL)
        return (NULL);
    for (size_t i = 0; i < previous_size; i++)
    {
        new_ptr[i] = ((uint8_t *)ptr)[i];
    }

    free(ptr);
    return (new_ptr);
}