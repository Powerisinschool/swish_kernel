#pragma once
#include <string.h>
#include <stddef.h>

class String {
private:
    char *data;
    size_t length;

public:
    String();
    String(const char *str);
    String(const String& other);
    ~String();

    String &operator=(const String& other);
    bool operator==(const char *other) const;

    const char *c_str() const;
    bool empty() const;
    size_t len() const;

    using iterator = char *;
    using const_iterator = const char *;

    iterator begin();
    iterator end();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;
};
