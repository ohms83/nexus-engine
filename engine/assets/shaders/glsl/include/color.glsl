/**
 * @brief Encodes linear RGB values using the standard sRGB transfer function.
 *
 * Use this for final shader output when lighting calculations are performed
 * in linear space and the framebuffer is not doing sRGB encoding. Negative
 * input components are clamped to zero; values above one are preserved.
 * The constants are from the standard sRGB transfer function: 12.92 is the
 * slope of its linear segment near black, while 1.055, 1/2.4, and -0.055
 * define the power curve. The 0.0031308 threshold joins the two segments.
 *
 * @param linearColor RGB color in linear-light space.
 * @return The sRGB-encoded RGB color.
 */
vec3 LinearToSRGB(vec3 linearColor)
{
    vec3 nonNegativeColor = max(linearColor, vec3(0.0));
    vec3 linearSegment = 12.92 * nonNegativeColor;
    vec3 srgbSegment = 1.055 * pow(nonNegativeColor, vec3(1.0 / 2.4)) - 0.055;
    return mix(srgbSegment, linearSegment, lessThanEqual(nonNegativeColor, vec3(0.0031308)));
}
