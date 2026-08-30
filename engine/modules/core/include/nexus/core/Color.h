#pragma once

#include <nexus/NxsDefine.h>
#include <glm/glm.hpp>

NXS_NAMESPACE
{
    class Color4F;
    class Color3F;
    class Color4B;
    class Color3B;

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
        explicit Color4F(const uint32_t color)
        {
            this->r = CAST<float>(color >> 24 & 0xFF)  / 255.0f;
            this->g = CAST<float>(color >> 16 & 0xFF)  / 255.0f;
            this->b = CAST<float>(color >>  8 & 0xFF)  / 255.0f;
            this->a = CAST<float>(color       & 0xFF)  / 255.0f;
        }
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

        NODISCARD uint32_t ToRGBA() const
        {
            const auto _r = CAST<uint8_t>(this->r * 255);
            const auto _g = CAST<uint8_t>(this->g * 255);
            const auto _b = CAST<uint8_t>(this->b * 255);
            const auto _a = CAST<uint8_t>(this->a * 255);
            return _a | _b << 8 | _g << 16 | _r << 24;
        }

        explicit operator uint32_t() const
        {
            return ToRGBA();
        }

        Color4F& operator = (const uint32_t color)
        {
            this->r = CAST<float>(color >> 24 & 0xFF)  / 255.0f;
            this->g = CAST<float>(color >> 16 & 0xFF)  / 255.0f;
            this->b = CAST<float>(color >>  8 & 0xFF)  / 255.0f;
            this->a = CAST<float>(color       & 0xFF)  / 255.0f;
            return *this;
        }

        static const Color4F White;
        static const Color4F Black;
        static const Color4F Red;
        static const Color4F Green;
        static const Color4F Blue;
        static const Color4F Yellow;
        static const Color4F Magenta;
        static const Color4F Orange;
        static const Color4F Gray;
        static const Color4F Grey;
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
        explicit Color3F(const uint32_t color)
        {
            this->r = CAST<float>(color >> 24 & 0xFF)  / 255.0f;
            this->g = CAST<float>(color >> 16 & 0xFF)  / 255.0f;
            this->b = CAST<float>(color >>  8 & 0xFF)  / 255.0f;
        }
        explicit Color3F(const glm::vec<3, float>& color)
            : Color3(color)
        {}
        explicit Color3F(glm::vec<3, float>&& color)
            : Color3(color)
        {}

        NODISCARD uint32_t ToRGB() const
        {
            const auto _r = CAST<uint8_t>(this->r * 255);
            const auto _g = CAST<uint8_t>(this->g * 255);
            const auto _b = CAST<uint8_t>(this->b * 255);
            return _b | _g << 8 | _r << 16;
        }

        explicit operator uint32_t() const
        {
            return ToRGB();
        }

        explicit operator Color4F() const
        {
            return Color4F(this->r, this->g, this->b, 1.0f);
        }

        explicit operator glm::vec4() const
        {
            return glm::vec4(this->r, this->g, this->b, 1.0f);
        }

        explicit operator Color4B() const;
        explicit operator Color3B() const;

        Color3F& operator = (const uint32_t color)
        {
            this->r = CAST<float>(color >> 24 & 0xFF)  / 255.0f;
            this->g = CAST<float>(color >> 16 & 0xFF)  / 255.0f;
            this->b = CAST<float>(color >>  8 & 0xFF)  / 255.0f;
            return *this;
        }

        static const Color3F White;
        static const Color3F Black;
        static const Color3F Red;
        static const Color3F Green;
        static const Color3F Blue;
        static const Color3F Yellow;
        static const Color3F Magenta;
        static const Color3F Orange;
        static const Color3F Gray;
        static const Color3F Grey;
    };
    /**
     * Color data in 4 unsigned byte RGBA formats.
     * Each component can have a value between [0x00, 0xFF] for the standard dynamic range.
     */
    class  Color4B : public Color4<uint8_t>
    {
    public:
        Color4B() = default;
        explicit Color4B(const uint32_t color)
        {
            this->r = CAST<uint8_t>(color >> 24 & 0xFF);
            this->g = CAST<uint8_t>(color >> 16 & 0xFF);
            this->b = CAST<uint8_t>(color >>  8 & 0xFF);
            this->a = CAST<uint8_t>(color       & 0xFF);
        }
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

        NODISCARD uint32_t ToRGBA() const
        {
            return this->a | this->b << 8 | this->g << 16 | this->r << 24;
        }

        explicit operator uint32_t() const
        {
            return ToRGBA();
        }

        explicit operator glm::vec3() const
        {
            return glm::vec3(this->r, this->g, this->b);
        }

        explicit operator glm::vec4() const
        {
            return glm::vec4(this->r, this->g, this->b, 1.0f);
        }

        explicit operator Color4F() const
        {
            return Color4F(this->r / 255.0f, this->g / 255.0f, this->b / 255.0f, this->a / 255.0f);
        }

        explicit operator Color3F() const
        {
            return Color3F(this->r / 255.0f, this->g / 255.0f, this->b / 255.0f);
        }

        Color4B& operator = (const uint32_t color)
        {
            this->r = CAST<uint8_t>(color >> 24 & 0xFF);
            this->g = CAST<uint8_t>(color >> 16 & 0xFF);
            this->b = CAST<uint8_t>(color >>  8 & 0xFF);
            this->a = CAST<uint8_t>(color       & 0xFF);
            return *this;
        }

        static const Color4B White;
        static const Color4B Black;
        static const Color4B Red;
        static const Color4B Green;
        static const Color4B Blue;
        static const Color4B Yellow;
        static const Color4B Magenta;
        static const Color4B Orange;
        static const Color4B Gray;
        static const Color4B Grey;
    };
    /**
     * Color data in 3 unsigned byte RGB formats.
     * Each component can have a value between [0x00, 0xFF] for the standard dynamic range.
     */
    class  Color3B : public Color3<uint8_t>
    {
    public:
        Color3B() = default;
        explicit Color3B(const uint32_t color)
        {
            this->r = CAST<uint8_t>(color >> 16 & 0xFF);
            this->g = CAST<uint8_t>(color >>  8 & 0xFF);
            this->b = CAST<uint8_t>(color       & 0xFF);
        }
        Color3B(const uint8_t _r, const uint8_t _g, const uint8_t _b)
            : Color3(_r, _g, _b)
        {}
        explicit Color3B(const glm::vec<3, uint8_t>& color)
            : Color3(color)
        {}
        explicit Color3B(glm::vec<3, uint8_t>&& color)
            : Color3(color)
        {}

        NODISCARD uint32_t ToRGB() const
        {
            return this->b | this->g << 8 | this->r << 16;
        }

        explicit operator uint32_t() const
        {
            return ToRGB();
        }

        Color3B& operator = (const uint32_t color)
        {
            this->r = CAST<uint8_t>(color >> 16 & 0xFF);
            this->g = CAST<uint8_t>(color >>  8 & 0xFF);
            this->b = CAST<uint8_t>(color       & 0xFF);
            return *this;
        }

        static const Color3B White;
        static const Color3B Black;
        static const Color3B Red;
        static const Color3B Green;
        static const Color3B Blue;
        static const Color3B Yellow;
        static const Color3B Magenta;
        static const Color3B Orange;
        static const Color3B Gray;
        static const Color3B Grey;
    };
}