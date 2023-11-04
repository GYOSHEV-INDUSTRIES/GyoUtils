#pragma once

/*
In this file: Basic profile functionality for nested/recursive functions
*/

/*
USAGE:
- add 'END_OF_COMPILATION_UNIT' as the last line of the file with your app entry point
- call begin_profile() when you want to start profiling
- call end_and_print_profile() when you want to get profiling information
- add TIME_FUNC or TIME_BLOCK("name") to every block you want to profile

This simple profiling can handle functions both nested and recursive.
It has a limit of 4096 timed blocks.
*/

//TODO(cogno): TEST THIS, expecially in:
//             - single functions / blocks
//             - nested functions / blocks
//             - recursive functions
//             - recursive functions with more than 1 function (function a calls b that calls a that calls b ...)
//             - recursive function followed by the same function but NOT recursive

#ifndef PROFILING_V1
    #define PROFILING_V1 1
#endif

#if PROFILING_V1

#ifndef GYOFIRST
    #include "first.h"
#endif
#ifndef GYOPERFORMANCE_COUNTER
    #include "performance_counter.h"
#endif

//API(cogno): we can very easily save the index of the parent to have better ui, but it costs more, is it worth it?
struct TimeAnchor {
    u64 elapsed_exclusive;
    u64 elapsed_inclusive;
    u64 hit_count;
    const char* label;
};

#define ANCHORS_AMT 4096
TimeAnchor _anchors[ANCHORS_AMT] = {};
int _current_parent_index = 0;
u64 _profile_start = 0;
u64 _profile_end = 0;


struct time_block {
    time_block(int counter, const char* func_name) {
        this->_parent = _current_parent_index;
        this->_current = counter;
        this->_label = func_name;
        this->_old_elapsed_at_root = _anchors[counter].elapsed_inclusive;
        
        _current_parent_index = counter;
        this->_start = read_cpu_timer();
    }
    
    ~time_block() {
        u64 total = read_cpu_timer() - this->_start;
        _current_parent_index = this->_parent;
        
        _anchors[this->_parent ].elapsed_exclusive -= total;
        _anchors[this->_current].elapsed_exclusive += total;
        _anchors[this->_current].elapsed_inclusive = this->_old_elapsed_at_root + total;
        _anchors[this->_current].hit_count++;
        
        _anchors[this->_current].label = this->_label; // yes I know, it gets replaced multiple times, I hate C++
    }
    
    const char* _label;
    u64 _start = 0;
    int _current = 0; //save them so we can keep them (inner blocks edit global ones)
    int _parent = 0;  //save them so we can keep them (inner blocks edit global ones)
    int _old_elapsed_at_root = 0;
};

#define TIME_FUNC time_block STRING_JOIN(t_, __LINE__)(__COUNTER__ + 1, __FUNCTION__)
#define TIME_BLOCK(name) time_block STRING_JOIN(t_, __LINE__)(__COUNTER__ + 1, name)
#define END_OF_COMPILATION_UNIT static_assert(__COUNTER__ < ANCHORS_AMT, "Number of profile points exceeds size of profiler::Anchors array")

void begin_profile() {
    _profile_start = read_cpu_timer();
}

void pad_right(int to_add) {
    for(int i = 0; i < to_add; i++) putchar(' ');
}

#ifndef GYOMATH
inline u8 count_digits(u64 x){
    u8 n = 0;
    do {
        n++;
    } while(x /= 10);
    return n;
}
#endif

void end_and_print_profile() {
    // Todo(Quattro) use print instead of printf
    _profile_end = read_cpu_timer();
    u64 total_elapsed = _profile_end - _profile_start;
    
    //calculate lengths of texts for vertical formatting
    int max_label_len = 0;
    u8 max_time_length = 0;
    u8 max_incl_time_length = 0;
    bool one_has_childrens = false;
    for(int i = 0; i < ANCHORS_AMT; i++) {
        TimeAnchor t = _anchors[i];
        if(t.label == nullptr) continue;
        
        int label_len = 0;
        while(t.label[label_len++]) {}
        if(label_len > max_label_len) max_label_len = label_len;

        u8 digits_count = count_digits(t.elapsed_exclusive);
        if (digits_count > max_time_length) max_time_length = digits_count;
        
        u8 incl_digits_count = count_digits(t.elapsed_inclusive);
        if (incl_digits_count > max_incl_time_length) max_incl_time_length = incl_digits_count;
        
        if(t.elapsed_inclusive != t.elapsed_exclusive) one_has_childrens = true;
    }
    
    // PERF(cogno): can be optimized for printing speed, no need for now since it's a report tool which prints once at the end of the whole program
    for(int i = 0; i < ANCHORS_AMT; i++) {
        TimeAnchor t = _anchors[i];
        if (t.label == nullptr) continue;
        
        float percentage_exclusive     = 100.0f * t.elapsed_exclusive / total_elapsed;
        float percentage_with_children = 100.0f * t.elapsed_inclusive / total_elapsed;
        
        printf("%d=> ", i);
        if(t.elapsed_inclusive != t.elapsed_exclusive) {
            //there was an inner block, report complete info
            printf("w/ childrens | ");
        } else {
            //no inner block, less info required
            printf("basic ");
            if(one_has_childrens) printf("       ");
            printf("| ");
        }
        
        //align all label names vertically
        printf("%s", t.label);
        int label_len = 0;
        while(t.label[label_len++]) {}
        pad_right(max_label_len - label_len);
        putchar(':');
        putchar(' ');
        
        //total time
        printf("total=%lld ", t.elapsed_exclusive);
        u8 digits_count = count_digits(t.elapsed_exclusive);
        pad_right(max_time_length - digits_count);
        
        //total time percentage
        printf("(%5.2f%%), ", percentage_exclusive);
        
        //children time
        if(t.elapsed_inclusive != t.elapsed_exclusive) {
            printf("incl=%lld ", t.elapsed_inclusive);
            u8 incl_digits_count = count_digits(t.elapsed_inclusive);
            pad_right(max_incl_time_length - incl_digits_count);
            printf("(%5.2f%% w/ childs), ", percentage_with_children);
        } else if(one_has_childrens) {
            pad_right(5 + max_incl_time_length + 21);
        }
        
        //hit count
        if(t.hit_count == 1) printf("hit once");
        else {
            printf("hit %lld time", t.hit_count);
            if(t.hit_count > 1) putchar('s');
        }
        putchar('\n');
    }
    
    //TODO(cogno): print total cycles/seconds from start to end?
}

#else

#define TIME_FUNC
#define TIME_BLOCK(name)
#define END_OF_COMPILATION_UNIT


void begin_profile() {}
void end_and_print_profile() {}


#endif
