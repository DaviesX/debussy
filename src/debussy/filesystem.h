#ifndef FILESYSTEM_H_INCLUDED
#define FILESYSTEM_H_INCLUDED

#include <types.h>

/*
 * <directory> decl
 */
enum DirectoryType {
        DirectoryPure = 1 << 1,
        DirectoryFile = 1 << 2,
        DirectorySys = 1 << 3
};

struct directory {
        uint8_t type;
        char*   path;
};

/*
 * <directory> public
 */
void dir_init(struct directory* self, const char* path, uint8_t type);
void dir_free(struct directory* self);


/*
 * <file> decl
 */
struct file;

typedef void            (*f_File_Free) (struct file* self);
typedef size_t          (*f_File_Read) (struct file* self, size_t num_bytes, const void* buf);
typedef size_t          (*f_File_Write) (struct file* self, size_t num_bytes, void* buf);
typedef size_t          (*f_File_Seek) (struct file* self, size_t offset);
struct file {
        struct directory        __parent;
        f_File_Free             f_free;
        f_File_Read             f_read;
        f_File_Write            f_write;
        f_File_Seek             f_seek;
        size_t                  size;
        char*                   name;
};

/*
 * <file> public
 */
void    file_init(struct file* self, const char* path,
                  f_File_Free f_free,
                  f_File_Read f_read,
                  f_File_Write f_write,
                  f_File_Seek f_seek);
void    file_free(struct file* self);
size_t  file_read(struct file* self, size_t num_bytes, const void* buf);
size_t  file_write(struct file* self, size_t num_bytes, void* buf);
size_t  file_seek(struct file* self, size_t offset);


/*
 * <filesystem> decl
 */
enum FilesystemType {
        FileSysEEPROM,
        FileSysPOSIX,
};

struct filesystem;

typedef bool                    (*f_FileSys_Visit) (const char* curr_path, void* user_data);

typedef void                    (*f_Filesys_Free) (struct filesystem* self);
typedef struct directory*       (*f_Filesys_Open_Directory) (struct filesystem* self, const char* path, bool is_2create);
typedef void                    (*f_Filesys_Close_Directory) (struct filesystem* self, struct directory* dir);
typedef bool                    (*f_Filesys_Remove_Directory) (struct filesystem* self, struct directory* dir);
typedef void                    (*f_Filesys_List_Directory) (struct filesystem* self,
                                                             const char* path, uint8_t depth, uint8_t type_filter,
                                                             f_FileSys_Visit f_visit, void* user_data);

typedef struct file*            (*f_Filesys_Open_File) (struct filesystem* self, const char* file_path, bool is_2create);
typedef void                    (*f_Filesys_Close_File) (struct filesystem* self, struct file* file);
typedef bool                    (*f_Filesys_Remove_File) (struct filesystem* self, struct file* file);

struct filesystem {
        uint8_t                         type;
        char*                           cwd;
        f_Filesys_Free                  f_free;
        f_Filesys_Open_Directory        f_open_directory;
        f_Filesys_Close_Directory       f_close_directory;
        f_Filesys_Remove_Directory      f_remove_directory;
        f_Filesys_List_Directory        f_list_directory;

        f_Filesys_Open_File             f_open_file;
        f_Filesys_Close_File            f_close_file;
        f_Filesys_Remove_File           f_remove_file;
};

/*
 * <filesystem> public
 */
void                    filesys_init(struct filesystem* self, const uint8_t type,
                                     f_Filesys_Free f_free,
                                     f_Filesys_Open_Directory f_open_directory,
                                     f_Filesys_Close_Directory f_close_directory,
                                     f_Filesys_Remove_Directory f_remove_directory,
                                     f_Filesys_List_Directory f_list_directory,
                                     f_Filesys_Open_File f_open_file,
                                     f_Filesys_Close_File f_close_file,
                                     f_Filesys_Remove_File f_remove_file);
void                    filesys_free(struct filesystem* self);
const char*             filesys_connect_directory(struct filesystem* self, const char* path);
const char*             filesys_working_directory(struct filesystem* self);
bool                    filesys_clone_file(struct filesystem* self, struct file* src, struct file* dst);

struct directory*       filesys_open_directory(struct filesystem* self, const char* path, bool is_2create);
void                    filesys_close_directory(struct filesystem* self, struct directory* dir);
bool                    filesys_remove_directory(struct filesystem* self, struct directory* dir);
void                    filesys_list_directory(struct filesystem* self,
                                               const char* path, uint8_t depth, uint8_t type_filter,
                                               f_FileSys_Visit f_visit, void* user_data);

struct file*            filesys_open_file(struct filesystem* self, const char* file_path, bool is_2create);
void                    filesys_close_file(struct filesystem* self, struct file* file);
bool                    filesys_remove_file(struct filesystem* self, struct file* file);

/*
 * <filesystem> test cases.
 */
void filesys_test_connect_directory();


#endif // FILESYSTEM_H_INCLUDED
