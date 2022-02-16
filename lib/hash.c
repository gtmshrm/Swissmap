#include "hash.h"

/*
 * DJB2 Hash Function.
 */
size_t hash_djb2(unsigned char *str)
{
    size_t hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;
    return hash;
}
