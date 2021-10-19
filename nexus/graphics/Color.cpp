#include "Color.hpp"

USING_NAMESPACE_NXS;

Color::Color()
{
    r = g = b = a = 0;
}

Color::Color(float red, float green, float blue)
: r(red)
, g(green)
, b(blue)
, a(1.0f)
{}

Color::Color(float red, float green, float blue, float alpha)
: r(red)
, g(green)
, b(blue)
, a(alpha)
{}

Color::Color(const Color4B& rhs)
: r(rhs.r / 255.0f)
, g(rhs.g / 255.0f)
, b(rhs.b / 255.0f)
, a(rhs.a / 255.0f)
{}

const Color Color::WHITE  (1.00f, 1.00f, 1.00f, 1.00f);
const Color Color::BLACK  (0.00f, 0.00f, 0.00f, 1.00f);
const Color Color::RED    (1.00f, 0.00f, 0.00f, 1.00f);
const Color Color::GREEN  (0.00f, 1.00f, 0.00f, 1.00f);
const Color Color::BLUE   (0.00f, 0.00f, 1.00f, 1.00f);
const Color Color::YELLOW (1.00f, 1.00f, 0.00f, 1.00f);
const Color Color::MAGENTA(1.00f, 0.00f, 1.00f, 1.00f);
const Color Color::ORANGE (1.00f, 0.50f, 0.00f, 1.00f);
const Color Color::GRAY   (0.65f, 0.65f, 0.65f, 1.00f);

Color4B::Color4B()
{
    r = g = b = a = 0;
}

Color4B::Color4B(float red, float green, float blue)
: r(red)
, g(green)
, b(blue)
, a(0xFF)
{}

Color4B::Color4B(float red, float green, float blue, float alpha)
: r(red)
, g(green)
, b(blue)
, a(alpha)
{}

Color4B::Color4B(const Color& rhs)
: r(rhs.r * 255)
, g(rhs.g * 255)
, b(rhs.b * 255)
, a(rhs.a * 255)
{}

const Color4B Color4B::WHITE  (0xFF, 0xFF, 0xFF, 0xFF);
const Color4B Color4B::BLACK  (0x00, 0x00, 0x00, 0xFF);
const Color4B Color4B::RED    (0xFF, 0x00, 0x00, 0xFF);
const Color4B Color4B::GREEN  (0x00, 0xFF, 0x00, 0xFF);
const Color4B Color4B::BLUE   (0x00, 0x00, 0xFF, 0xFF);
const Color4B Color4B::YELLOW (0xFF, 0xFF, 0x00, 0xFF);
const Color4B Color4B::MAGENTA(0xFF, 0x00, 0xFF, 0xFF);
const Color4B Color4B::ORANGE (0xFF, 0x9B, 0x00, 0xFF);
const Color4B Color4B::GRAY   (0xA6, 0xA6, 0xA6, 0xFF);