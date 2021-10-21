#ifndef __NXS_COLOR_H__
#define __NXS_COLOR_H__

#include "NxsMacros.h"

#include <cstdint>

NXS_NAMESPACE {
    struct Color;
    struct Color3;
    struct Color4B;
    struct Color3B;
    
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
     * Color data in 3 floating points RGB format.
     * Each component can have value between [0, 1.0] for the standard dynamic range.
     */
    struct Color3
    {
        /**
         * Default constructor. All components will be set to 0.
         */
        Color3();
        /**
         * Construct a @c Color object from the specified red, green, and blue components.
         * The alpha component will be assumed 0xFF.
         */
        Color3(float red, float green, float blue);
        /// Convert from @c Color3B
        Color3(const Color3B& rhs);
        
        float r, g, b;
        
        static const Color3 WHITE;
        static const Color3 BLACK;
        static const Color3 RED;
        static const Color3 GREEN;
        static const Color3 BLUE;
        static const Color3 YELLOW;
        static const Color3 MAGENTA;
        static const Color3 ORANGE;
        static const Color3 GRAY;
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
    
    /**
     * Color data in 3 bytes RGB format. Each component can have value between [0, 255]
     * This struct is more memory efficient than @c Color3 and it's a preferable choice to pass
     * around between functions when the high precision is not the main concern.
     */
    struct Color3B
    {
        /**
         * Default constructor. All components will be set to 0.
         */
        Color3B();
        /**
         * Construct a @c Color4B object from the specified red, green, and blue components.
         * The alpha component will be assumed 0xFF.
         */
        Color3B(float red, float green, float blue);
        /**
         * Convert from @c Color3
         * @warning There input value won't be clamped during convertion and any components that have
         *          their value higher than 1.0f will result in overflow.
         */
        Color3B(const Color3& rhs);
        
        uint8_t r, g, b;
        
        static const Color3B WHITE;
        static const Color3B BLACK;
        static const Color3B RED;
        static const Color3B GREEN;
        static const Color3B BLUE;
        static const Color3B YELLOW;
        static const Color3B MAGENTA;
        static const Color3B ORANGE;
        static const Color3B GRAY;
    };
}

#endif // __NXS_COLOR_H__
