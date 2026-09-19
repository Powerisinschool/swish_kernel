#pragma once
#include "fs.hpp"
#include "OutputStream.hpp"
#include "String.hpp"

class Shell;
inline void using_history() {}
inline void read_history(const char* file) {}
inline void write_history(const char* file) {}
inline void add_history(const String& input) {}

typedef bool (*CommandCallback)(const Shell *shell, const String *args, int argCount, OutputStream &output);

struct CommandDef {
    const char *name;
    CommandCallback execute;
    const char *help_text;
};

static bool builtin_ls(const Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_cat(const Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_touch(const Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_mkdir(const Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_echo(const Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_display(const Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_exit(const Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_help(const Shell *shell, const String *args, int argCount, OutputStream &output);

class Shell {
public:
    Shell(const Shell &) = delete;
    Shell &operator=(const Shell &) = delete;

    static Shell &getInstance();

    String homeEnv;
    String pathEnv;
    String HISTORY_FILE;
    
    [[nodiscard]] FSNode *get_current_directory() const;
    void set_current_directory(FSNode *node);

    // Core evaluation loop
    bool eval_user_input(const String& rawInput, OutputStream *out = nullptr) const;

private:
    FSNode *current_directory;

    Shell() = default;
    bool process_builtin(const String& cmd, String* args, int argCount, OutputStream &output) const;
};
