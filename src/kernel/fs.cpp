#include <kernel/fs.hpp>

FSNodeFlags FSNodeFlags::FILE = {0x01};
FSNodeFlags FSNodeFlags::DIRECTORY = {0x02};
FSNodeFlags FSNodeFlags::CHARDEVICE = {0x03};
FSNodeFlags FSNodeFlags::BLOCKDEVICE = {0x04};
FSNodeFlags FSNodeFlags::PIPE = {0x05};
FSNodeFlags FSNodeFlags::SYMLINK = {0x06};
FSNodeFlags FSNodeFlags::MOUNTPOINT = {0x07};

bool FSNodeFlags::operator==(const FSNodeFlags &other) const {
    return (this->data == other.data);
}

FSNode *fs_root = nullptr;

void FSNode::set_flags(FSNodeFlags &type) {
    flags = type;
}

void FSNode::set_name(const char *new_name) {
    strncpy(name, new_name, strlen(new_name) + 1);
}

uint32_t FSNode::read(uint32_t offset, uint32_t size, void *buffer) {
    return (0);
}

uint32_t FSNode::write(uint32_t offset, uint32_t size, const void *buffer) {
    return (0);
}

void FSNode::open() {}
void FSNode::close() {}

dirent *FSNode::readdir(uint32_t index) {
    return (nullptr);
}

FSNode *FSNode::lookup(const char *search_name) {
    return (nullptr);
}

void FSNode::add_child(FSNode *child) {}

void vfs_set_name(FSNode *node, const char *name) {
    if (node == nullptr) return;
    node->set_name(name);
}

uint32_t vfs_read(FSNode *node, const uint32_t offset, const uint32_t size, void *buffer) {
    if (node == nullptr) return 0;
    return node->read(offset, size, buffer);
}

uint32_t vfs_write(FSNode *node, const uint32_t offset, const uint32_t size, const void *buffer) {
    if (node == nullptr) return 0;
    return node->write(offset, size, buffer);
}

void vfs_open(FSNode *node) {
    if (node == nullptr) return;
    node->open();
}

void vfs_close(FSNode *node) {
    if (node == nullptr) return;
    node->close();
}

dirent *vfs_readdir(FSNode *node, const uint32_t index) {
    if (node == nullptr || node->flags != FSNodeFlags::DIRECTORY) return nullptr;
    return node->readdir(index);
}

FSNode *vfs_lookup(FSNode *node, const char *search_name) {
    if (node == nullptr || node->flags != FSNodeFlags::DIRECTORY) return nullptr;
    return node->lookup(search_name);
}

void vfs_add_child(FSNode *node, FSNode *child) {
    if (node == nullptr || node->flags != FSNodeFlags::DIRECTORY) return;
    node->add_child(child);
}

bool FSNode::operator==(const FSNode &other) const {
    return (this->inode == other.inode);
}

bool FSNode::operator==(const String &comp) const {
    return (comp == name);
}

FSNode *vfs_resolve_path(const char *path, FSNode *cwd) {
    if (path == nullptr || *path == '\0') return nullptr;

    FSNode *current_node = cwd;

    if (*path == '/') {
        current_node = fs_root;
        while (*path == '/') path++;
    }

    while (current_node != nullptr && current_node->flags == FSNodeFlags::DIRECTORY) {
        char segment[128];
        size_t index = 0;

        while (*path != '/' && *path != '\0') {
            segment[index++] = *path;
            path++;
        }
        segment[index] = '\0';

        while (*path == '/') path++; // Consume all consecutive slashes

        if (strlen(segment) == 1 && segment[0] == '.') continue;
        if (strlen(segment) == 2 && segment[0] == '.' && segment[1] == '.') {
            if (current_node->parent != nullptr) {
                current_node = current_node->parent;
            }
            continue;
        }

        if (strlen(segment) == 0) break;

        current_node = vfs_lookup(current_node, segment);
    }

    return (current_node);
}

void vfs_split_path(const char *full_path, char *dirname, size_t dir_max, char *basename, size_t base_max) {
    if (dir_max == 0 || base_max == 0) return;

    const size_t len = strlen(full_path);
    int last_slash = -1;

    // Scan backwards to find the final delimiter
    for (int i = len - 1; i >= 0; i--) {
        if (full_path[i] == '/') {
            last_slash = i;
            break;
        }
    }

    if (last_slash == -1) {
        // No slashes found. Directory is current ("."), file is the whole string.
        strncpy(dirname, ".", dir_max);
        strncpy(basename, full_path, base_max);
    } else if (last_slash == 0) {
        // The file is located exactly at the root directory "/"
        strncpy(dirname, "/", dir_max);
        strncpy(basename, full_path + 1, base_max);
    } else {
        // Located in a nested directory
        size_t copy_len = (last_slash < dir_max - 1) ? last_slash : dir_max - 1;
        strncpy(dirname, full_path, copy_len);
        dirname[copy_len] = '\0';

        strncpy(basename, full_path + last_slash + 1, base_max);
    }
}
