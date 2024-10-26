#pragma once

/*
In this file:
functions to simplify working with windows' file system.
- get_only_files_in_dir(...) to know which files only are in a directory
- get_only_folders_in_dir(...) to know which folders only are in a directory
- get_drive_names(...) to know which drives you have in your pc
- win64_get_last_write_time(...) to know when a file was last edited
- win64_write_file(...) to write data to a file
- win64_read_entire_file(...) to read the entire contents of a file
*/

#ifndef GYOFIRST
    #include "first.h"
#endif

// API(cogno): since array allocates, we can make a get_next_file_in_dir to avoid the allocation. It's basically like str_split_left which returns the single split instead of str_split that returns the array that must be allocated.

bool win64_get_only_files_in_dir(str folder_path, Array<str>* filenames) {
    StrBuilder builder = make_str_builder();
    defer { free(builder.ptr); };
    str_builder_append(&builder, folder_path);
    str_builder_append(&builder, "\\*");
    str_builder_append_raw(&builder, (u8)0);
    
    WIN32_FIND_DATA win_file_data = {};
    HANDLE handle = FindFirstFile((char*)builder.ptr, &win_file_data);
    defer { FindClose(handle); };
    if(handle == INVALID_HANDLE_VALUE) return false;
    
    str_builder_remove_last_bytes(&builder, 2);
    StrBuilder copy = str_builder_copy(&builder);
    defer { free(copy.ptr); };
    
    do {
        // reset str builder into the initial folder path
        copy.size = builder.size;
        str_builder_append(&copy, win_file_data.cFileName);
        
        // .sys files are used by the os, we have to skip them
        str to_check = str_builder_get_str(&copy);
        bool is_sys_file = str_ends_with(to_check, ".sys");
        if(is_sys_file) continue;
        
        // get info if it's a file or not
        str_builder_append_raw(&copy, (u8)0);
        auto result = GetFileAttributes((const char*)copy.ptr);
        if((int)result < 0) {
            DWORD err = GetLastError();
            if(err == ERROR_SHARING_VIOLATION) continue; // skip files we can't access
            return false;
        }
        
        if(result & FILE_ATTRIBUTE_ARCHIVE) {
            str file_name = str_copy((const char*)win_file_data.cFileName); // else it doesn't live outside this function
            array_append(filenames, file_name);
        }
        
    } while(FindNextFile(handle, &win_file_data));
    
    auto err = GetLastError();
    if(err != ERROR_NO_MORE_FILES) return false;
    
    return true;
}

bool win64_get_only_folders_in_dir(str folder_path, Array<str>* folders) {
    StrBuilder builder = make_str_builder();
    defer { free(builder.ptr); };
    str_builder_append(&builder, folder_path);
    str_builder_append(&builder, "\\*");
    str_builder_append_raw(&builder, (u8)0);
    
    WIN32_FIND_DATA win_file_data = {};
    HANDLE handle = FindFirstFile((char*)builder.ptr, &win_file_data);
    defer { FindClose(handle); };
    if(handle == INVALID_HANDLE_VALUE) return false;
    
    str_builder_remove_last_bytes(&builder, 2);
    StrBuilder copy = str_builder_copy(&builder);
    defer { free(copy.ptr); };
    
    do {
        // reset str builder into the initial folder path
        copy.size = builder.size;
        str_builder_append(&copy, win_file_data.cFileName);
        
        // .sys files are used by the os, we have to skip them
        str to_check = str_builder_get_str(&copy);
        bool is_sys_file = str_ends_with(to_check, ".sys");
        if(is_sys_file) continue;
        
        // get info if it's a file or not
        str_builder_append_raw(&copy, (u8)0);
        auto result = GetFileAttributes((const char*)copy.ptr);
        if((int)result < 0) {
            DWORD err = GetLastError();
            if(err == ERROR_SHARING_VIOLATION) continue; // skip files we can't access
            return false;
        }
        
        if(result & FILE_ATTRIBUTE_DIRECTORY) {
            // NOTE(cogno): windows is so fucking stupid it adds '.' and '..' as folders, we need to filter them out
            str folder_name = str_copy(win_file_data.cFileName); // else it doesn't live outside this function
            if(str_matches(folder_name, "."))  continue;
            if(str_matches(folder_name, "..")) continue;
            array_append(folders, folder_name);
        }
        
    } while(FindNextFile(handle, &win_file_data));
    
    auto err = GetLastError();
    if(err != ERROR_NO_MORE_FILES) return false;
    
    return true;
}

// NOTE(cogno): each name will be terminated with ':', so you'll see "C:" etc.
bool win64_get_drive_names(Array<str>* drive_names) {
    const int BUFF_SIZE = 255;
    char buf[BUFF_SIZE];
    // get the drive letters as a set of strings
    int sz = GetLogicalDriveStrings(sizeof(buf), buf);
    if(sz == 0) return false;
    
    if(sz > BUFF_SIZE) {
        ASSERT(sz <= BUFF_SIZE, "buffer not big enough (needs at least % bytes)", sz);
        return false;
    }

    // buf now contains a list of all the drive letters. Each drive letter is
    // terminated with '\0' and the last one is terminated by two consecutive '\0' bytes.
    char* start = buf;
    while(true) {
        char ch = *start;
        if(ch == 0) break;
        
        str drive_name = str_copy(start);
        drive_name.size--; //remove the '\'
        array_append(drive_names, drive_name);
        start += drive_name.size + 2;
    }
    
    return drive_names->size > 0;
}



inline FILETIME win64_get_last_write_time(char *filename) {
    ASSERT(filename != NULL, "no input file given");
    FILETIME last_write_time = {};
    
    WIN32_FIND_DATA find_data;
    HANDLE find_handle = FindFirstFileA(filename, &find_data);
    if (find_handle != INVALID_HANDLE_VALUE) {
        last_write_time = find_data.ftLastWriteTime;
        FindClose(find_handle);
    }
    
    return last_write_time;
}

// Writes input data to a file which is created automatically if the file doesn't exist.
// Returns true if the file is created and the data is saved without any problem, false otherwise.
// For extended error information you should look at win64_print_error().
bool win64_write_file(const char* filename, u8* file_data, u32 data_size) {
    auto file_handle = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (file_handle == INVALID_HANDLE_VALUE) return false;
    defer {
        bool ok = CloseHandle(file_handle);
        ASSERT(ok, "couldn't close file");
    };

    DWORD bytes_written;
    WriteFile(file_handle, file_data, data_size, &bytes_written, 0);
    return true;
}


// Reads the file size of the given file name.
// If the functions succeeds returns the file handle which should be closed later and sets the out_file_size pointer.
HANDLE win64_read_file_size(const char* filename, int* out_file_size) {
    HANDLE file_handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(file_handle == INVALID_HANDLE_VALUE) return NULL;

    DWORD file_size = GetFileSize(file_handle, NULL);
    if(file_size == INVALID_FILE_SIZE) {
        bool ok = CloseHandle(file_handle);
        ASSERT(ok, "couldn't close file");
        return NULL;
    }

    *out_file_size = file_size;
    return file_handle;
}

// Reads the given file amount from the given file handle, then closes the handle automatically.
// Returns the output data as a str for convenience (if the str is empty there was a problem reading the file).
str _win64_read_entire_file(HANDLE file_handle, int file_size, void* dest, int dest_size) {
    ASSERT(file_handle != NULL, "invalid input file handle (was NULL)");
    ASSERT(dest        != NULL, "invalid input destination buffer (was NULL)");
    ASSERT_ALWAYS(file_size <= dest_size, "Input buffer is not big enough! (wanted % but got %)", file_size, dest_size);
    defer {
        bool ok = CloseHandle(file_handle);
        ASSERT(ok, "couldn't close file");
    };
    str out = {};
    out.ptr = (u8*)dest;
    bool ok = ReadFile(file_handle, out.ptr, file_size, (LPDWORD)&out.size, 0);
    if(!ok) return {};
    return out;
}

// reads the entire file data (and automatically allocates)
str win64_read_entire_file(const char* filename, Allocator alloc) {
    int file_size = 0;
    HANDLE file_handle = win64_read_file_size(filename, &file_size);
    if(file_handle == NULL) return {}; // handle already closed by function
    return _win64_read_entire_file(file_handle, file_size, mem_alloc(alloc, file_size), file_size);
}

// reads the entire file data (and automatically allocates)
str win64_read_entire_file(const char* filename) { return win64_read_entire_file(filename, default_allocator); }

