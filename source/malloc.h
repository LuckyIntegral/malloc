#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>	  		// size_t
#include <sys/mman.h> 		// mmap + FLAGS
#include <unistd.h>	  		// getpageSize()
#include <stdint.h>	  		//  uintXX_t
#include <sys/resource.h>	// getrlimit()

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20 // cant find MAP_ANONYMOUS in sys/mman.h when running on Macs
#endif

#define BLOCK_SIZES_LEN 2
#define BLOCK_SIZES \
	{               \
		32,        \
		256,        \
	} // This have to be in ascending order to function effeciently

#define MIN_BLOCKS_IN_ZONE 100

// Zone types
#define ZONE_TYPE_SINGLE (uint8_t)1
#define ZONE_TYPE_BLOCK (uint8_t)2

typedef struct s_metadata_block
{
	uint32_t nbr_of_used_blocks;
	uint32_t max_nbr_of_blocks;
	uint32_t size_of_each_block;
	uint32_t index_of_first_alloc;
} t_metadata_block;

// Metadata functions
t_metadata_block read_metadata_block_from_array(uint8_t *ptr);
void write_metadata_block_from_array(uint8_t *ptr, t_metadata_block metadata);

typedef struct s_zone
{
	uint8_t *mem;
	uint8_t type;
	size_t size;
} t_zone;

typedef struct s_storage
{
	t_zone *zones;
	size_t capacity;
	uint32_t page_size;
	uint32_t block_sizes[BLOCK_SIZES_LEN];
	uint64_t total_allocated_memory;
} t_storage;

extern t_storage storage;

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

void *alloc(size_t size);
void dealloc(void *ptr, size_t size);

t_zone *add_zone(size_t size);

void show_alloc_mem(void);

// Helper
void write_u32_to_array(uint8_t array[], uint32_t value);
uint32_t read_u32_from_array(const uint8_t array[]);
uint8_t get_bit(uint8_t value, uint8_t pos);
void activate_bit(uint8_t *ptr, uint8_t index);
void clear_bit(uint8_t *ptr, int bit);

#endif