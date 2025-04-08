#pragma once

struct Color : public Variant
{
    float r, g, b, a;

    // Default constructor
    constexpr Color()                                               : r(0.0f), g(0.0f), b(0.0f), a(255.0f) {}
    constexpr Color(float r, float g, float b, float a = 255.0f)    : r(r), g(g), b(b), a(a) {}
    constexpr Color(uint32 r, uint32 g, uint32 b, uint32 a = 255)   : r((float)r * (1.0f / 255.0f)), g((float)g * (1.0f / 255.0f)), b((float)b * (1.0f / 255.0f)), a((float)a* (1.0f / 255.0f)) {}
    constexpr Color(const Vector4& col)                             : r(col.x), g(col.y), b(col.z), a(col.w) {}

    // Color operations
    Color operator+(const Color& other) const { return Color{ r + other.r, g + other.g, b + other.b, a }; }
    Color operator-(const Color& other) const { return Color{ r - other.r, g - other.g, b - other.b, a }; }
    Color operator*(const Color& other) const { return Color{ r * other.r, g * other.g, b * other.b, a }; }
    Color operator/(const Color& other) const { return Color{ r / other.r, g / other.g, b / other.b, a }; }

    Color operator+(float scalar) const { return Color{ r + scalar, g + scalar, b + scalar, a }; }
    Color operator-(float scalar) const { return Color{ r - scalar, g - scalar, b - scalar, a }; }
    Color operator*(float scalar) const { return Color{ r * scalar, g * scalar, b * scalar, a }; }
    Color operator/(float scalar) const { return Color{ r / scalar, g / scalar, b / scalar, a }; }

    Color& operator+=(const Color& other) { r += other.r; g += other.g; b += other.b; return *this; }
    Color& operator-=(const Color& other) { r -= other.r; g -= other.g; b -= other.b; return *this; }
    Color& operator*=(const Color& other) { r *= other.r; g *= other.g; b *= other.b; return *this; }
    Color& operator/=(const Color& other) { r /= other.r; g /= other.g; b /= other.b; return *this; }

    Color& operator+=(float scalar) { r += scalar; g += scalar; b += scalar; return *this; }
    Color& operator-=(float scalar) { r -= scalar; g -= scalar; b -= scalar; return *this; }
    Color& operator*=(float scalar) { r *= scalar; g *= scalar; b *= scalar; return *this; }
    Color& operator/=(float scalar) { r /= scalar; g /= scalar; b /= scalar; return *this; }

    // Comparison operators
    bool operator==(const Color& other) const { return r == other.r && g == other.g && b == other.b && a == other.a; }
    bool operator!=(const Color& other) const { return !(*this == other); }

    // Static colors
    static Color RED()      { return Color(1.0f, 0.0f, 0.0f); }
    static Color GREEN()    { return Color(0.0f, 1.0f, 0.0f); }
    static Color BLUE()     { return Color(0.0f, 0.0f, 1.0f); }
    static Color WHITE()    { return Color(1.0f, 1.0f, 1.0f); }
    static Color BLACK()    { return Color(0.0f, 0.0f, 0.0f); }
    static Color YELLOW()   { return Color(1.0f, 1.0f, 0.0f); }
    static Color CYAN()     { return Color(0.0f, 1.0f, 1.0f); }
    static Color MAGENTA()  { return Color(1.0f, 0.0f, 1.0f); }


    void toHSV(float& h, float& s, float& v) const {
        float max = std::max({ r, g, b });
        float min = std::min({ r, g, b });
        v = max;

        float delta = max - min;
        if (max == 0) {
            s = 0;
            h = 0;
            return;
        }

        s = delta / max;

        if (r == max) {
            h = (g - b) / delta + (g < b ? 6 : 0);
        }
        else if (g == max) {
            h = (b - r) / delta + 2;
        }
        else {
            h = (r - g) / delta + 4;
        }

        h /= 6;
    }

    void toRGB(float h, float s, float v) {
        int i = static_cast<int>(h * 6);
        float f = h * 6 - i;
        float p = v * (1 - s);
        float q = v * (1 - f * s);
        float t = v * (1 - (1 - f) * s);

        switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        }
    }
};

// Color Utilities
Color        ColorConvertU32ToFloat4(uint32_t in);
uint32_t         ColorConvertFloat4ToU32(const Color& in);
void          ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v);
void          ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);