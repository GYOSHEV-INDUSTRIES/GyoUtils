#pragma once

/*
In this file:
functions to simplify working with windows' file system.
- get_only_files_in_dir(...) to know which files only are in a directory
- get_only_folders_in_dir(...) to know which folders only are in a directory
- get_drive_names(...) to know which drives you have in your pc
*/

#ifndef DISABLE_INCLUDES
    #include <windows.h>
#endif

#ifndef GYOFIRST
    #include "first.h"
#endif

void win64_print_error() {
    char buf[255]; // hope this is enough
    DWORD dwError = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, dwError, 0, buf, sizeof(buf), 0);
    print(buf);
}

bool get_only_files_in_dir(str folder_path, Array<str>* filenames) {
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
            str file_name = str_new_alloc(win_file_data.cFileName); // else it doesn't live outside this function
            array_append(filenames, file_name);
        }
        
    } while(FindNextFile(handle, &win_file_data));
    
    auto err = GetLastError();
    if(err != ERROR_NO_MORE_FILES) return false;
    
    return true;
}

bool get_only_folders_in_dir(str folder_path, Array<str>* folders) {
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
            str folder_name = str_new_alloc(win_file_data.cFileName); // else it doesn't live outside this function
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
bool get_drive_names(Array<str>* drive_names) {
    const int BUFF_SIZE = 255;
    char buf[BUFF_SIZE];
    // get the drive letters as a set of strings
    int sz = GetLogicalDriveStrings(sizeof(buf), buf);
    if(sz == 0) return false;
    
    ASSERT(sz <= BUFF_SIZE, "buffer not big enough (needs at least % bytes)", sz);

    // buf now contains a list of all the drive letters. Each drive letter is
    // terminated with '\0' and the last one is terminated by two consecutive '\0' bytes.
    char* start = buf;
    while(true) {
        char ch = *start;
        if(ch == 0) break;
        
        str drive_name = str_new_alloc(start);
        drive_name.size--; //remove the '\'
        array_append(drive_names, drive_name);
        start += drive_name.size + 2;
    }
    
    return drive_names->size > 0;
}