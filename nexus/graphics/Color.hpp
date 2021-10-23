#ifndef __NXS_COLOR_H__
#define __NXS_COLOR_H__

#include "NxsDefine.h"

#include "glm/glm.hpp"
#include <cstdint>

NXS_NAMESPACE {
    /**
     * Color data in 4 floating points RGBA format.
     * Each component can have value between [0, 1.0] for the standard dynamic range.
     */
    typedef glm::vec<4, float> Color4F;
    /**
     * Color data in 3 floating points RGB format.
     * Each component can have value between [0, 1.0] for the standard dynamic range.
     */
    typedef glm::vec<3, float> Color3F;
    /**
     * Color data in 4 unsigned byte RGBA format.
     * Each component can have value between [0x00, 0xFF] for the standard dynamic range.
     */
    typedef glm::vec<4, float> Color4B;
    /**
     * Color data in 3 unsigned byte RGB format.
     * Each component can have value between [0x00, 0xFF] for the standard dynamic range.
     */
    typedef glm::vec<3, float> Color3B;
    
    static const Color4F COLOR4F_WHITE  (1.00f, 1.00f, 1.00f, 1.00f);
    static const Color4F COLOR4F_BLACK  (0.00f, 0.00f, 0.00f, 1.00f);
    static const Color4F COLOR4F_RED    (1.00f, 0.00f, 0.00f, 1.00f);
    static const Color4F COLOR4F_GREEN  (0.00f, 1.00f, 0.00f, 1.00f);
    static const Color4F COLOR4F_BLUE   (0.00f, 0.00f, 1.00f, 1.00f);
    static const Color4F COLOR4F_YELLOW (1.00f, 1.00f, 0.00f, 1.00f);
    static const Color4F COLOR4F_MAGENTA(1.00f, 0.00f, 1.00f, 1.00f);
    static const Color4F COLOR4F_ORANGE (1.00f, 0.50f, 0.00f, 1.00f);
    static const Color4F COLOR4F_GRAY   (0.65f, 0.65f, 0.65f, 1.00f);
    
    static const Color3F COLOR3F_WHITE  (1.00f, 1.00f, 1.00f);
    static const Color3F COLOR3F_BLACK  (0.00f, 0.00f, 0.00f);
    static const Color3F COLOR3F_RED    (1.00f, 0.00f, 0.00f);
    static const Color3F COLOR3F_GREEN  (0.00f, 1.00f, 0.00f);
    static const Color3F COLOR3F_BLUE   (0.00f, 0.00f, 1.00f);
    static const Color3F COLOR3F_YELLOW (1.00f, 1.00f, 0.00f);
    static const Color3F COLOR3F_MAGENTA(1.00f, 0.00f, 1.00f);
    static const Color3F COLOR3F_ORANGE (1.00f, 0.50f, 0.00f);
    static const Color3F COLOR3F_GRAY   (0.65f, 0.65f, 0.65f);
    
    static const Color4B COLOR4B_WHITE  (0xFF, 0xFF, 0xFF, 0xFF);
    static const Color4B COLOR4B_BLACK  (0x00, 0x00, 0x00, 0xFF);
    static const Color4B COLOR4B_RED    (0xFF, 0x00, 0x00, 0xFF);
    static const Color4B COLOR4B_GREEN  (0x00, 0xFF, 0x00, 0xFF);
    static const Color4B COLOR4B_BLUE   (0x00, 0x00, 0xFF, 0xFF);
    static const Color4B COLOR4B_YELLOW (0xFF, 0xFF, 0x00, 0xFF);
    static const Color4B COLOR4B_MAGENTA(0xFF, 0x00, 0xFF, 0xFF);
    static const Color4B COLOR4B_ORANGE (0xFF, 0x9B, 0x00, 0xFF);
    static const Color4B COLOR4B_GRAY   (0xA6, 0xA6, 0xA6, 0xFF);
    
    static const Color3B COLOR3B_WHITE  (0xFF, 0xFF, 0xFF);
    static const Color3B COLOR3B_BLACK  (0x00, 0x00, 0x00);
    static const Color3B COLOR3B_RED    (0xFF, 0x00, 0x00);
    static const Color3B COLOR3B_GREEN  (0x00, 0xFF, 0x00);
    static const Color3B COLOR3B_BLUE   (0x00, 0x00, 0xFF);
    static const Color3B COLOR3B_YELLOW (0xFF, 0xFF, 0x00);
    static const Color3B COLOR3B_MAGENTA(0xFF, 0x00, 0xFF);
    static const Color3B COLOR3B_ORANGE (0xFF, 0x9B, 0x00);
    static const Color3B COLOR3B_GRAY   (0xA6, 0xA6, 0xA6);
}

#endif // __NXS_COLOR_H__
