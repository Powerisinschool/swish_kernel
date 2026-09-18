#pragma once
#include <kernel/Vector.hpp>
#include "fs.hpp"

class RamFSFile : public FSNode {
public:
    Vector<uint8_t> file_data;

    uint32_t read(uint32_t offset, uint32_t size, void *buffer) override;
    uint32_t write(uint32_t offset, uint32_t size, const void *buffer) override;
};

class RamFSDirectory : public FSNode {
public:
    // Vector<RamFSFile> files;
    // Vector<RamFSDirectory> directories;
    Vector<FSNode*> children;

    dirent *readdir(uint32_t index) override;
    FSNode *lookup(const char *search_name) override;
    void add_child(FSNode *child) override;
};
