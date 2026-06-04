#include <kernel/String.hpp>

String::String() : length(0) {
    data = new char[1];
    data[0] = '\0';
}

String::String(const char *str) {
    if (str) {
        length = strlen(str);
        data = new char[length + 1];
        memcpy(data, str, length);
        data[length] = '\0';
    } else {
        length = 0;
        data = new char[1];
        data[0] = '\0';
    }
}

String::String(const String& other) {
    length = other.length;
    data = new char[length + 1];
    memcpy(data, other.data, length + 1);
}

String::~String() {
    delete[] data;
}

String &String::operator=(const String& other) {
    if (this != &other) {
        delete[] data;
        length = other.length;
        data = new char[length + 1];
        memcpy(data, other.data, length + 1);
    }
    return *this;
}

// Add to String.hpp public methods
bool String::operator==(const char* other) const {
    if (!data || !other) return false;
    size_t i = 0;
    while (data[i] && other[i]) {
        if (data[i] != other[i]) return false;
        i++;
    }
    return data[i] == other[i];
}

const char* String::c_str() const { return data; }
bool String::empty() const { return length == 0; }
size_t String::len() const { return length; }
