#ifndef MEMORY__MANAGEMENT__HEADER__GUARD
#define MEMORY__MANAGEMENT__HEADER__GUARD

#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

//structure of the header to the memory block
typedef struct block_header{
    bool free;
    size_t size;
    struct block_header *next;
}block_header;
//define size of the header
#define HEADER_SIZE sizeof(struct block_header)

size_t align(size_t size);

block_header *get_free_block(size_t size, block_header **previous);

block_header *get_new_block(size_t size, block_header *previous);

void * _malloc(size_t size);

void _free(void * ptr);

#endif