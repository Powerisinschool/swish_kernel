#pragma once
#include "fs.hpp"
#include "OutputStream.hpp"
#include "String.hpp"

class Shell;
inline void using_history() {}
inline void read_history(const char* file) {}
inline void write_history(const char* file) {}
inline void add_history(const String& input) {}

typedef bool (*CommandCallback)(Shell *shell, const String *args, int argCount, OutputStream &output);

struct CommandDef {
    const char *name;
    CommandCallback execute;
    const char *help_text;
};

static bool builtin_cd(Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_ls(Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_cat(Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_touch(Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_mkdir(Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_mount(Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_echo(Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_display(Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_exit(Shell *shell, const String *args, int argCount, OutputStream &output);
static bool builtin_help(Shell *shell, const String *args, int argCount, OutputStream &output);

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
    bool eval_user_input(const String& rawInput, OutputStream *out = nullptr);

private:
    FSNode *current_directory = nullptr;

    Shell() = default;
    bool process_builtin(const String& cmd, String* args, int argCount, OutputStream &output);
};
