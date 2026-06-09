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

    void on_mouse_button(uint64_t local_x, uint64_t local_y, uint8_t button, bool is_down) const;

    [[nodiscard]] const Surface *get_surface() const;
private:
    char text_buffer[33] = {0};
    Surface *surface = nullptr;
};
