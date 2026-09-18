#pragma once
#include <stddef.h>
#include <string.h>

template <typename T>
class Vector {
public:
    Vector() : data(nullptr), m_size(0), m_capacity(0) {}

    Vector(const Vector &other) : m_size(other.m_size), m_capacity(other.m_capacity) {
        data = new T[m_capacity];
        for (size_t i = 0; i < m_size; i++) {
            data[i] = other.data[i];
        }
    }

    ~Vector() {
        delete[] data;
    }

    Vector &operator=(const Vector &other) {
        if (this != &other) {
            delete[] data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            data = new T[m_capacity];
            for (size_t i = 0; i < m_size; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    void push_back(const T &value) {
        if (m_size >= m_capacity) {
            reallocate(m_capacity == 0 ? 2 : m_capacity * 2);
        }
        data[m_size++] = value;
    }

    void pop_back() {
        if (m_size > 0) m_size--;
    }

    void resize(const size_t new_size) {
        if (m_size >= new_size) return;
        while (m_capacity < new_size) {
            reallocate(m_capacity == 0 ? 2 : m_capacity * 2);
        }
        memset(data + m_size, 0, (new_size - m_size) * sizeof(T));
        m_size = new_size;
    }

    [[nodiscard]] size_t size() const { return m_size; }
    [[nodiscard]] size_t capacity() const { return m_capacity; }
    [[nodiscard]] bool empty() const { return m_size == 0; }

    T &operator[](size_t index) {
        // if (index >= m_size) {
        //     throw std::out_of_range("Index out of range");
        // }
        return data[index];
    }
    const T &operator[](size_t index) const {
        return data[index];
    }

    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() { return data; }
    [[nodiscard]] const_iterator begin() const { return data; }
    iterator end() { return data + m_size; }
    [[nodiscard]] const_iterator end() const { return data + m_size; }
private:
    T *data;
    size_t m_size;
    size_t m_capacity;

    void reallocate(const size_t new_capacity) {
        T *new_data = new T[new_capacity];
        for (size_t i = 0; i < m_size; i++) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        m_capacity = new_capacity;
    }
};
