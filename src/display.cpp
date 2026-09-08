#include <algorithm>
#include "display.hpp"
#include "rendering.hpp"


Display::Display(uint32_t width, uint32_t height, int displayScale, int fpsCap)
  : width(width),
    height(height),
    displayScale(displayScale),
    fpsCap(fpsCap),
    framebuffer(width * height),
    zbuffer(width*height)
{
    SetTargetFPS(fpsCap);

    // initialize the texture
    Image image = {
        framebuffer.data(), (int)width, (int)height, 1,
        PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    framebufferTexture = LoadTextureFromImage(image);
}

Display::~Display() {
    UnloadTexture(framebufferTexture);
}  // dtor

void Display::clearFrameBuffer(Color color) {
    std::fill(framebuffer.begin(), framebuffer.end(), color);
}

void Display::clearDepthBuffer(float defaultZ) {
    std::fill(zbuffer.begin(), zbuffer.end(), defaultZ);
}

void Display::clearBuffers(Color color, float defaultZ) {
    clearFrameBuffer(color);
    clearDepthBuffer(defaultZ);
}

void Display::putPixel(uint32_t x, uint32_t y, Color color) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;

    framebuffer[y*width + x] = color;
}
void Display::putPixel(Point2D p, Color color) {
    if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height) return;

    framebuffer[p.y*width + p.x] = color;
}

void Display::setRowPixels(uint32_t y, const std::vector<Color> &colors) {
    std::copy(colors.begin(), colors.end(), framebuffer.begin() + y * width);
}

void Display::drawBresenhamLine(Point2D a, Point2D b, Color color) {
    int dx = abs(b.x - a.x);
    int dy = abs(b.y - a.y);

    int stepX = (a.x < b.x) ? 1 : -1;
    int stepY = (a.y < b.y) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        putPixel(a.x, a.y, color);

        if (a.x == b.x && a.y == b.y) 
            break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            a.x += stepX;
        }

        if (e2 < dx) {
            err += dx;
            a.y += stepY;
        }
    }

}


void Display::renderFramebuffer() {
    UpdateTexture(framebufferTexture, framebuffer.data());

    if (displayScale == 1) {
        DrawTexture(framebufferTexture, 0, 0, WHITE);
    }

    // scaling lowres
    else {
        SetTextureFilter(framebufferTexture, TEXTURE_FILTER_POINT);
        DrawTexturePro(
            framebufferTexture,
            {0, 0, (float)width, (float)height},
            {0, 0, (float)width * displayScale, (float)height * displayScale},
            {0, 0}, 0.0f, WHITE
        );
    }
}
