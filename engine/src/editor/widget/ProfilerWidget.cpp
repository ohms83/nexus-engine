//
// Created by nutta on 7/17/2025.
//

#include "editor/widget/ProfilerWidget.h"

#include "imgui.h"
#include "implot.h"
#include "core/Logger.h"
#include "nexus/graphics/GraphicsConst.h"
#include "nexus/graphics/RenderSystem.h"

#define MAX_PROFILED_FRAMES 20

USING_NAMESPACE_NXS;

ProfilerWidget::ProfilerWidget()
    : EditorWidget("ProfilerWidget")
{
    m_frameTimes.reserve(MAX_PROFILED_FRAMES);
    m_frameCounters.reserve(MAX_PROFILED_FRAMES);
}

void ProfilerWidget::Draw_Internal(RenderSystem& renderSystem)
{
    const ImGuiIO& io = ImGui::GetIO(); (void)io;

    const uint32_t frameIndex = renderSystem.GetFrameIndex();
    const auto frameTime = 1000.0f / io.Framerate; // Frame update time in seconds.
    const auto& api = GraphicsAPIToString(renderSystem.GetGraphicsConfig().api);
    ImGui::Text("Graphics API: %s", api.c_str());
    ImGui::Text("FPS: %.2f", io.Framerate);
    ImGui::Text("Frame Update: %.2f ms", frameTime);
    ImGui::Text("Render: %.2f ms", renderSystem.GetrenderTime());
    ImGui::Text("Draw Calls: %d", renderSystem.GetDrawCount());
    ImGui::Text("Polygons: %d", renderSystem.GetPolygonCount());

    if (ImPlot::BeginPlot("Frame Rates"))
    {
        ImPlot::SetupAxes("Frame Index", "Milliseconds", ImPlotAxisFlags_NoTickLabels);
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::SetupAxisLimits(ImAxis_X1,frameIndex > MAX_PROFILED_FRAMES ? frameIndex - MAX_PROFILED_FRAMES + 1 : 0, frameIndex, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1,0,60);
        ImPlot::SetNextFillStyle(ImVec4(1,1,0,1),0.25f);
        m_frameCounters.push_back(frameIndex);
        m_frameTimes.push_back(CAST<uint32_t>(frameTime));

        if (m_frameCounters.size() > MAX_PROFILED_FRAMES) m_frameCounters.erase(m_frameCounters.begin());
        if (m_frameTimes.size() > MAX_PROFILED_FRAMES) m_frameTimes.erase(m_frameTimes.begin());

        const int32_t dataCount = m_frameTimes.size() < MAX_PROFILED_FRAMES ? m_frameTimes.size() : MAX_PROFILED_FRAMES;
        ImPlot::PlotShaded("Frame", m_frameCounters.data(), m_frameTimes.data(), dataCount);
        ImPlot::EndPlot();
    }
}
