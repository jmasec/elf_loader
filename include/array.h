#ifndef ARRAY
#define ARRAY

// credit to Tsoding for cool dynamic array implementation
// do while needed so it acts as a statement for th macro
// https://www.youtube.com/watch?v=95M6V3mZgrI
#define da_append(xs, x)\
    do{\
        if (xs.count >= xs.capacity) {\
            if(xs.capacity == 0) xs.capacity = 256;\
            else xs.capacity *= 2;\
            xs.items = realloc(xs.items, xs.capacity*sizeof(*xs.items));\
        }\
        xs.items[xs.count++] = x;\
    } while(0)\

#define da_delete(xs)\
    do{\
        for(int i = 0, i < xs.count; i++){\
            free(xs.items[i]);\
        }\
    }while(0)\

#endif