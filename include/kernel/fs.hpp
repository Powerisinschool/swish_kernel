#pragma once

#include <stdint.h>

#include "String.hpp"

struct FSNodeFlags {
    uint32_t data;

    static FSNodeFlags FILE;
    static FSNodeFlags DIRECTORY;
    static FSNodeFlags CHARDEVICE;
    static FSNodeFlags BLOCKDEVICE;
    static FSNodeFlags PIPE;
    static FSNodeFlags SYMLINK;
    static FSNodeFlags MOUNTPOINT;

    bool operator==(const FSNodeFlags &other) const;
};

struct dirent {
    uint32_t inode;
    FSNodeFlags type;
    char name[128];
};

class FSNode {
public:
    char name[128];
    FSNodeFlags flags;
    uint32_t length;
    uint32_t inode;
    uint32_t impl;

    virtual ~FSNode() = default;

    // Helper Functions
    virtual void set_flags(FSNodeFlags &type);
    virtual void set_name(const char *new_name);

    // Core File I/O
    virtual uint32_t read(uint32_t offset, uint32_t size, void *buffer);
    virtual uint32_t write(uint32_t offset, uint32_t size, const void *buffer);

    // Lifecycle
    virtual void open();
    virtual void close();

    // Directory Operations
    virtual dirent *readdir(uint32_t index);
    virtual FSNode *lookup(const char *search_name);
    virtual void add_child(FSNode *child);

    bool operator==(const FSNode &other) const;
    bool operator==(const String &comp) const;
};

extern FSNode *fs_root;

void vfs_set_name(FSNode *node, const char *new_name);
uint32_t vfs_read(FSNode *node, uint32_t offset, uint32_t size, void *buffer);
uint32_t vfs_write(FSNode *node, uint32_t offset, uint32_t size, const void *buffer);
void vfs_open(FSNode *node);
void vfs_close(FSNode *node);
dirent *vfs_readdir(FSNode *node, uint32_t index);
FSNode *vfs_lookup(FSNode *node, const char *search_name);
void vfs_add_child(FSNode *node, FSNode *child);
FSNode *vfs_resolve_path(const char *path, FSNode *cwd);
