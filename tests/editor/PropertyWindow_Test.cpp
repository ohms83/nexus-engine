#include "gtest/gtest.h"
#include "nexus/editor/widget/PropertyWindow.h"
#include "nexus/scene/SceneManager.h"

#include "imgui.h"

namespace
{
    class BitfieldWidgetTest : public testing::Test
    {
    protected:
        void SetUp() override
        {
            ImGui::CreateContext();
            auto& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.DisplaySize = ImVec2(640.0f, 480.0f);
            io.DeltaTime = 1.0f / 60.0f;
            unsigned char* pixels = nullptr;
            int width = 0;
            int height = 0;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        }

        void TearDown() override { ImGui::DestroyContext(); }

        bool Draw(nxs::PropertyWindow& window, uint32_t& value, ImVec2* itemCenter = nullptr)
        {
            ImGui::NewFrame();
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(400, 300));
            ImGui::Begin("Test");
            const bool changed = window.VisitBitfield("Flags", value, {
                {"Duplicate", 0x1u},
                {"Duplicate", 0x80000000u}
            });
            if (itemCenter)
            {
                const auto min = ImGui::GetItemRectMin();
                const auto max = ImGui::GetItemRectMax();
                *itemCenter = ImVec2((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);
            }
            ImGui::End();
            ImGui::Render();
            return changed;
        }
    };
}

TEST_F(BitfieldWidgetTest, TogglingHighBitPreservesUnlistedFlagsAndNotifiesOnce)
{
    nxs::SceneManager manager(nullptr, nullptr);
    nxs::PropertyWindow window(manager);
    uint32_t value = 0x00000041u;
    int notifications = 0;
    auto connection = window.onValueChangedEvent.connect([&notifications](const std::string& name) {
        EXPECT_EQ(name, "Flags");
        ++notifications;
    });

    ImVec2 center;
    EXPECT_FALSE(Draw(window, value, &center));
    ImGui::GetIO().AddMousePosEvent(center.x, center.y);
    ImGui::GetIO().AddMouseButtonEvent(0, true);
    EXPECT_FALSE(Draw(window, value));
    ImGui::GetIO().AddMouseButtonEvent(0, false);
    EXPECT_TRUE(Draw(window, value));
    EXPECT_EQ(value, 0x80000041u);
    EXPECT_EQ(notifications, 1);

    ImGui::GetIO().AddMouseButtonEvent(0, true);
    EXPECT_FALSE(Draw(window, value));
    ImGui::GetIO().AddMouseButtonEvent(0, false);
    EXPECT_TRUE(Draw(window, value));
    EXPECT_EQ(value, 0x00000041u);
    EXPECT_EQ(notifications, 2);
    window.onValueChangedEvent.disconnect(connection);
}
