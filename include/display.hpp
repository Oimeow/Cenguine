#pragma once

#include <cstdint>
#include <vector>
#include "raylib.h"
#include "utils.hpp"

class Display {
public:

    // ctor
    Display(uint32_t width, uint32_t height, int displayScale=1, int fpsCap=60);
    ~Display();

    void clear(uint32_t color = 0);

    void putPixel(uint32_t x, uint32_t y, uint32_t color);
    void putPixel(Point2D p, uint32_t color);

    void setRowPixels(uint32_t y, const std::vector<uint32_t> &colors);
    void drawBresenhamLine(Point2D a, Point2D b, uint32_t color);

    // expose
    uint32_t* data();
    uint32_t W() const { return width; }
    uint32_t H() const { return height; }

    void renderFramebuffer();

private:
    int fpsCap;
    int displayScale;
    uint32_t width;
    uint32_t height;

    std::vector<uint32_t> framebuffer;  // ARGB * 8 bits per = 32 bits

    Texture2D framebufferTexture;

};