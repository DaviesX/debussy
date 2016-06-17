#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <filesystem.h>

/*
 * helpers.
 */
static const char* __to_absolute_path(const char* cwd, const char* rel_path)
{
        if (*rel_path == '/') {
                // It's an absolute path.
                return strcpy(malloc(strlen(rel_path) + 1), rel_path);
        } else {
                // It's a relative path;
                uint16_t cwd_len = strlen(cwd),
                         rel_len = strlen(rel_path);
                char* new_path = strcpy(malloc(cwd_len + rel_len + 2), cwd);
                // Append a "/" so we can copy the relative path directly to the new path.
                if (cwd[cwd_len - 1] != '/') {
                        new_path[cwd_len] = '/';
                        new_path[cwd_len + 1] = '\0';
                        cwd_len ++;
                }
                // Process the relative path into the new path.
                uint16_t i, j = cwd_len;
                for (i = 0; i < rel_len; ) {
                        if (!strncmp("..", &rel_path[i], 2)) {
                                i += 3;
                                // Backtrack the path.
                                if (new_path[j - 1] == '/') j --;
                                while (j > 0 && new_path[-- j] != '/') continue;
                                new_path[++ j] = '\0';
                        } else if (!strncmp(".", &rel_path[i], 1)) {
                                i += 2;
                        } else {
                                new_path[j ++] = rel_path[i ++];
                        }
                }
                // Deal with trailing.
                if (j > 1 && new_path[j - 1] == '/')
                        new_path[j - 1] = '\0';
                else
                        new_path[j] = '\0';

                return new_path;
        }
}

/*
 * <directory> public
 */
void dir_init(struct directory* self, const char* path, uint8_t type)
{
        self->type = type;
        self->path = (char*) path;
}

void dir_free(struct directory* self)
{
}

/*
 * <file> public
 */
void file_init(struct file* self, const char* path,
               f_File_Free f_free,
               f_File_Read f_read,
               f_File_Write f_write,
               f_File_Seek f_seek)
{
        dir_init(&self->__parent, path, DirectoryFile);
        self->f_free = f_free;
        self->f_read = f_read;
        self->f_write = f_write;
        self->f_seek = f_seek;
}

void file_free(struct file* self)
{
        self->f_free(self);
}

size_t file_read(struct file* self, size_t num_bytes, const void* buf)
{
        return self->f_read(self, num_bytes, buf);
}

size_t file_write(struct file* self, size_t num_bytes, void* buf)
{
        return self->f_write(self, num_bytes, buf);
}

size_t file_seek(struct file* self, size_t offset)
{
        return self->f_seek(self, offset);
}

/*
 * <filesystem> public
 */
void filesys_init(struct filesystem* self, const uint8_t type,
                  f_Filesys_Free f_free,
                  f_Filesys_Open_Directory f_open_directory,
                  f_Filesys_Close_Directory f_close_directory,
                  f_Filesys_Remove_Directory f_remove_directory,
                  f_Filesys_List_Directory f_list_directory,
                  f_Filesys_Open_File f_open_file,
                  f_Filesys_Close_File f_close_file,
                  f_Filesys_Remove_File f_remove_file)
{
        memset(self, 0, sizeof(*self));
        self->type = type;
        self->cwd = strcpy(malloc(2), "/");
        self->f_free = f_free;
        self->f_open_directory = f_open_directory;
        self->f_close_directory = f_close_directory;
        self->f_remove_directory = f_remove_directory;
        self->f_list_directory = f_list_directory;
        self->f_open_file = f_open_file;
        self->f_close_file = f_close_file;
        self->f_remove_file = f_remove_file;
}

void filesys_free(struct filesystem* self)
{
        self->f_free(self);
        free(self->cwd);
        memset(self, 0, sizeof(*self));
}

const char* filesys_connect_directory(struct filesystem* self, const char* path)
{
        const char* new_path = __to_absolute_path(self->cwd, path);
        struct directory* dir;
        if (nullptr != (dir = filesys_open_directory(self, new_path, false))) {
                // New directory exists.
                free(self->cwd);
                self->cwd = (char*) new_path;
                filesys_close_directory(self, dir);
                return self->cwd;
        } else {
                // New directory is invalid.
                return nullptr;
        }
}

const char* filesys_working_directory(struct filesystem* self)
{
        return self->cwd;
}

bool filesys_clone_file(struct filesystem* self, struct file* src, struct file* dst)
{
#ifdef ARCH_X86_64
        uint8_t buf[BUFSIZ];
#else
        uint8_t buf[64];
#endif // ARCH_X86_64
        uint16_t bytes_read;
        while (0 < (bytes_read = file_read(src, sizeof(buf), buf))) {
                if (bytes_read != file_write(dst, bytes_read, buf)) {
                        return false;
                }
        }
        return true;

}


struct directory* filesys_open_directory(struct filesystem* self, const char* path, bool is_2create)
{
        return self->f_open_directory(self, path, is_2create);
}

void filesys_close_directory(struct filesystem* self, struct directory* dir)
{
        self->f_close_directory(self, dir);
}

bool filesys_remove_directory(struct filesystem* self, struct directory* dir)
{
        return self->f_remove_directory(self, dir);
}

void filesys_list_directory(struct filesystem* self, const char* path, uint8_t depth, uint8_t type_filter,
                            f_FileSys_Visit f_visit, void* user_data)
{
        self->f_list_directory(self, path, depth, type_filter, f_visit, user_data);
}


struct file* filesys_open_file(struct filesystem* self, const char* file_path, bool is_2create)
{
        return self->f_open_file(self, file_path, is_2create);
}

void filesys_close_file(struct filesystem* self, struct file* file)
{
        self->f_close_file(self, file);
}

bool filesys_remove_file(struct filesystem* self, struct file* file)
{
        return self->f_remove_file(self, file);
}

/*
 * <filesystem> test cases.
 */
#include <assert.h>

void filesys_test_connect_directory()
{
        struct filesystem fs;
        filesys_init(&fs, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        filesys_connect_directory(&fs, "a/b/c/d");
        printf("dir: %s\n", filesys_working_directory(&fs));
        assert(!strcmp("/a/b/c/d", filesys_working_directory(&fs)));

        filesys_connect_directory(&fs, ".");
        printf("dir: %s\n", filesys_working_directory(&fs));
        assert(!strcmp("/a/b/c/d", filesys_working_directory(&fs)));

        filesys_connect_directory(&fs, "..");
        printf("dir: %s\n", filesys_working_directory(&fs));
        assert(!strcmp("/a/b/c", filesys_working_directory(&fs)));

        filesys_connect_directory(&fs, "./");
        printf("dir: %s\n", filesys_working_directory(&fs));
        assert(!strcmp("/a/b/c", filesys_working_directory(&fs)));

        filesys_connect_directory(&fs, "../");
        printf("dir: %s\n", filesys_working_directory(&fs));
        assert(!strcmp("/a/b", filesys_working_directory(&fs)));

        filesys_connect_directory(&fs, "../e/g/../f");
        printf("dir: %s\n", filesys_working_directory(&fs));
        assert(!strcmp("/a/e/f", filesys_working_directory(&fs)));

        filesys_connect_directory(&fs, "k/");
        printf("dir: %s\n", filesys_working_directory(&fs));
        assert(!strcmp("/a/e/f/k", filesys_working_directory(&fs)));

        filesys_connect_directory(&fs, "../../../../../");
        printf("dir: %s\n", filesys_working_directory(&fs));
        assert(!strcmp("/", filesys_working_directory(&fs)));

        filesys_connect_directory(&fs, "/a/e/f/k");
        filesys_connect_directory(&fs, "../");
        printf("dir: %s\n", filesys_working_directory(&fs));
        assert(!strcmp("/a/e/f", filesys_working_directory(&fs)));

        filesys_connect_directory(&fs, "/1/2/3/4/5/6");
        printf("dir: %s\n", filesys_working_directory(&fs));
        assert(!strcmp("/1/2/3/4/5/6", filesys_working_directory(&fs)));

        filesys_free(&fs);
}
