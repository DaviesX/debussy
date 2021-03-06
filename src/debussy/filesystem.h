#ifndef FILESYSTEM_H_INCLUDED
#define FILESYSTEM_H_INCLUDED

#include <types.h>


/*
 * <fs_entity> decl
 */
enum FsEntityType {
        FsEntityDirectory = 1 << 1,
        FsEntityFile = 1 << 2,
        FsEntitySystem = 1 << 3
};

struct fs_entity {
        uint8_t type;
        char*   path;
};

/*
 * <fs_entity> decl
 */
void            fs_entity_init(struct fs_entity* self, const char* path, uint8_t type);
void            fs_entity_free(struct fs_entity* self);
const char*     fs_entity_get_path(struct fs_entity* self);

/*
 * <directory> decl
 */
struct directory;
typedef void    (*f_Dir_Free) (struct directory* self);
typedef bool    (*f_Dir_First) (struct directory* self, struct fs_entity* ent);
typedef bool    (*f_Dir_Next) (struct directory* self, struct fs_entity* ent);

struct directory {
        struct fs_entity        __parent;
        f_Dir_Free              f_free;
        f_Dir_First             f_first;
        f_Dir_Next              f_next;
};

/*
 * <directory> public
 */
void            dir_init(struct directory* self, const char* path,
                         f_Dir_Free f_free,
                         f_Dir_First f_first,
                         f_Dir_Next f_next);
void            dir_free(struct directory* self);
const char*     dir_get_path(struct directory* self);
bool            dir_first(struct directory* self, struct fs_entity* ent);
bool            dir_next(struct directory* self, struct fs_entity* ent);


/*
 * <file> decl
 */
struct file;

typedef void            (*f_File_Free) (struct file* self);
typedef size_t          (*f_File_Read) (struct file* self, size_t num_bytes, void* buf);
typedef size_t          (*f_File_Write) (struct file* self, size_t num_bytes, const void* buf);
typedef size_t          (*f_File_Seek) (struct file* self, size_t offset);
struct file {
        struct fs_entity        __parent;
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
void            file_init(struct file* self, const char* path,
                          f_File_Free f_free,
                          f_File_Read f_read,
                          f_File_Write f_write,
                          f_File_Seek f_seek);
void            file_free(struct file* self);
const char*     file_get_path(struct file* self);
size_t          file_read(struct file* self, size_t num_bytes, void* buf);
size_t          file_write(struct file* self, size_t num_bytes, const void* buf);
size_t          file_seek(struct file* self, size_t offset);


/*
 * <filesystem> decl
 */
enum FilesystemType {
        FileSysEEPROM,
        FileSysPOSIX,
};

struct filesystem;

typedef bool                    (*f_FileSys_Visit) (const char* curr_path, uint8_t depth, void* user_data);

typedef void                    (*f_Filesys_Free) (struct filesystem* self);
typedef struct directory*       (*f_Filesys_Open_Directory) (struct filesystem* self, const char* path, bool is_2create);
typedef bool                    (*f_Filesys_Is_Directory) (struct filesystem* self, const char* path);
typedef void                    (*f_Filesys_Close_Directory) (struct filesystem* self, struct directory* dir);
typedef bool                    (*f_Filesys_Remove_Directory) (struct filesystem* self, struct directory* dir);

typedef struct file*            (*f_Filesys_Open_File) (struct filesystem* self, const char* file_path, bool is_2create);
typedef void                    (*f_Filesys_Close_File) (struct filesystem* self, struct file* file);
typedef bool                    (*f_Filesys_Remove_File) (struct filesystem* self, struct file* file);

typedef const char*             (*f_Filesys_2string) (const struct filesystem* self);

struct filesystem {
        uint8_t                         type;
        char*                           cwd;
        f_Filesys_Free                  f_free;
        f_Filesys_Open_Directory        f_open_directory;
        f_Filesys_Is_Directory          f_is_directory;
        f_Filesys_Close_Directory       f_close_directory;
        f_Filesys_Remove_Directory      f_remove_directory;

        f_Filesys_Open_File             f_open_file;
        f_Filesys_Close_File            f_close_file;
        f_Filesys_Remove_File           f_remove_file;

        f_Filesys_2string               f_2string;
};

/*
 * <filesystem> public
 */
void                    filesys_init(struct filesystem* self, const uint8_t type,
                                     f_Filesys_Free f_free,
                                     f_Filesys_Open_Directory f_open_directory,
                                     f_Filesys_Close_Directory f_close_directory,
                                     f_Filesys_Remove_Directory f_remove_directory,
                                     f_Filesys_Is_Directory f_is_directory,
                                     f_Filesys_Open_File f_open_file,
                                     f_Filesys_Close_File f_close_file,
                                     f_Filesys_Remove_File f_remove_file,
                                     f_Filesys_2string f_2string);
void                    filesys_free(struct filesystem* self);
const char*             filesys_connect_directory(struct filesystem* self, const char* path);
const char*             filesys_working_directory(struct filesystem* self);
bool                    filesys_clone_file(const struct filesystem* self, struct file* src, struct file* dst);
void                    filesys_scan(struct filesystem* self, const char* path, f_FileSys_Visit f_visit, void* user_data);

struct directory*       filesys_open_directory(struct filesystem* self, const char* path, bool is_2create);
bool                    filesys_is_directory(struct filesystem* self, const char* path);
void                    filesys_close_directory(struct filesystem* self, struct directory* dir);
bool                    filesys_remove_directory(struct filesystem* self, struct directory* dir);

struct file*            filesys_open_file(struct filesystem* self, const char* file_path, bool is_2create);
void                    filesys_close_file(struct filesystem* self, struct file* file);
bool                    filesys_remove_file(struct filesystem* self, struct file* file);

const char*             filesys_2string(const struct filesystem* self);


/*
 * <filesystem> test cases.
 */
void filesys_test_connect_directory();

#ifndef ARCH_X86_64

#else
#  include <container/set.h>

/*
 * <fs_posix> decl
 */
struct dir_posix;
struct file_posix;

struct fs_posix {
        struct filesystem       __parent;
        char*                   base;
        int                     last_err;
        set_templ(struct dir_posix*, void*) open_dirs;
        set_templ(struct file_posix*, void*) open_files;

};

/*
 * <fs_posix> public
 */
struct fs_posix*        fs_posix_create(const char* base);
void                    fs_posix_init(struct fs_posix* self, const char* base);
void                    fs_posix_free(struct fs_posix* self);

struct dir_posix*       fs_posix_open_directory(struct fs_posix* self, const char* path, bool is_2create);
void                    fs_posix_close_directory(struct fs_posix* self, struct dir_posix* dir);
bool                    fs_posix_remove_directory(struct fs_posix* self, struct dir_posix* dir);
bool                    fs_posix_is_directory(struct fs_posix* self, const char* path);

struct file_posix*      fs_posix_open_file(struct fs_posix* self, const char* file_path, bool is_2create);
void                    fs_posix_close_file(struct fs_posix* self, struct file_posix* file);
bool                    fs_posix_remove_file(struct fs_posix* self, struct file_posix* file);

const char*             fs_posix_2string(const struct fs_posix* self);

/*
 * <dir_posix> decl
 */
typedef struct __dirstream DIR;
struct dirent;
struct dir_posix {
        struct directory        __parent;
        char*                   actual_path;
        DIR*                    ds;
        struct dirent*          entry;
};

/*
 * <dir_posix> public
 */
struct dir_posix*       dir_posix_create(const char* base, const char* cwd, const char* path, bool is_2create);
bool                    dir_posix_init(struct dir_posix* self,
                                       const char* base,
                                       const char* cwd,
                                       const char* path,
                                       bool is_2create);
void                    dir_posix_free(struct dir_posix* self);
bool                    dir_posix_first(struct dir_posix* self, struct fs_entity* ent);
bool                    dir_posix_next(struct dir_posix* self, struct fs_entity* ent);


/*
 * <file_posix> decl
 */
struct file_posix {
        struct file             __parent;
        char*                   actual_path;
        int                     fd;
};

/*
 * <file_posix> public
 */
struct file_posix*      file_posix_create(const char* base, const char* cwd, const char* path, bool is_2create);
bool                    file_posix_init(struct file_posix* self,
                                        const char* base,
                                        const char* cwd,
                                        const char* path,
                                        bool is_2create);
void                    file_posix_free(struct file_posix* self);
size_t                  file_posix_read(struct file_posix* self, size_t num_bytes, void* buf);
size_t                  file_posix_write(struct file_posix* self, size_t num_bytes, const void* buf);
size_t                  file_posix_seek(struct file_posix* self, size_t offset);

#endif // ARCH_X86_64


#endif // FILESYSTEM_H_INCLUDED
