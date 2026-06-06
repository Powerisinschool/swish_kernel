#pragma once
#include <stddef.h>
#include <stdint.h>

#include "Graphics.h"

class Window {
public:
    size_t x, y, width, height;

    explicit Window(size_t x, size_t y, size_t width, size_t height);

    /**
     * @brief Copy the window to the screen row by row to respect the screen's pitch
     */
    void render() const;
private:
    uint32_t *pixel_buffer;
};
