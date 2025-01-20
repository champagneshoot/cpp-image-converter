#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <vector>

namespace img_lib {

struct Size {
    int width;
    int height;
};

struct Color 
{
    static Color Black() {
        return {std::byte{0}, std::byte{0}, std::byte{0}, std::byte{255}};
    }

    std::byte r, g, b, a;
};

class Image 
{
public:
    Image() = default;

    Image(int w, int h, Color fill);

    Color GetPixel(int x, int y) const {
        return const_cast<Image*>(this)->GetPixel(x, y);
    }
    Color& GetPixel(int x, int y) {
        assert(x < GetWidth() && y < GetHeight() && x >= 0 && y >= 0);
        return GetLine(y)[x];
    }

    Color* GetLine(int y);
    const Color* GetLine(int y) const;

    int GetWidth() const;
    int GetHeight() const;
    int GetStep() const;

    explicit operator bool() const {
        return GetWidth() > 0 && GetHeight() > 0;
    }

    bool operator!() const {
        return !operator bool();
    }

private:
    int width_ = 0;
    int height_ = 0;
    int step_;

    std::vector<Color> pixels_;
};

} 
