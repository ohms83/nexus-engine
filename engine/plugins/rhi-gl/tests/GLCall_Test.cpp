#include "nexus/NxsGL.h"

#include <gtest/gtest.h>
#include <iostream>

namespace
{
    GLenum nextError = GL_NO_ERROR;
    unsigned errorChecks = 0;

    GLenum APIENTRY GetTestError()
    {
        ++errorChecks;
        return nextError;
    }

    class GLCallTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            previousGetError = glad_glGetError;
            glad_glGetError = GetTestError;
            nextError = GL_NO_ERROR;
            errorChecks = 0;
        }

        void TearDown() override
        {
            glad_glGetError = previousGetError;
            nxs::Assert::onAssertionFailedEvent.disconnect_all();
        }

        PFNGLGETERRORPROC previousGetError = nullptr;
    };
}

TEST_F(GLCallTest, SuccessfulChecksDoNotEvaluateMessage)
{
    int evaluations = 0;
    CHECK_GL_ERROR(TestCall);
    CHECK_GL_ERROR_MSG(TestCall, (++evaluations, "details"));
    EXPECT_EQ(errorChecks, 2u);
    EXPECT_EQ(evaluations, 0);
}

TEST(TextureInternalFormatTest, SRgbRgbUsesSrgbInternalFormat)
{
    EXPECT_EQ(
        nxs::GL::ResolveTextureInternalFormat(
            nxs::PixelFormat::RGB,
            nxs::TextureColorSpace::SRgb,
            GL_RGB),
        GL_SRGB8);
}

TEST(TextureInternalFormatTest, SRgbRgbaUsesSrgbAlphaInternalFormat)
{
    EXPECT_EQ(
        nxs::GL::ResolveTextureInternalFormat(
            nxs::PixelFormat::RGBA,
            nxs::TextureColorSpace::SRgb,
            GL_RGBA),
        GL_SRGB8_ALPHA8);
}

TEST(TextureInternalFormatTest, LinearRgbUsesLinearInternalFormat)
{
    EXPECT_EQ(
        nxs::GL::ResolveTextureInternalFormat(
            nxs::PixelFormat::RGB,
            nxs::TextureColorSpace::Linear,
            GL_RGB),
        GL_RGB);
}

TEST(TextureInternalFormatTest, UnsupportedSrgbFormatUsesFallback)
{
    EXPECT_EQ(
        nxs::GL::ResolveTextureInternalFormat(
            nxs::PixelFormat::Depth,
            nxs::TextureColorSpace::SRgb,
            GL_DEPTH_COMPONENT),
        GL_DEPTH_COMPONENT);
}

#if !defined(NXS_NO_LOG)
TEST_F(GLCallTest, FailedCheckReportsFunctionAndError)
{
    EXPECT_DEATH({
        glad_glGetError = GetTestError;
        nextError = GL_INVALID_ENUM;
        nxs::Assert::onAssertionFailedEvent.connect([](const std::string& message) {
            std::cerr << message << std::endl;
        });
        CHECK_GL_ERROR(TestCall);
    }, "Error calling function TestCall.*Error Code=500");
}

TEST_F(GLCallTest, FailedCheckReportsCustomMessage)
{
    EXPECT_DEATH({
        glad_glGetError = GetTestError;
        nextError = GL_INVALID_OPERATION;
        nxs::Assert::onAssertionFailedEvent.connect([](const std::string& message) {
            std::cerr << message << std::endl;
        });
        CHECK_GL_ERROR_MSG(TestCall, "details");
    }, "Error calling function TestCall.*Error Code=502 Message=details");
}
#endif
