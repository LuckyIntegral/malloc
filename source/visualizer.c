#include "malloc.h"

static uint64_t show_zone(t_zone *zone);
static  uint64_t show_zone_single(t_zone *zone);
static uint64_t show_zone_block(t_zone *zone);
void	ft_putnbr_base_u_long(uint64_t nbr, char *base);

void show_alloc_mem(void)
{  
    t_zone *current = NULL;
    t_zone *previous = NULL;

    uint64_t total_used_user_memory = 0;
    do
    {
        current = NULL;
        for (size_t i = 0; i < storage.capacity; i++)
        {
            t_zone *zone = storage.zones + i;
            if (zone->mem == NULL)
                continue;

            if (previous == NULL || zone->mem > previous->mem)
            {
                if (current == NULL)
                    current = zone;
                else if (zone->mem < current->mem)
                    current = zone;
            }
        }

        total_used_user_memory += show_zone(current); 

        previous = current;
     } while (current);
    
    write(1, "Total: ", 7);
    ft_putnbr_base_u_long(total_used_user_memory, "0123456789");
    write(1, " bytes\n", 7);
}
static uint64_t show_zone(t_zone *zone)
{
    if (zone == NULL)
        return (0);
    
    if (zone->type == ZONE_TYPE_SINGLE)
        return (show_zone_single(zone));
    else if (zone->type == ZONE_TYPE_BLOCK)
        return (show_zone_block(zone));
    return (0);
}

static uint64_t show_zone_single(t_zone *zone)
{
    write(1, "LARGE : ", 8);
    ft_putnbr_base_u_long((uint64_t) zone->mem, "0123456789ABCDEF");
    write(1, "\n", 1);

    ft_putnbr_base_u_long((uint64_t) zone->mem, "0123456789ABCDEF");
    write(1, " - ", 3);
    ft_putnbr_base_u_long((uint64_t) zone->mem + zone->size -1, "0123456789ABCDEF");
    write(1, " : ", 3);
    ft_putnbr_base_u_long((uint64_t) zone->size, "0123456789");
    write(1, " bytes\n", 7);

    return ((uint64_t) zone->size);
}

static uint64_t show_zone_block(t_zone *zone)
{
    t_metadata_block meta_data = read_metadata_block_from_array(zone->mem);

    if (meta_data.size_of_each_block == storage.block_sizes[0])
        write(1, "TINY : ", 7);
    else if (meta_data.size_of_each_block == storage.block_sizes[1])
        write(1, "SMALL : ", 8);
    else
        write(1, "SPECIAL : ", 10); 

    ft_putnbr_base_u_long((uint64_t) zone->mem, "0123456789ABCDEF");
    write(1, "\n", 1);

    uint64_t allocated_user_memory = 0;
    for (size_t i = 0; i < meta_data.max_nbr_of_blocks; i++)
    {
        uint8_t byte = i / 8;
        uint8_t bit = i % 8;

        uint8_t isSet = get_bit((uint64_t) zone->mem[16 + byte], bit);
        if (isSet)
        {
            void *alloc_start =  zone->mem + meta_data.index_of_first_alloc + (i * meta_data.size_of_each_block);

            ft_putnbr_base_u_long((uint64_t) alloc_start, "0123456789ABCDEF");
            write(1, " - ", 3);
            ft_putnbr_base_u_long((uint64_t) alloc_start + meta_data.size_of_each_block -1 , "0123456789ABCDEF");
            write(1, " : ", 3);
            ft_putnbr_base_u_long(meta_data.size_of_each_block, "0123456789");
            write(1, " bytes\n", 7);

            allocated_user_memory += meta_data.size_of_each_block;
        }
    }
    return (allocated_user_memory); 
}