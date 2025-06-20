#pragma once

#include <nexus/NxsDefine.h>
#include <glm/glm.hpp>
#include <cstdint>

NXS_NAMESPACE
{
    template<typename T>
    class Color4 : public glm::vec<4, T>
    {
    public:
        Color4() = default;
        Color4(const Color4& color) = default;
        Color4(Color4&& color) = default;

        explicit Color4(const glm::vec<4, T>& color)
            : glm::vec<4, T>(color)
        {}
        explicit Color4(glm::vec<3, T>&& color)
            : glm::vec<4, T>(color)
        {}

        Color4(T _r, T _g, T _b)
        {
            this->r = _r;
            this->g = _g;
            this->b = _b;
            this->a = 1;
        }
        Color4(T _r, T _g, T _b, T _a)
        {
            this->r = _r;
            this->g = _g;
            this->b = _b;
            this->a = _a;
        }

        Color4& operator=(const Color4& color) = default;
        Color4& operator=(const glm::vec<4, T>& color)
        {
            this->r = color.r;
            this->g = color.g;
            this->b = color.b;
            this->a = color.a;
            return *this;
        }
    };

    template<typename T>
    class Color3 : public glm::vec<3, T>
    {
    public:
        Color3() = default;
        Color3(const Color3& color) = default;
        Color3(Color3&& color) = default;

        explicit Color3(const glm::vec<3, T>& color)
            : glm::vec<3, T>(color)
        {}
        explicit Color3(glm::vec<3, T>&& color)
            : glm::vec<3, T>(color)
        {}

        Color3(T _r, T _g, T _b)
        {
            this->r = _r;
            this->g = _g;
            this->b = _b;
        }

        Color3& operator=(const Color3& color) = default;
        Color3& operator=(const glm::vec<3, T>& color)
        {
            this->r = color.r;
            this->g = color.g;
            this->b = color.b;
            return *this;
        }
    };

    /**
     * Color data in 4 floating points RGBA format.
     * Each component can have a value between [0, 1.0] for the standard dynamic range.
     */
    class  Color4F : public Color4<float>
    {
    public:
        Color4F() = default;
        Color4F(float _r, float _g, float _b)
            : Color4(_r, _g, _b)
        {}
        Color4F(float _r, float _g, float _b, float _a)
            : Color4(_r, _g, _b, _a)
        {}
        explicit Color4F(const glm::vec<4, float>& color)
            : Color4(color)
        {}
        explicit Color4F(glm::vec<4, float>&& color)
            : Color4(color)
        {}

        [[nodiscard]] uint32_t RGBA() const
        {
            const auto _r = CAST<uint8_t>(this->r * 255);
            const auto _g = CAST<uint8_t>(this->g * 255);
            const auto _b = CAST<uint8_t>(this->b * 255);
            const auto _a = CAST<uint8_t>(this->a * 255);
            return _a | _b << 8 | _g << 16 | _r << 24;
        }

        explicit operator uint32_t() const
        {
            return RGBA();
        }

        Color4F& operator = (const uint32_t color)
        {
            this->r = CAST<float>(color >> 24 & 0xFF)  / 255.0f;
            this->g = CAST<float>(color >> 16 & 0xFF)  / 255.0f;
            this->b = CAST<float>(color >>  8 & 0xFF)  / 255.0f;
            this->a = CAST<float>(color       & 0xFF)  / 255.0f;
            return *this;
        }
    };
    /**
     * Color data in 3 floating points RGB format.
     * Each component can have a value between [0, 1.0] for the standard dynamic range.
     */
    class  Color3F : public Color3<float>
    {
    public:
        Color3F() = default;
        Color3F(float _r, float _g, float _b)
            : Color3(_r, _g, _b)
        {}
        explicit Color3F(const glm::vec<3, float>& color)
            : Color3(color)
        {}
        explicit Color3F(glm::vec<3, float>&& color)
            : Color3(color)
        {}

        [[nodiscard]] uint32_t RGBA() const
        {
            const auto _r = CAST<uint8_t>(this->r * 255);
            const auto _g = CAST<uint8_t>(this->g * 255);
            const auto _b = CAST<uint8_t>(this->b * 255);
            return _b | _g << 8 | _r << 16;
        }

        explicit operator uint32_t() const
        {
            return RGBA();
        }

        Color3F& operator = (const uint32_t color)
        {
            this->r = CAST<float>(color >> 24 & 0xFF)  / 255.0f;
            this->g = CAST<float>(color >> 16 & 0xFF)  / 255.0f;
            this->b = CAST<float>(color >>  8 & 0xFF)  / 255.0f;
            return *this;
        }
    };
    /**
     * Color data in 4 unsigned byte RGBA format.
     * Each component can have a value between [0x00, 0xFF] for the standard dynamic range.
     */
    class  Color4B : public Color4<uint8_t>
    {
    public:
        Color4B() = default;
        Color4B(uint8_t _r, uint8_t _g, uint8_t _b)
            : Color4(_r, _g, _b)
        {}
        Color4B(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
            : Color4(_r, _g, _b, _a)
        {}
        explicit Color4B(const glm::vec<4, uint8_t>& color)
            : Color4(color)
        {}
        explicit Color4B(glm::vec<4, uint8_t>&& color)
            : Color4(color)
        {}

        [[nodiscard]] uint32_t RGBA() const
        {
            return this->a | this->b << 8 | this->g << 16 | this->r << 24;
        }

        explicit operator uint32_t() const
        {
            return RGBA();
        }

        Color4B& operator = (const uint32_t color)
        {
            this->r = CAST<uint8_t>(color >> 24 & 0xFF);
            this->g = CAST<uint8_t>(color >> 16 & 0xFF);
            this->b = CAST<uint8_t>(color >>  8 & 0xFF);
            this->a = CAST<uint8_t>(color       & 0xFF);
            return *this;
        }
    };
    /**
     * Color data in 3 unsigned byte RGB format.
     * Each component can have a value between [0x00, 0xFF] for the standard dynamic range.
     */
    class  Color3B : public Color3<uint8_t>
    {
    public:
        Color3B() = default;
        Color3B(uint8_t _r, uint8_t _g, uint8_t _b)
            : Color3(_r, _g, _b)
        {}
        explicit Color3B(const glm::vec<3, uint8_t>& color)
            : Color3(color)
        {}
        explicit Color3B(glm::vec<3, uint8_t>&& color)
            : Color3(color)
        {}

        [[nodiscard]] uint32_t RGB() const
        {
            return this->b | this->g << 8 | this->r << 16;
        }

        explicit operator uint32_t() const
        {
            return RGB();
        }

        Color3B& operator = (const uint32_t color)
        {
            this->r = CAST<uint8_t>(color >> 16 & 0xFF);
            this->g = CAST<uint8_t>(color >>  8 & 0xFF);
            this->b = CAST<uint8_t>(color       & 0xFF);
            return *this;
        }
    };
    
    static const Color4F COLOR4F_WHITE  (1.00f, 1.00f, 1.00f, 1.00f);
    static const Color4F COLOR4F_BLACK  (0.00f, 0.00f, 0.00f, 1.00f);
    static const Color4F COLOR4F_RED    (1.00f, 0.00f, 0.00f, 1.00f);
    static const Color4F COLOR4F_GREEN  (0.00f, 1.00f, 0.00f, 1.00f);
    static const Color4F COLOR4F_BLUE   (0.00f, 0.00f, 1.00f, 1.00f);
    static const Color4F COLOR4F_YELLOW (1.00f, 1.00f, 0.00f, 1.00f);
    static const Color4F COLOR4F_MAGENTA(1.00f, 0.00f, 1.00f, 1.00f);
    static const Color4F COLOR4F_ORANGE (1.00f, 0.50f, 0.00f, 1.00f);
    static const Color4F COLOR4F_GRAY   (0.65f, 0.65f, 0.65f, 1.00f);
    static const Color4F COLOR4F_GREY   (0.65f, 0.65f, 0.65f, 1.00f);
    
    static const Color3F COLOR3F_WHITE  (1.00f, 1.00f, 1.00f);
    static const Color3F COLOR3F_BLACK  (0.00f, 0.00f, 0.00f);
    static const Color3F COLOR3F_RED    (1.00f, 0.00f, 0.00f);
    static const Color3F COLOR3F_GREEN  (0.00f, 1.00f, 0.00f);
    static const Color3F COLOR3F_BLUE   (0.00f, 0.00f, 1.00f);
    static const Color3F COLOR3F_YELLOW (1.00f, 1.00f, 0.00f);
    static const Color3F COLOR3F_MAGENTA(1.00f, 0.00f, 1.00f);
    static const Color3F COLOR3F_ORANGE (1.00f, 0.50f, 0.00f);
    static const Color3F COLOR3F_GRAY   (0.65f, 0.65f, 0.65f);
    static const Color3F COLOR3F_GREY   (0.65f, 0.65f, 0.65f);
    
    static const Color4B COLOR4B_WHITE  (0xFF, 0xFF, 0xFF, 0xFF);
    static const Color4B COLOR4B_BLACK  (0x00, 0x00, 0x00, 0xFF);
    static const Color4B COLOR4B_RED    (0xFF, 0x00, 0x00, 0xFF);
    static const Color4B COLOR4B_GREEN  (0x00, 0xFF, 0x00, 0xFF);
    static const Color4B COLOR4B_BLUE   (0x00, 0x00, 0xFF, 0xFF);
    static const Color4B COLOR4B_YELLOW (0xFF, 0xFF, 0x00, 0xFF);
    static const Color4B COLOR4B_MAGENTA(0xFF, 0x00, 0xFF, 0xFF);
    static const Color4B COLOR4B_ORANGE (0xFF, 0x9B, 0x00, 0xFF);
    static const Color4B COLOR4B_GRAY   (0xA6, 0xA6, 0xA6, 0xFF);
    static const Color4B COLOR4B_GREY   (0xA6, 0xA6, 0xA6, 0xFF);
    
    static const Color3B COLOR3B_WHITE  (0xFF, 0xFF, 0xFF);
    static const Color3B COLOR3B_BLACK  (0x00, 0x00, 0x00);
    static const Color3B COLOR3B_RED    (0xFF, 0x00, 0x00);
    static const Color3B COLOR3B_GREEN  (0x00, 0xFF, 0x00);
    static const Color3B COLOR3B_BLUE   (0x00, 0x00, 0xFF);
    static const Color3B COLOR3B_YELLOW (0xFF, 0xFF, 0x00);
    static const Color3B COLOR3B_MAGENTA(0xFF, 0x00, 0xFF);
    static const Color3B COLOR3B_ORANGE (0xFF, 0x9B, 0x00);
    static const Color3B COLOR3B_GRAY   (0xA6, 0xA6, 0xA6);
    static const Color3B COLOR3B_GREY   (0xA6, 0xA6, 0xA6);
}