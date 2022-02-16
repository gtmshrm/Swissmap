#include <stdio.h>
#include "hash.h"

#include "hashmap.h"

#include <time.h>

char *randstring(int length) {    
    static int mySeed = 25011984;
    char *string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
    size_t stringLen = strlen(string);        
    char *randomString = NULL;

    srand(time(NULL) * length + ++mySeed);

    if (length < 1) {
        length = 1;
    }

    randomString = malloc(sizeof(char) * (length +1));

    if (randomString) {
        short key = 0;

        for (int n = 0;n < length;n++) {            
            key = rand() % stringLen;          
            randomString[n] = string[key];
        }

        randomString[length] = '\0';

        return randomString;        
    }
    else {
        printf("No memory");
        exit(1);
    }
}

char *str_alloc(char *val)
{
    char *str = malloc(strlen(val)+1);
    strcpy(str, val);
    return str;
}

void str_print(char *val)
{
    printf("String is -> %s\n", val);
}

bool str_equal(char *val1, char *val2)
{
    return !strcmp(val1, val2);
}

typedef struct kv {
    char *key;
    char *value;
} kv_t;

int main()
{

    const size_t count = 500000;

    char *keys[count], *tmp;
    char key_len = 11;

    char *values[count];
    char value_len = 3;

    for (uint64_t i = 0; i < count; ++i) {
        keys[i] = randstring(key_len);
    }
    for (uint64_t i = 0; i < count; ++i) {
        values[i] = randstring(value_len);
    }
    double time_spent;
    size_t ns = 1000000000, cap;

    size_t i;
    clock_t begin, end;

    cap = 0;

    hm_map_t *map = hm_new(cap, &hash_djb2, &str_equal);

    begin = clock();
    for (i = 0; i < count; ++i) {
        hm_insert(&map, keys[i], values[i]);
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("set\t\t%d ops -> %lf seconds, %.0lf ns/op %.0lf op/sec\n", count, time_spent, time_spent*ns/count, count/time_spent);

    size_t idx;
    begin = clock();
    for (i = 0; i < count; ++i)
        if (!hm_find(map, keys[i], &idx))
            printf("-----\n");

    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("get\t\t%d ops -> %lf seconds, %.0lf ns/op %.0lf op/sec\n", count, time_spent, time_spent*ns/count, count/time_spent);

    hm_key_t *match_key;
    hm_value_t *match_value;
    size_t n_removed = 0;
    begin = clock();
    for (i = 0; i < count; ++i) {
        match_value = hm_remove(map, keys[i], &match_key);
        if (match_value) {
            free(match_key);
            free(match_value);
            n_removed++;
        }
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("delete\t\t%d ops -> %lf seconds, %.0lf ns/op %.0lf op/sec\n", count, time_spent, time_spent*ns/count, count/time_spent);

    printf("--After removing %llu values, map->items = %llu\n", n_removed, map->items);

    hm_key_t *key_it;
    hm_value_t *value_it;

    size_t idx_it = 0;
    size_t c = 0;
    while (hm_iterate(map, &idx_it, &key_it, &value_it)) {
        if (key_it) {
            printf("key = %s removed at idx %llu\n", key_it, idx_it-1);
            free(key_it);
            free(value_it);
            c++;
        }
    }
    printf("c = %llu\n", c);
    printf("items = %llu\n", map->items);
    printf("size = %llu\n", map->size);
    printf("sentinel = %llu\n", map->sentinel);

    hm_clear(map);
    hm_erase(map);
    free(map);
}
