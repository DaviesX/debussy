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
 * <fs_entity> decl
 */
void fs_entity_init(struct fs_entity* self, const char* path, uint8_t type)
{
        self->type = type;
        self->path = strcpy(malloc(strlen(path) + 1), path);
}

void fs_entity_free(struct fs_entity* self)
{
        free(self->path);
}

const char* fs_entity_get_path(struct fs_entity* self)
{
        return self->path;
}

/*
 * <directory> public
 */
void dir_init(struct directory* self, const char* path,
              f_Dir_Free f_free,
              f_Dir_First f_first,
              f_Dir_Next f_next)
{
        memset(self, 0, sizeof(*self));
        fs_entity_init(&self->__parent, path, FsEntityDirectory);

        self->f_free = f_free;
        self->f_first = f_first;
        self->f_next = f_next;
}

void dir_free(struct directory* self)
{
        self->f_free(self);

        fs_entity_free(&self->__parent);
        memset(self, 0, sizeof(*self));
}

const char* dir_get_path(struct directory* self)
{
        return fs_entity_get_path(&self->__parent);
}

bool dir_first(struct directory* self, struct fs_entity* ent)
{
        return self->f_first(self, ent);
}

bool dir_next(struct directory* self, struct fs_entity* ent)
{
        return self->f_next(self, ent);
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
        memset(self, 0, sizeof(*self));
        fs_entity_init(&self->__parent, path, FsEntityFile);
        self->f_free = f_free;
        self->f_read = f_read;
        self->f_write = f_write;
        self->f_seek = f_seek;
}

void file_free(struct file* self)
{
        self->f_free(self);

        fs_entity_free(&self->__parent);
        memset(self, 0, sizeof(*self));
}

const char* file_get_path(struct file* self)
{
        return fs_entity_get_path(&self->__parent);
}

size_t file_read(struct file* self, size_t num_bytes, void* buf)
{
        return self->f_read(self, num_bytes, buf);
}

size_t file_write(struct file* self, size_t num_bytes, const void* buf)
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
                  f_Filesys_Is_Directory f_is_directory,
                  f_Filesys_Open_File f_open_file,
                  f_Filesys_Close_File f_close_file,
                  f_Filesys_Remove_File f_remove_file,
                  f_Filesys_2string f_2string)
{
        memset(self, 0, sizeof(*self));
        self->type = type;
        self->cwd = strcpy(malloc(2), "/");
        self->f_free = f_free;
        self->f_open_directory = f_open_directory;
        self->f_close_directory = f_close_directory;
        self->f_remove_directory = f_remove_directory;
        self->f_is_directory = f_is_directory;
        self->f_open_file = f_open_file;
        self->f_close_file = f_close_file;
        self->f_remove_file = f_remove_file;
        self->f_2string = f_2string;
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

bool filesys_clone_file(const struct filesystem* self, struct file* src, struct file* dst)
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

static void __filesys_scan_recur(struct filesystem* self, const char* path, int depth, f_FileSys_Visit f_visit, void* user_data)
{
        struct directory* dir = filesys_open_directory(self, path, false);
        if (!dir)
                return ;
        struct fs_entity ent;
        if (!dir_first(dir, &ent))
                return ;
        do {
                const char* curr_path = fs_entity_get_path(&ent);
                char* end = strrchr(curr_path, '\0');
                if (*(end - 1) != '.') {
                        if (!f_visit(curr_path, depth, user_data)) {
                                fs_entity_free(&ent);
                                break;
                        }
                        if (filesys_is_directory(self, curr_path))
                                __filesys_scan_recur(self, curr_path, depth + 1, f_visit, user_data);
                }
                fs_entity_free(&ent);
        } while (dir_next(dir, &ent));
        filesys_close_directory(self, dir), free(dir);
}

void filesys_scan(struct filesystem* self, const char* path, f_FileSys_Visit f_visit, void* user_data)
{
        __filesys_scan_recur(self, path, 0, f_visit, user_data);
}


struct directory* filesys_open_directory(struct filesystem* self, const char* path, bool is_2create)
{
        return self->f_open_directory(self, path, is_2create);
}

bool filesys_is_directory(struct filesystem* self, const char* path)
{
        return self->f_is_directory(self, path);
}

void filesys_close_directory(struct filesystem* self, struct directory* dir)
{
        self->f_close_directory(self, dir);
}

bool filesys_remove_directory(struct filesystem* self, struct directory* dir)
{
        return self->f_remove_directory(self, dir);
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

const char* filesys_2string(const struct filesystem* self)
{
        return self->f_2string(self);
}

/*
 * <filesystem> test cases.
 */
#include <assert.h>

void filesys_test_connect_directory()
{
#ifndef ARCH_X86_64
        struct filesystem* fs = nullptr;
#else
        struct filesystem* fs = &fs_posix_create(".")->__parent;
#endif // ARCH_X86_64
        struct directory* dir;
        if (!(dir = filesys_open_directory(fs, "/a/b/c/d", true))) {
                printf("failed to create directory\n");
                abort();
        }
        filesys_close_directory(fs, dir);

        filesys_connect_directory(fs, "a/b/c/d");
        printf("dir: %s\n", filesys_working_directory(fs));
        assert(!strcmp("/a/b/c/d", filesys_working_directory(fs)));

        filesys_connect_directory(fs, ".");
        printf("dir: %s\n", filesys_working_directory(fs));
        assert(!strcmp("/a/b/c/d", filesys_working_directory(fs)));

        filesys_connect_directory(fs, "..");
        printf("dir: %s\n", filesys_working_directory(fs));
        assert(!strcmp("/a/b/c", filesys_working_directory(fs)));

        filesys_connect_directory(fs, "./");
        printf("dir: %s\n", filesys_working_directory(fs));
        assert(!strcmp("/a/b/c", filesys_working_directory(fs)));

        filesys_connect_directory(fs, "../");
        printf("dir: %s\n", filesys_working_directory(fs));
        assert(!strcmp("/a/b", filesys_working_directory(fs)));

        filesys_open_directory(fs, "/a/e/f", true);
        filesys_connect_directory(fs, "../e/g/../f");
        printf("dir: %s\n", filesys_working_directory(fs));
        assert(!strcmp("/a/e/f", filesys_working_directory(fs)));

        filesys_open_directory(fs, "/a/e/f/k", true);
        filesys_connect_directory(fs, "k/");
        printf("dir: %s\n", filesys_working_directory(fs));
        assert(!strcmp("/a/e/f/k", filesys_working_directory(fs)));

        filesys_connect_directory(fs, "../../../../../");
        printf("dir: %s\n", filesys_working_directory(fs));
        assert(!strcmp("/", filesys_working_directory(fs)));

        filesys_connect_directory(fs, "/a/e/f/k");
        filesys_connect_directory(fs, "../");
        printf("dir: %s\n", filesys_working_directory(fs));
        assert(!strcmp("/a/e/f", filesys_working_directory(fs)));

        struct directory* dir2 = filesys_open_directory(fs, "/a", false);
        filesys_remove_directory(fs, dir2);

        filesys_open_directory(fs, "/1/2/3/4/5/6/", true);
        filesys_connect_directory(fs, "/1/2/3/4/5/6");
        printf("dir: %s\n", filesys_working_directory(fs));
        assert(!strcmp("/1/2/3/4/5/6", filesys_working_directory(fs)));

        filesys_free(fs), free(fs);
}


#ifndef ARCH_X86_64

#else
#  include <errno.h>
#  include <unistd.h>
#  include <fcntl.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>

/*
 * <fs_posix> public
 */
struct fs_posix* fs_posix_create(const char* base)
{
        struct fs_posix* self = malloc(sizeof(*self));
        fs_posix_init(self, base);
        return self;
}

void fs_posix_init(struct fs_posix* self, const char* base)
{
        memset(self, 0, sizeof(*self));

        self->base = strdup(base);
        set_init(&self->open_files, 1, nullptr);
        set_init(&self->open_dirs, 1, nullptr);

        filesys_init(&self->__parent, FileSysPOSIX,
                     (f_Filesys_Free) fs_posix_free,
                     (f_Filesys_Open_Directory) fs_posix_open_directory,
                     (f_Filesys_Close_Directory) fs_posix_close_directory,
                     (f_Filesys_Remove_Directory) fs_posix_remove_directory,
                     (f_Filesys_Is_Directory) fs_posix_is_directory,
                     (f_Filesys_Open_File) fs_posix_open_file,
                     (f_Filesys_Close_File) fs_posix_close_file,
                     (f_Filesys_Remove_File) fs_posix_remove_file,
                     (f_Filesys_2string) fs_posix_2string);
}

void fs_posix_free(struct fs_posix* self)
{
        struct file_posix* file;
        set_for_each(&self->open_files, file,
                fs_posix_close_file(self, file);
        );
        set_free(&self->open_files);

        struct dir_posix* dir;
        set_for_each(&self->open_dirs, dir,
                fs_posix_close_directory(self, dir);
        );
        set_free(&self->open_dirs);

        free(self->base);
        memset(self, 0, sizeof(*self));
}

static const char* __posix_get_full_path(const char* base, const char* cwd, const char* file_path)
{
        int l = strlen(file_path),
            m = strlen(base),
            n = strlen(cwd);
        char* path;
        path = strcpy(malloc(l + m + n + 2), base);
        if (path[m - 1] == '/') m --;
        if (file_path[0] != '/') {
                // Relative path.
                strcpy(&path[m], cwd);
                path[m + n ++] = '/';
        } else
                // Absolute path.
                n = 0;
        strcpy(&path[m + n], file_path);
        return path;
}

#define fs_entity_is_equal(__x, __y, __d)       (*(__x) == *(__y))
#define fs_entity_hash(__x)                     (int_hash((uint64_t) (__x)))

struct dir_posix* fs_posix_open_directory(struct fs_posix* self, const char* path, bool is_2create)
{
        const char* cwd = filesys_working_directory(&self->__parent);
        struct dir_posix* dir;
        if (nullptr != (dir = dir_posix_create(self->base, cwd, path, is_2create))) {
                set_iter_templ(struct dir_posix*) iter;
                set_add(&self->open_dirs, dir, fs_entity_hash, fs_entity_is_equal, &iter);
        }
        return dir;
}

void fs_posix_close_directory(struct fs_posix* self, struct dir_posix* dir)
{
        set_iter_templ(struct dir_posix*) iter;
        set_find(&self->open_dirs, dir, fs_entity_hash, fs_entity_is_equal, &iter);
        if (set_iter_has_next(&iter, &self->open_dirs)) {
                // Valid directory.
                dir_free(&dir->__parent), free(dir);
                set_remove_at(&self->open_dirs, &iter);
        } else
                // Invalid pointer.
                abort();
}

static bool __fs_posix_rmpath(const char* path, uint8_t depth, int* res)
{
        struct stat sb;
        if (-1 == stat(path, &sb) || -1 == unlink(path))
                *res = -1;
        if (!S_ISDIR(sb.st_mode))
                unlink(path);
        return true;
}

bool fs_posix_remove_directory(struct fs_posix* self, struct dir_posix* dir)
{
        const char* path = strdup(dir_get_path(&dir->__parent));
        fs_posix_close_directory(self, dir);
        int res = 0;
        filesys_scan(&self->__parent, path, (f_FileSys_Visit) __fs_posix_rmpath, &res), free((void*) path);
        return res != -1;
}

bool fs_posix_is_directory(struct fs_posix* self, const char* path)
{
        const char* cwd = filesys_working_directory(&self->__parent);
        const char* actual_path = __posix_get_full_path(self->base, cwd, path);
        struct stat sb;
        bool res = stat(actual_path, &sb) == 0 && S_ISDIR(sb.st_mode);
        free((void*) actual_path);
        return res;
}

struct file_posix* fs_posix_open_file(struct fs_posix* self, const char* file_path, bool is_2create)
{
        const char* cwd = filesys_working_directory(&self->__parent);
        struct file_posix* file;
        if (nullptr != (file = file_posix_create(self->base, cwd, file_path, is_2create))) {
                set_iter_templ(struct file_posix*) iter;
                set_add(&self->open_files, file, fs_entity_hash, fs_entity_is_equal, &iter);
        }
        return file;
}

void fs_posix_close_file(struct fs_posix* self, struct file_posix* file)
{
        set_iter_templ(struct file*) iter;
        set_find(&self->open_files, file, fs_entity_hash, fs_entity_is_equal, &iter);
        if (set_iter_has_next(&iter, &self->open_files)) {
                // Valid file.
                set_remove_at(&self->open_files, &iter);
                file_free(&file->__parent), free(file);
        } else
                // Invalid pointer.
                abort();
}

bool fs_posix_remove_file(struct fs_posix* self, struct file_posix* file)
{
        char path[BUFSIZ];
        strcpy(path, file_get_path(&file->__parent));
        fs_posix_close_file(self, file);
        return -1 != unlink(path);
}

const char* fs_posix_2string(const struct fs_posix* self)
{
        char* buf = malloc(strlen(self->base) + strlen("fs_posix on ") + 1);
        sprintf(buf, "fs_posix on %s", self->base);
        return buf;
}


/*
 * <dir_posix> public
 */
struct dir_posix* dir_posix_create(const char* base, const char* cwd, const char* path, bool is_2create)
{
        struct dir_posix* self = malloc(sizeof(*self));
        if (dir_posix_init(self, base, cwd, path, is_2create))
                return self;
        else {
                free(self);
                return nullptr;
        }
}

static int __dir_posix_mkpath(const char* file_path, mode_t mode)
{
        if (!file_path || !(*file_path))
                return 0;
        char* last = strrchr(file_path, '\0');
        char* p;
        for (p = strchr(file_path + 1, '/'); ; p = (p = strchr(p + 1, '/')) == nullptr ? last : p) {
                *p = '\0';
                if (mkdir(file_path, mode) == -1) {
                        if (errno != EEXIST) {
                                *p = '/';
                                return -1;
                        }
                }
                if (p != last) *p = '/';
                else break;
        }
        return 0;
}

bool dir_posix_init(struct dir_posix* self, const char* base, const char* cwd, const char* path, bool is_2create)
{
        memset(self, 0, sizeof(*self));
        self->actual_path = (void*) __posix_get_full_path(base, cwd, path);
        struct stat sb;
        if (!is_2create && (stat(self->actual_path, &sb) != 0 || !S_ISDIR(sb.st_mode)))
                // Directory doesn't exist or not a directory.
                return false;

        if (is_2create) {
                // Create the directory immediately.
                if (-1 == __dir_posix_mkpath(self->actual_path, 0755))
                        return false;
                self->ds = nullptr;
        } else
                // Lazy open policy.
                self->ds = nullptr;
        self->entry = nullptr;

        dir_init(&self->__parent, path,
                 (f_Dir_Free) dir_posix_free,
                 (f_Dir_First) dir_posix_first,
                 (f_Dir_Next) dir_posix_next);
        return true;
}

void dir_posix_free(struct dir_posix* self)
{
        if (self->ds)
                closedir(self->ds);
        free(self->actual_path);
}

static void __dir_posix_fill_fs_entity(struct dir_posix* self, struct fs_entity* ent)
{
        char buf[BUFSIZ];
        int l = strlen(dir_get_path(&self->__parent));
        strcpy(buf, self->actual_path);
        if (buf[l - 1] != '/')
                buf[l ++] = '/';
        strcpy(&buf[l], self->entry->d_name);

        switch (self->entry->d_type) {
        case DT_DIR:
                fs_entity_init(ent, buf, FsEntityDirectory);
                break;
        case DT_REG:
                fs_entity_init(ent, buf, FsEntityFile);
                break;
        default:
                fs_entity_init(ent, buf, FsEntitySystem);
                break;
        }
}

bool dir_posix_first(struct dir_posix* self, struct fs_entity* ent)
{
        if (!self->ds) {
                if (!(self->ds = opendir(self->actual_path)))
                        return false;
        }
        if (nullptr == (self->entry = readdir(self->ds)))
                return false;
        else {
                __dir_posix_fill_fs_entity(self, ent);
                return true;
        }
}

bool dir_posix_next(struct dir_posix* self, struct fs_entity* ent)
{
        if (self->entry == nullptr || nullptr == (self->entry = readdir(self->ds)))
                return false;
        else {
                __dir_posix_fill_fs_entity(self, ent);
                return true;
        }
}

/*
 * <file_posix> public
 */
struct file_posix* file_posix_create(const char* base, const char* cwd, const char* path, bool is_2create)
{
        struct file_posix* self = malloc(sizeof(*self));
        if (file_posix_init(self, base, cwd, path, is_2create))
                return self;
        else {
                free(self);
                return nullptr;
        }
}

bool file_posix_init(struct file_posix* self, const char* base, const char* cwd, const char* path, bool is_2create)
{
        memset(self, 0, sizeof(*self));

        self->actual_path = (char*) __posix_get_full_path(base, cwd, path);
        if (-1 == access(self->actual_path, F_OK) && !is_2create)
                // File not exists.
                return false;

        if (is_2create) {
                // Open the file right away.
                if (-1 == (self->fd = open(self->actual_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)))
                        return false;
        } else
                // Lazy open policy.
                self->fd = -1;

        file_init(&self->__parent, path,
                  (f_File_Free) file_posix_free,
                  (f_File_Read) file_posix_read,
                  (f_File_Write) file_posix_write,
                  (f_File_Seek) file_posix_seek);
        return true;
}

void file_posix_free(struct file_posix* self)
{
        if (self->fd > 0)
                close(self->fd);
        free(self->actual_path);
}

size_t file_posix_read(struct file_posix* self, size_t num_bytes, void* buf)
{
        if (self->fd == -1) {
                // Lazy open.
                if (-1 == (self->fd = open(self->actual_path, O_RDWR)))
                        return 0;
        }
        return read(self->fd, buf, num_bytes);
}

size_t file_posix_write(struct file_posix* self, size_t num_bytes, const void* buf)
{
        if (self->fd == -1) {
                // Lazy open.
                if (-1 == (self->fd = open(self->actual_path, O_RDWR)))
                        return 0;
        }
        return write(self->fd, buf, num_bytes);
}

size_t file_posix_seek(struct file_posix* self, size_t offset)
{
        abort();
}

#endif // ARCH_X86_64
