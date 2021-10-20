#ifndef __NXS_COLOR_H__
#define __NXS_COLOR_H__

#include "NxsMacros.h"

#include <cstdint>

NXS_NAMESPACE {
    struct Color;
    struct Color4B;
    
    /**
     * Color data in 4 floating points RGBA format.
     * Each component can have value between [0, 1.0] for the standard dynamic range.
     */
    struct Color
    {
        /**
         * Default constructor. All components will be set to 0.
         */
        Color();
        /**
         * Construct a @c Color object from the specified red, green, and blue components.
         * The alpha component will be assumed 0xFF.
         */
        Color(float red, float green, float blue);
        /**
         * Construct a @c Color object from the specified red, green, blue, and alpha components.
         */
        Color(float red, float green, float blue, float alpha);
        /// Convert from @c Color4B
        Color(const Color4B& rhs);

        float r, g, b, a;

        static const Color WHITE;
        static const Color BLACK;
        static const Color RED;
        static const Color GREEN;
        static const Color BLUE;
        static const Color YELLOW;
        static const Color MAGENTA;
        static const Color ORANGE;
        static const Color GRAY;
    };

    /**
     * Color data in 4 bytes RGBA format. Each component can have value between [0, 255]
     * This struct is more memory efficient than @c Color and it's a preferable choice to pass
     * around between functions when the high precision is not the main concern.
     */
    struct Color4B
    {
        /**
         * Default constructor. All components will be set to 0.
         */
        Color4B();
        /**
         * Construct a @c Color4B object from the specified red, green, and blue components.
         * The alpha component will be assumed 0xFF.
         */
        Color4B(float red, float green, float blue);
        /**
         * Construct a @c Color4B object from the specified red, green, blue, and alpha components.
         */
        Color4B(float red, float green, float blue, float alpha);
        /// Copy constructor
        Color4B(const Color4B& rhs);
        /**
         * Convert from @c Color
         * @warning There input value won't be clamped during convertion and any components that have
         *          their value higher than 1.0f will result in overflow.
         */
        Color4B(const Color& rhs);

        uint8_t r, g, b, a;

        static const Color4B WHITE;
        static const Color4B BLACK;
        static const Color4B RED;
        static const Color4B GREEN;
        static const Color4B BLUE;
        static const Color4B YELLOW;
        static const Color4B MAGENTA;
        static const Color4B ORANGE;
        static const Color4B GRAY;
    };
}

#endif // __NXS_COLOR_H__