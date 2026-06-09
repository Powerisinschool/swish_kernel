#pragma once
#include "String.hpp"

inline void using_history() {}
inline void read_history(const char* file) {}
inline void write_history(const char* file) {}
inline void add_history(const String& input) {}

class Shell {
public:
    Shell(const Shell &) = delete;
    Shell &operator=(const Shell &) = delete;

    static Shell &getInstance();

    String homeEnv;
    String pathEnv;
    String HISTORY_FILE;

    // Core evaluation loop
    bool eval_user_input(const String& rawInput);

private:
    Shell() = default;
    static bool process_builtin(const String& cmd, String* args, int argCount);
    static void display_help();
};
