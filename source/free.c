#include "malloc.h"

t_zone *get_zone(uint8_t *ptr);
static void free_single(t_zone *zone);
static void free_block(t_zone *zone, uint8_t *ptr);
static void dealloc_zone(t_zone *zone);

void free(void *ptr)
{
    if (ptr == NULL)
        return;

    t_zone *zone = get_zone(ptr);
    if (zone == NULL)
        return;

    if (zone->type == ZONE_TYPE_SINGLE)
        free_single(zone);
    else if (zone->type == ZONE_TYPE_BLOCK)
        free_block(zone, ptr);
}

static void free_single(t_zone *zone)
{
    dealloc_zone(zone);
}

void free_block(t_zone *zone, uint8_t *ptr)
{
    t_metadata_block metadata = read_metadata_block_from_array(zone->mem);
    const uint32_t relative_ptr = ptr - (zone->mem + metadata.index_of_first_alloc);
    const uint32_t index = relative_ptr / metadata.size_of_each_block;

    const uint8_t byte = index / 8;
    const uint8_t bit = index % 8;

    clear_bit(zone->mem + byte + 16, bit);

    metadata.nbr_of_used_blocks--;
    write_metadata_block_from_array(zone->mem, metadata);

    if (metadata.nbr_of_used_blocks == 0)
        dealloc_zone(zone);
}

static void dealloc_zone(t_zone *zone)
{
    dealloc(zone->mem, zone->size);
    zone->mem = NULL;
    zone->size = 0;
    zone->type = 0;
}

t_zone *get_zone(uint8_t *ptr)
{
    t_zone *zone = NULL;
    for (size_t i = 0; i < storage.capacity; i++)
    {
        t_zone *tmp = storage.zones + i;
        if (ptr >= tmp->mem && ptr <= (tmp->mem + tmp->size))
        {
            zone = tmp;
            break;
        }
    }
    return (zone);
}