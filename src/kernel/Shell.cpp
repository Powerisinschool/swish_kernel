#include <kernel/Shell.h>

#include "terminal.h"
#include "kernel/fs.hpp"
#include "kernel/ramfs.hpp"
#include "subsystems/input.h"

static const CommandDef commands[] = {
    { "ls", builtin_ls, "List directory contents" },
    { "cat", builtin_cat, "Display file contents" },
    { "touch", builtin_touch, "Create an empty file" },
    { "mkdir", builtin_mkdir, "Create an empty directory" },
    { "echo", builtin_echo, "Print text to the terminal" },
    { "display", builtin_display, "Switch to the graphical interface" },
    { "exit", builtin_exit, "Exit the shell environment" },
    { "help", builtin_help, "Show this help message" }
};

bool is_shell_builtin(const String& cmd) {
    for (const auto & command : commands) {
        if (cmd == command.name) return true;
    }
    return false;
}

// Forward declaration of the built-in processor
bool Shell::process_builtin(const String& cmd, String* args, const int argCount, OutputStream &output) const {
    for (auto & command : commands) {
        if (cmd == command.name) {
            return command.execute(this, args, argCount, output);
        }
    }

    // output << "[Shell built-in command `" << cmd << "`]\r\n";
    // if (argCount > 1) {
    //     output << "Args:\r\n";
    // }
    // for (int i = 1; i < argCount; i++) {
    //     output << "    - " << args[i] << "\r\n";
    // }

    return false;
}

Shell &Shell::getInstance() {
    static Shell instance;
    return instance;
}

FSNode *Shell::get_current_directory() const {
    return current_directory;
}

void Shell::set_current_directory(FSNode *node) {
    if (node == nullptr) return;
    if (current_directory != nullptr) return;
    current_directory = node;
}

bool Shell::eval_user_input(const String &rawInput, OutputStream *out) const {
    if (rawInput.empty()) {
        return false;
    }

    constexpr int MAX_ARGS = 16;
    String args[MAX_ARGS];
    int argCount = 0;

    const char* str = rawInput.c_str();
    char buffer[256];
    int bufIdx = 0;

    OutputStream& output = (out != nullptr) ? *out : cout;

    // Parse the string character by character
    for (size_t i = 0; i <= rawInput.len(); i++) {
        // If we hit a space or the null terminator, finalize the token
        if (str[i] == ' ' || str[i] == '\0') {
            if (bufIdx > 0) {
                buffer[bufIdx] = '\0'; // Null terminate the buffer
                if (argCount < MAX_ARGS) {
                    args[argCount] = String(buffer);
                    argCount++;
                }
                bufIdx = 0; // Reset buffer for the next word
            }
        } else {
            // Prevent buffer overflow on giant words
            if (bufIdx < 255) {
                buffer[bufIdx++] = str[i];
            }
        }
    }

    if (argCount == 0) {
        return false;
    }

    const String cmd = args[0];

    if (is_shell_builtin(cmd))
    {
        return process_builtin(cmd, args, argCount, output);
    }
    output << "Command not found or external execution not yet supported: " << cmd.c_str() << "\r\n";

    return false;
}

void list_directory(FSNode *node, OutputStream &output) {
    uint32_t index = 0;
    const dirent *entry = vfs_readdir(node, index);

    while (entry != nullptr) {
        output << entry->name << (entry->type == FSNodeFlags::DIRECTORY ? "/" : "") << " ";
        delete entry;

        index++;
        entry = vfs_readdir(node, index);
    }
    output << "\r\n";
}

// Builtin Commands
bool builtin_ls(const Shell *shell, const String *args, const int argCount, OutputStream &output) {
    if (argCount == 1) list_directory(shell->get_current_directory(), output);
    for (int i = 1; i < argCount; i++) {
        const auto existing = vfs_resolve_path(args[i].c_str(), shell->get_current_directory());
        if (existing == nullptr) {
            output << "No such file or directory: " << args[i] << "\r\n";
            continue;
        }

        if (existing->flags == FSNodeFlags::FILE) {
            output << args[i] << "\r\n";
        } else if (existing->flags == FSNodeFlags::DIRECTORY) {
            output << args[i] << ": " << "\r\n";
            list_directory(existing, output);
        }
        if (i < argCount) output << "\r\n";
    }
    return false;
}

bool builtin_cat(const Shell *shell, const String *args, const int argCount, OutputStream &output) {
    for (int i = 1; i < argCount; i++) {
        const auto file = vfs_lookup(fs_root, args[i].c_str());
        if (file == nullptr) {
            output << "No such file or directory: " << args[i] << "\r\n";
            continue;
        }

        if (file->flags != FSNodeFlags::FILE) {
            output << "Not a file: " << args[i] << "\r\n";
            continue;
        }

        auto *buffer = new char[file->length + 1];

        vfs_read(file, 0, file->length, buffer);
        buffer[file->length] = '\0';

        output << buffer;

        delete[] buffer;
    }
    return false;
}

bool builtin_touch(const Shell *shell, const String *args, const int argCount, OutputStream &output) {
    for (int i = 1; i < argCount; i++) {
        // Check if the file already exists
        const auto existing = vfs_lookup(fs_root, args[i].c_str());
        if (existing != nullptr) {
            // Real UNIX updates the modified timestamp here.
            // We do not have time tracking yet, so we just skip it.
            continue;
        }

        // Instantiate the new file
        auto *new_file = new RamFSFile();
        new_file->set_flags(FSNodeFlags::FILE);
        vfs_set_name(new_file, args[i].c_str());

        // Attach it to the root directory
        vfs_add_child(fs_root, new_file);
    }
    return false;
}

bool builtin_mkdir(const Shell *shell, const String *args, const int argCount, OutputStream &output) {
    for (int i = 1; i < argCount; i++) {
        // Check if a node with that name exists
        const auto existing = vfs_lookup(fs_root, args[i].c_str());
        if (existing != nullptr) {
            output << args[i] << " already exists" << "\r\n";
            continue;
        }

        // Instantiate the new directory
        auto *new_dir = new RamFSDirectory();
        new_dir->set_flags(FSNodeFlags::DIRECTORY);
        vfs_set_name(new_dir, args[i].c_str());

        vfs_add_child(fs_root, new_dir);
    }
    return false;
}

bool builtin_echo(const Shell *shell, const String *args, const int argCount, OutputStream &output) {
    for (int i = 1; i < argCount - 1; i++) {
        output << args[i] << " ";
    }
    output << args[argCount - 1] << "\r\n";
    return false;
}

bool builtin_display(const Shell *shell, const String *args, const int argCount, OutputStream &output) {
    Input::switch_to_gui();
    return false;
}

bool builtin_exit(const Shell *shell, const String *args, const int argCount, OutputStream &output) {
    return true;
}

bool builtin_help(const Shell *shell, const String *args, const int argCount, OutputStream &output) {
    output << "Type a command `help` for the command's help.\r\n";
    output << "Available commands:\r\n";
    for (auto & command : commands) {
        // Output formatting: "ls        - List directory contents"
        output << "  " << command.name << " - " << command.help_text << "\r\n";
    }
    return false;
}
