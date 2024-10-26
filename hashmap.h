#pragma once
/*
In this file:
- a simple to use hashmap, useful as a replacement to std::unordered_map
*/
#ifndef GYOFIRST
    #include "first.h"
#endif

#ifndef GYO_ALLOCATORS
    #include "allocators.h"
#endif

#ifndef GYO_ARRAY
    #include "array.h"
#endif

template<typename T, typename U>
struct HashPair {
    T key;
    U value;
};

template<typename T, typename U> void printsl_custom(HashPair<T, U> pair) {
    printsl("{k=%, v=%}", pair.key, pair.value);
}

// API(cogno): it would be cool to have 0 as invalid index, but then the stack has 1 extra element which is wack when you iterate. We could say "each index add/remove 1" as if the array is 1-based instead of 0-based, i don't know...
#define MAP_INVALID_INDEX -1

struct Finder {
    int pair_index        = MAP_INVALID_INDEX;
    int next_finder_index = MAP_INVALID_INDEX;
};

// djb2 hashing taken from http://www.cse.yorku.ca/~oz/hash.html
// API(cogno): we need to find a way to have more than one hashing algorithm, maybe let the user choose his own? either that or have a universal hashing algorithm
template<class T>
u64 hash_default(T* str, int size){
    u64 hash = 5381;

    while (size-- > 0) {
        u8 c = (u8)*str++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

template<>
u64 hash_default<str>(str* string, int size) {
    u64 hash = 5381;

    str string_cp = *string;

    while (string_cp.size-- > 0) {
        u8 c = *string_cp.ptr++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

template<typename T, typename U>
struct HashMap {
    // since we want to iterate over elements using our For macro we hold them in a stack and use a second special array for hashes.
    Array<HashPair<T, U>> elements; // API(cogno): if we inline the array into ptr + size + reserved_size we can directly iterate over elements with our For macro which might be cooler
    // TODO(cogno): this ^ is a bad idea because removing elements is TERRIBLE. Instead on focusing on having ALL elements quickly, focus on removing them, then having all of them can take as much as we want, it doesn't matter. Alternatively have the whole list with a flag that tells you if it's valid and when you iterate over them skip useless ones.

    // API(cogno): a fixed size allocator would be good too but where do you hold the Bump allocator data?
    Finder* matrix_ptr; // if you don't have conflicts this fills up (and does NOT resize)
    int matrix_size;
    
    Array<Finder> solver; // if you have conflicts this fills up (and *does* resize)
    // target: fill as much matrix with as little solver as possible
    
    // PERF(cogno): if we want to make this even faster we might SOA intead of AOS. Instead of holding an array of pair/next, we hold an array of pair and an array of next (such that with an index you can get both in different arrays)
    
    // PERF(cogno): another optimization. If we convert the linked lists into arrays then iterating through a linked list is very cache friendly. The problem becomes reallocation which is basically always O(linked_list.size). If we had only 1 linked list then it might be O(1) since we can expand the allocation zone without memcpy, but we have more than 1 list, so after the block for 1 array you will definitely find a block for another one, which needs a memcpy. Now, is it better to keep the linked lists (so O(1) realloc + bad cache iteration) or to have arrays (O(n) realloc + good cache iteration) ? I think the second one because the realloc penalty is not very often at all. If you think about it you have to realloc when an entire chain gets filled, which happens basically if every other chain gets filled first. Maybe we can realloc each array all together instead of 1 at a time?
    
    Allocator alloc; // we control allocations with this (except for the matrix which is fixed)
};

// API(cogno): our For macro uses ptr and size, but if the hashmap is an array of linked lists, how can we iterate across each element?

template<typename T, typename U>
HashMap<T, U> make_hashmap(int size, Allocator alloc) {
    // API(cogno): maybe have a min size?
    HashMap<T, U> map;
    map.alloc = alloc;
    
    // make the fixed array which we use for hashing
    void* matrix_space = mem_alloc(alloc, size * sizeof(Finder));
    map.matrix_ptr = (Finder*)matrix_space;
    map.matrix_size = size;
    for(int i = 0; i < size; i++) map.matrix_ptr[i] = {}; // PERF(cogno): if we can have MAP_INVALID_INDEX == 0 then this becomes useless (but we also need to force the array with zeros...)
    
    // make space to hold the actual key-value pairs
    map.elements = make_array<HashPair<T, U>>(size, alloc);
    
    // and finally make space to solve hashing conflicts
    map.solver = make_array<Finder>(size, alloc);
    // API(cogno): we should rename size for array/hashmap to say element count and for mem_alloc/allocator to say byte count
    return map;
}

template<typename T, typename U>
HashMap<T, U> make_hashmap(int size) { return make_hashmap<T, U>(size, default_allocator); }




// API(cogno): Is it a good idea to have str == str operator? I don't think so.
// I would prefer to have to explicitly tell map_insert to use str_equals
// and then have a template which defaults to that, something like this:
// template<typename U> void map_insert(HashMap<str, U>* map, str key, U value) {
//   map_insert(map, key, value, str_hash, str_equals);
//}
// this way the user can also implement both his own hashing and his own equals for his own types
//               - Cogno 2024/08/28


template<typename T, typename U>
void map_insert(HashMap<T, U>* map, T key, U value) {
    HashPair<T, U> p = {key, value};
    
    u64 hash = hash_default(&key, sizeof(key));
    
    int matrix_index = hash % map->matrix_size;
    ASSERT_BOUNDS(matrix_index, 0, map->matrix_size);
    Finder* current_finder = &map->matrix_ptr[matrix_index];
    if(current_finder->pair_index == MAP_INVALID_INDEX) {
        array_append(&map->elements, p);
        // simple case, no hash conflict, replace finder
        Finder new_finder = {};
        new_finder.pair_index = map->elements.size - 1;
        map->matrix_ptr[matrix_index] = new_finder;
    } else {
        // complex case, either we have the key and we need to replace the value, or we have to use solver to set a new key
        while(true) {
            int possible_location = current_finder->pair_index;
            // API(cogno): what if the struct doesn't have operator equals?
            if(map->elements[possible_location].key == key) {
                // key found, replace value
                map->elements[possible_location].value = value;
                return;
            }
            
            int next_index = current_finder->next_finder_index;
            if(next_index == MAP_INVALID_INDEX) break;
            current_finder = &map->solver[next_index];
        }
        
        // value is new, add to collision queue in the solver
        array_append(&map->elements, p);
        Finder new_finder = {};
        new_finder.pair_index = map->elements.size - 1;
        array_append(&map->solver, new_finder);
        current_finder->next_finder_index = map->solver.size - 1; // keep the linked list intact
    }
}

// API(cogno): I wish we could U map_find(T key); but we can't return null or something like that because if that's the value it was added then we have no way to know if we couldn't find it or if we could find it and it was null...
template<typename T, typename U>
bool map_find(HashMap<T, U>* map, T key, U* out_value) {
    u64 hash = hash_default(&key, sizeof(key));
    int index = hash % map->matrix_size;
    ASSERT_BOUNDS(index, 0, map->matrix_size);
    Finder* current_finder = &map->matrix_ptr[index];
    
    while (true) {
        int possible_location = current_finder->pair_index;
        if(possible_location == MAP_INVALID_INDEX) break; // no elements ever set
        
        // location valid, check if the key is the same
        // API(cogno): what if the struct doesn't have operator equals?
        auto pair = map->elements[possible_location];
        if(pair.key == key) {
            *out_value = pair.value;
            return true;
        }
        
        int next_index = current_finder->next_finder_index;
        if(next_index == MAP_INVALID_INDEX) break;
        current_finder = &map->solver[next_index];
    }
    
    return false;
}

template<typename T, typename U>
void map_free(HashMap<T, U>* map) {
    // TODO(cogno): free memory here
}

template<typename T, typename U>
void map_remove(HashMap<T, U>* map, T key) {
    // TODO(cogno): this
    // with the current way the hashmap is constructed this is TERRIBLE, you need to update basically EVERY index both in map.matrix and map.solver to adjust the stack position, mega mega balls
    // API(cogno): if we loosen up the constraint of iterating over elements then we can have for each Pair a value that says "this spot is free". This way we don't have to move Pairs around (and so we don't have to recalculate indeces). The problem is 1) we don't have an immediate way to iterate over elements (you have to iterate and ignore free spots) and 2) it decreases cache locality so it gets slower. But since we already loose the easy iteration process we could split key/values into 2 arrays, so key + "is it free" bool in one array and values in the other.
}

// API(cogno): can we change the For macro so we can iterate over elements of a collection *while skipping unwanted elements automatically* ?
