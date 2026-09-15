#include <gui/Window.h>

#include "string.h"
#include "kernel/memory.hpp"

Window::Window(const size_t x, const size_t y, const size_t width, const size_t height, const Color bg_color) : loc(x, y), cursor(0, 0)
{
    const auto pixel_buffer = static_cast<uint32_t *>(kmalloc(width * height * sizeof(uint32_t)));
    surface = new Surface{pixel_buffer, width, height, width * sizeof(uint32_t), bg_color};

    const auto bg_bytes = bg_color.get_bytes();
    for (size_t i = 0; i < width * height; i++) {
        pixel_buffer[i] = bg_bytes;
    }

    cols = width / Graphics::get_font_width();
    rows = height / Graphics::get_font_height();
    text_grid = static_cast<char *>(kmalloc(cols * rows));
    memset(text_grid, 0, cols * rows);
}

void Window::inject_key(const char c) {
    if (cols == 0 || rows == 0) return;

    if (c == '\n') {
        cursor.x = 0;
        cursor.y++;
        scroll_forward();
        return;
    }

    if (c == '\b') {
        if (cursor.x > 0) {
            text_grid[cursor.y * cols + --cursor.x] = ' ';
        } else if (cursor.y > 0) {
            cursor.y--;
            for (size_t col = 0; col < cols; col++) {
                if (text_grid[cursor.y * cols + col] == '\0') break;
                cursor.x++;
            }
        }
        return;
    }

    text_grid[cursor.y * cols + cursor.x++] = c;
    if (cursor.x < cols) text_grid[cursor.y * cols + cursor.x] = 0;
    if (cursor.x >= cols) {
        cursor.x = 0;
        cursor.y++;
    }
    scroll_forward();
}

void Window::scroll_forward() {
    if (cursor.y >= rows) {
        memmove(text_grid, text_grid + cols, (rows - 1) * cols);
        memset(text_grid + (rows - 1) * cols, 0, cols);
        cursor.y = rows - 1;
    }
}

void Window::render(bool focused) const {
    Graphics::draw_bg(surface);
    // Graphics::draw_string(surface, 10, 10, text_buffer, Color::black);
    const size_t fw = Graphics::get_font_width();
    const size_t fh = Graphics::get_font_height();

    // Draw the cursor to screen
    if (focused) Graphics::draw_rect_filled(surface, cursor.x * fw, cursor.y * fh, fw, fh, Color::gray);

    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            const char c = text_grid[row * cols + col];
            if (c == '\0') break; // skip the current row
            if (c == ' ') continue; // skip the current cell
            Graphics::draw_char(surface, col * fw, row * fh, c, Color::black);
        }
    }
    for (size_t row = 0; row < surface->height; row++) {
        uint32_t *screen_row_ptr = Graphics::get_buffer_address(loc.x, loc.y + row);
        if (!screen_row_ptr) continue;

        const uint32_t *window_row_ptr = surface->buffer + (row * surface->width);
        memcpy(screen_row_ptr, window_row_ptr, surface->width * sizeof(uint32_t));
    }
}

void Window::on_mouse_button(uint64_t local_x, uint64_t local_y, uint8_t button, const bool is_down) {
    if (is_down) {
        temp_bg = surface->background;
        Graphics::set_bg(surface, Color::red);
    } else {
        Graphics::set_bg(surface, temp_bg);
    }
}

const Surface *Window::get_surface() const {
    return surface;
}
