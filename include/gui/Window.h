#pragma once
#include <stddef.h>
#include <stdint.h>

#include "Graphics.h"

class Window {
public:
    int8_t char_index = 0;
    size_t x, y;

    explicit Window(size_t x, size_t y, size_t width, size_t height);

    void inject_key(char c);

    /**
     * @brief Copy the window to the screen row by row to respect the screen's pitch
     */
    void render() const;
private:
    char text_buffer[33] = {0};
    Surface *surface = nullptr;
};
