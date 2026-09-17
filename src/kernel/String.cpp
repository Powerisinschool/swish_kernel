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
String::iterator String::begin() { return data; }
String::iterator String::end() { return data + length; }
String::const_iterator String::begin() const { return data; }
String::const_iterator String::end() const { return data + length; }
String::reference String::operator[](size_t index) { return data[index]; }
String::reference String::at(size_t index) {
    if (index >= length) return data[0];
    return data[index];
}
String::reference String::front() { return data[0]; }
String::reference String::back() { return data[length - 1]; }
String::const_reference String::operator[](size_t index) const { return data[index]; }
String::const_reference String::at(size_t index) const {
    if (index >= length) return data[0];
    return data[index];
}
String::const_reference String::front() const { return data[0]; }
String::const_reference String::back() const { return data[length - 1]; }
