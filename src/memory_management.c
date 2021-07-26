#include "memory_management.h"

//head for the linked list
void *base_pointer = NULL;

size_t align(size_t size){
    //align the size to fit a wordsize on the given system (8 in 64bit and 4 in 32bit)
    //example size is 1101 (13)
    //plus the word-1 0111 (7)
    //is equal to    10100 (20)
    //then take that and bitwise and it with the opposite of the word
    //the opposite of word-1  11000 (24)
    //the size plus word-1    10100 (20)
    //is equal to             10000 (16)
    //so a 13 bit size would be aligned to 16 bits
    return (size + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);
}


//function to reuse memory if available, if not allocate new space
block_header *get_free_block(size_t size, block_header **previous){
    //set the current pointer to the base pointer
    block_header *temp = base_pointer;
    
    //check if the current pointer points to a memory block that is free and has enough space
    while (temp && !(size <= temp->size && temp->free)){
        
        //if not loop through and try the next option in the linked list
        *previous = temp;
        temp = temp->next;
    }
    
    //return the free block
    return temp;
}


block_header *get_new_block(size_t size, block_header *previous){
    //create a temp pointer of the type block header
    block_header *mem_block;
    
    //set the block header to point towards the beginning of the new block
    mem_block = sbrk(0);
    
    //create the new block of memory with included memory for the header
    size_t newsize = align(size);
    if(sbrk(newsize + HEADER_SIZE) == (void*)-1){
        //make sure to return NULL if the block of memory could not be created
        return NULL;
    }
    
    //check if the previous header exists
    if(previous){
        //if the previous header exists set it to point to the new one
        previous->next = mem_block;
    }
    
    //set the new header to include size, the next header(which wont exist yet) and if it is free or not
    mem_block->size = size;
    mem_block->next = NULL;
    mem_block->free = false;
    
    //return the memory block
    return mem_block;
}


void * _malloc(size_t size){
    //if size is less than 0 do not create a pointer
    if(size <= 0)
        return NULL;
    
    block_header *mem_block;
    
    //check if the linked list has been initialized yet
    if(!base_pointer){
        //if not initialize it 
        mem_block = get_new_block(size,NULL);
        if(!mem_block){
            return NULL;
        }
        //and set the base pointer to point to the start of the linked list
        base_pointer = mem_block;
    }
    
    //if it has already been initialized continue here
    else {
        //set the previous block to be the base 
        block_header *previous = base_pointer;
        
        //check if there is a free block with the amount of memory needed
        mem_block = get_free_block(size, &previous);
        
        //if there is no free block get a free block
        if(!mem_block){
            mem_block = get_new_block(size, previous);
            //if getting a new block somehow fails
            if(!mem_block){
                return NULL;
            }
        }
        
        //if a free block was found
        else{
            mem_block->free = false;
        }
    }

    //return the pointer
    return (mem_block+1);
}

void _free(void *ptr){
    //if the pointer doesn't exist exit out
    if(!ptr){
        return;
    }
    
    //mark the memory to be freed as free to use
    block_header *temp = (block_header*)ptr-1;
    temp->free = true;
    
    //if the memory that is getting freed is the last one in the heap return it to the operating system
    if(temp->next == NULL){
        sbrk(-temp->size - HEADER_SIZE);
        temp = sbrk(0);
    }
    
    //if the linked list hasn't been initialized yet just return
    if(base_pointer == NULL){
        return;
    }
    
    //otherwise try and clean up memory bits
    else{
        //start from the beginning of the linked list
        temp = base_pointer;
        
        //if there is only one block of memory just return
        if(temp->next == NULL){
            return;
        }
        
        //otherwise merge the memory block with the block one up
        else if(temp->next->free){
            //combine the size of the two memory blocks
            temp->size = temp->size + temp->next->size;
            //redirect the pointer to the end of the second memory block
            temp->next = temp->next->next;
        }
    }
    
    return;
}
