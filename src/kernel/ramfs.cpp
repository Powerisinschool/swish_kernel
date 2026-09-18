#include <kernel/ramfs.hpp>

#include "terminal.h"

uint32_t RamFSFile::read(const uint32_t offset, const uint32_t size, void *buffer) {
    if (offset >= length) return (0);

    uint32_t read_size = size;
    if (offset + size > length) {
        read_size = length - offset;
    }

    memcpy(buffer, file_data.begin() + offset, read_size);
    return (read_size);
}

uint32_t RamFSFile::write(const uint32_t offset, const uint32_t size, const void *buffer) {
    if (offset + size > length) file_data.resize(offset + size);
    memcpy(file_data.begin() + offset, buffer, size);
    if (offset + size > length) length = offset + size;
    return (size);
}

dirent *RamFSDirectory::readdir(const uint32_t index) {
    if (index >= children.size()) return (nullptr);

    const auto &child = children[index];
    const auto entry = new dirent{child->inode};
    strncpy(entry->name, child->name, 128);
    return (entry);
}

FSNode *RamFSDirectory::lookup(const char *search_name) {
    for (const auto &child : children) {
        if (*child == search_name) return (child);
    }
    return (nullptr);
}

void RamFSDirectory::add_child(FSNode *child) {
    children.push_back(child);
}
