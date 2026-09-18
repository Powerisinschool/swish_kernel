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

uint32_t FSNode::read(uint32_t offset, uint32_t size, void *buffer) {
    return (0);
}

uint32_t FSNode::write(uint32_t offset, uint32_t size, void *buffer) {
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

uint32_t vfs_read(FSNode *node, const uint32_t offset, const uint32_t size, void *buffer) {
    if (node == nullptr) return 0;
    return node->read(offset, size, buffer);
}

uint32_t vfs_write(FSNode *node, const uint32_t offset, const uint32_t size, void *buffer) {
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

bool FSNode::operator==(const FSNode &other) const {
    return (this->inode == other.inode);
}

bool FSNode::operator==(const String &comp) const {
    return (comp == name);
}
