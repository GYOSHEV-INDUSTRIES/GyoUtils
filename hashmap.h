#ifndef GYOFIRST
    #include "first.h"
#endif
#ifndef GYODYNAMIC_ARRAY
    #include "dynamic_array.h"
#endif
#ifndef GYOHASH
    #include "hash.cpp"
#endif

#define REFILL_REMOVED true
#define COUNT_COLLISIONS false
#define LOAD_FACTOR_PERCENT 70
#define NEVER_OCCUPIED_HASH 0
#define REMOVED_HASH 1
#define FIRST_VALID_HASH 2

template <typename K, typename V>
struct Entry {
    u32 hash;
    K key;
    V value;
};

template <typename K, typename V>
struct HashMap {  // Feature(Quattro) we could get from the user the hashing function and store it in the struct
    int count = 0;
    int allocated = 0;
    int slots_filled = 0;
    Array<Entry<K, V>> entries;
    #if COUNT_COLLISIONS
        int add_collisions = 0;
        int find_collisions = 0;
    #endif
};

template <typename K, typename V>
HashMap<K, V> hashmap_init(u32 size=32) {
    HashMap<K, V> map;
    hashmap_resize(&map, size);
    return map;
}

template <typename K, typename V>
void hashmap_resize(HashMap<K, V>* map, u32 size=32) {
    array_free_all(&map->entries);
    int n = next_power_of_two(size);
    map->allocated = n;
    map->entries = array_new<Entry<K, V>>(n);
    for(int i = 0; i < n; i++) {
        map->entries.ptr[i].hash = 0;
    }
}

template <typename K, typename V>
void hashmap_deinit(HashMap<K, V>* map) {
    array_free_all(&map->entries);
}

template <typename K, typename V>
void hashmap_reset(HashMap<K, V>* map) {
    map->count = 0;
    map->slots_filled = 0;
    for(int i = 0; i < map->count; i++) {
        map->entries.ptr[i].hash = 0;
    }
}

template <typename K, typename V>
void hashmap_ensure_space(HashMap<K, V>* map, int items) {
    if((map->slots_filled + items) * 100 >= map->allocated * LOAD_FACTOR_PERCENT) {
        hashmap_expand(map);
    }
}

template <typename K, typename V>
V* hashmap_add(HashMap<K, V>* map, K key, V value) {
    if(((map->slots_filled + 1) * 100) >= (map->allocated * LOAD_FACTOR_PERCENT))  hashmap_expand(map);
    ASSERT(map->slots_filled < map->allocated, "Hashmap expand didn't work as expected");
    
    #if REFILL_REMOVED
        bool slot_reused = false;
    #endif
    
    u32 mask = (u32)(map->allocated - 1);
    auto hash = get_hash(key);
    if(hash < FIRST_VALID_HASH) hash += FIRST_VALID_HASH;
    auto index = hash & mask;
    
    u32 probe_increment = 1;
    
    while(map->entries.ptr[index].hash) {
        #if REFILL_REMOVED
            if(map->entries.ptr[index].hash == REMOVED_HASH) {
                slot_reused = true;
                break;
            }
        #endif
        #if COUNT_COLLISIONS
            map->add_collisions += 1;
        #endif
        
        index = (index + probe_increment) & mask;
        probe_increment++;
    }
    
    map->count++;
    #if REFILL_REMOVED
        if(!slot_reused) {
            map->slots_filled++;
        }
    #else
        map->slots_filled++;
    #endif
    
    auto entry = &map->entries.ptr[index];
    entry->hash = hash;
    entry->key = key;
    entry->value = value;
    
    return &entry->value;
}

template <typename K, typename V>
V* hashmap_set(HashMap<K, V>* map, K key, V value) {
    auto value_ptr = hashmap_find_pointer(map, key);
    if(value_ptr) {
        *value_ptr = value;
        return value_ptr;
    } else {
        return hashmap_add(map, key, value);
    }
}

template <typename K, typename V>
bool hashmap_contains(HashMap<K, V>* map, K key) {
    return hashmap_find_pointer(map, key) != nullptr;
}

template <typename K, typename V>
V* hashmap_find_pointer(HashMap<K, V>* map, K key) {
    if(!map->allocated) return nullptr;
    
    u32 mask = (u32)(map->allocated - 1);
    u32 hash = get_hash(key);
    if(hash < FIRST_VALID_HASH) hash += FIRST_VALID_HASH;
    auto index = hash & mask;
    
    u32 probe_increment = 1;
    while(map->entries.ptr[index].hash) {
        auto entry = &map->entries.ptr[index];
        if(entry->hash == hash) {
            if(entry->key == key) return &entry->value;
        }
        #if COUNT_COLLISIONS
            map->find_collisions++;
        #endif
        
        index = (index + probe_increment) & mask;
        probe_increment++;
    }
    
    return nullptr;
}

template <typename K, typename V>
bool hashmap_find(HashMap<K, V>* map, K key, V* ret) {
    auto ptr = hashmap_find_pointer(map, key);
    if(ptr) {
        *ret = *ptr;
        return true;
    }
    return false;
}

template <typename K, typename V>
bool hashmap_remove(HashMap<K, V>* map, K key, V* value) {
    if(!map->allocated) return false;
    
    u32 mask = (u32)(map->allocated - 1);
    auto hash = get_hash(key);
    if(hash < FIRST_VALID_HASH) hash += FIRST_VALID_HASH;
    auto index = hash & mask;
    
    u32 probe_increment = 1;
    while(map->entries.ptr[index].hash) {
        auto entry = &map->entries.ptr[index];
        if(entry->hash == hash && entry->key == key) {
            entry->hash = REMOVED_HASH;
            map->count--;
            *value = entry->value;
            return true;
        }
        
        index = (index + probe_increment) & mask;
        probe_increment++;
    }
    
    return false;
}
template <typename K, typename V>
bool hashmap_remove(HashMap<K, V>* map, K key) {
    if(!map->allocated) return false;
    
    u32 mask = (u32)(map->allocated - 1);
    auto hash = get_hash(key);
    if(hash < FIRST_VALID_HASH) hash += FIRST_VALID_HASH;
    auto index = hash & mask;
    
    u32 probe_increment = 1;
    while(map->entries.ptr[index].hash) {
        auto entry = &map->entries.ptr[index];
        if(entry->hash == hash && entry->key == key) {
            entry->hash = REMOVED_HASH;
            map->count--;
            return true;
        }
        
        index = (index + probe_increment) & mask;
        probe_increment++;
    }
    
    return false;
}

template <typename K, typename V>
bool hashmap_find_or_add(HashMap<K, V>* map, K key, V* value) {
    V* ptr = hashmap_find_pointer(map, key);
    if(ptr) {
        *value = *ptr;
        return false;
    }
    V new_value = *value;
    ptr = hashmap_add(map, key, new_value);
    *value = *ptr;
    return true;
}

template <typename K, typename V>
void hashmap_expand(HashMap<K, V>* map) {
    Array<Entry<K, V>> old_entries = array_new<Entry<K, V>>(map->allocated);
    for(int i = 0; i < map->allocated; i++) {
        array_append(&old_entries, map->entries.ptr[i]);
    }
    
    int new_allocated;
    if((map->count * 2 + 1) * 100 < map->allocated * LOAD_FACTOR_PERCENT) {
        new_allocated = map->allocated;
    } else {
        new_allocated = map->allocated * 2;
    }
    if(new_allocated < 32) {
        new_allocated = 32;
    }
    hashmap_resize(map, new_allocated);
    map->count = 0;
    map->slots_filled = 0;
    
    for(int i = 0; i < old_entries.size; i++) {
        auto entry = old_entries[i];
        if(entry.hash >= FIRST_VALID_HASH) {
            hashmap_add(map, entry.key, entry.value);
        }
    }
    array_free_all(&old_entries);
}

int next_power_of_two(int x) {
    ASSERT(x != 0, "x must not be zero");
    int p = 2;
    while(x > p) {
        p <<= 1;
    }
    print("% %", x, p);
    return p;
}
