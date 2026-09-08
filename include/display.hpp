#pragma once

#include <cstdint>
#include <vector>
#include "raylib.h"
#include "utils.hpp"

class Display {
public:
    std::vector<Color> framebuffer;  // ARGB * 8 bits per = 32 bits
    std::vector<float> zbuffer;

    // ctor
    Display(uint32_t width, uint32_t height, int displayScale=1, int fpsCap=60);
    ~Display();

    void clearFrameBuffer(Color color = {0,0,0,255});
    void clearDepthBuffer(float defaultZ = MAXFLOAT);
    
    void clearBuffers(Color color = {0,0,0,255}, float defaultZ = MAXFLOAT);

    void putPixel(uint32_t x, uint32_t y, Color color);
    void putPixel(Point2D p, Color color);

    void setRowPixels(uint32_t y, const std::vector<Color> &colors);
    void drawBresenhamLine(Point2D a, Point2D b, Color color);

    uint32_t W() const { return width; }
    uint32_t H() const { return height; }

    void renderFramebuffer();

private:
    int fpsCap;
    int displayScale;
    uint32_t width;
    uint32_t height;

    Texture2D framebufferTexture;

};