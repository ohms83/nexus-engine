// Lightweight per-frame graphics profiler for draw-call composition analysis
#include "nexus/graphics/GraphicsProfiler.h"
#include "nexus/debug/LogDispatcher.h"

#include <unordered_set>
#include <unordered_map>
#include <atomic>
#include <format>
#include <vector>
#include <algorithm>
#include <string>

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(GraphicsProfiler);

NXS_NAMESPACE
{
    namespace GraphicsProfiler
    {
    struct FrameData
    {
        std::unordered_set<uint32_t> programs;
        std::unordered_set<uint32_t> textures;
        std::unordered_map<uint32_t, uint32_t> programDraws; // programHandle -> drawCount
        std::unordered_map<uint32_t, uint32_t> materialDraws; // materialId -> drawCount
        uint32_t drawCalls = 0;
        uint64_t indices = 0;
        std::atomic<uint32_t> programBindEvents{0};
        std::atomic<uint32_t> textureBindEvents{0};
    };

    static thread_local FrameData s_frame;
    static thread_local bool s_isEnabled = false;

    void Enable(bool enable)
    {
        s_isEnabled = enable;
    }

    void BeginFrame()
    {
        rmt_ScopedCPUSample(GraphicsProfiler_BeginFrame, 0);
        if (!s_isEnabled) return;
        s_frame.programs.clear();
        s_frame.textures.clear();
        s_frame.programDraws.clear();
        s_frame.materialDraws.clear();
        s_frame.drawCalls = 0;
        s_frame.indices = 0;
        s_frame.programBindEvents.store(0);
        s_frame.textureBindEvents.store(0);
    }

    void EndFrameAndReport()
    {
        rmt_ScopedCPUSample(GraphicsProfiler_EndFrameAndReport, 0);
        if (!s_isEnabled) return;

        // Create top-N lists for programs and materials
        std::vector<std::pair<uint32_t,uint32_t>> progList;
        progList.reserve(s_frame.programDraws.size());
        for (auto &p : s_frame.programDraws) progList.emplace_back(p.first, p.second);
        std::sort(progList.begin(), progList.end(), [](auto &a, auto &b){ return a.second > b.second; });

        std::vector<std::pair<uint32_t,uint32_t>> matList;
        matList.reserve(s_frame.materialDraws.size());
        for (auto &m : s_frame.materialDraws) matList.emplace_back(m.first, m.second);
        std::sort(matList.begin(), matList.end(), [](auto &a, auto &b){ return a.second > b.second; });

        std::string topProg = "";
        for (size_t i=0;i<std::min<size_t>(5, progList.size());++i)
        {
            topProg += std::format("[{}]={}, ", progList[i].first, progList[i].second);
        }

        std::string topMat = "";
        for (size_t i=0;i<std::min<size_t>(5, matList.size());++i)
        {
            topMat += std::format("[{}]={}, ", matList[i].first, matList[i].second);
        }

        LOG_INFO(LogGraphicsProfiler, std::format("Frame Composition: drawCalls={}, uniquePrograms={}, uniqueTextures={}, indices={}, programBindEvents={}, textureBindEvents={}\nTopPrograms: {}\nTopMaterials: {}",
            s_frame.drawCalls,
            s_frame.programs.size(),
            s_frame.textures.size(),
            s_frame.indices,
            s_frame.programBindEvents.load(),
            s_frame.textureBindEvents.load(),
            topProg,
            topMat));
    }

    void RecordProgramBind(uint32_t programHandle)
    {
        rmt_ScopedCPUSample(GraphicsProfiler_RecordProgramBind, 0);
        if (!s_isEnabled || programHandle == 0) return;
        s_frame.programs.insert(programHandle);
        s_frame.programBindEvents.fetch_add(1);
    }

    void RecordProgramDraw(uint32_t programHandle, uint32_t indexCount, uint32_t instanceCount)
    {
        rmt_ScopedCPUSample(GraphicsProfiler_RecordProgramDraw, 0);
        if (!s_isEnabled || programHandle == 0) return;
        s_frame.programDraws[programHandle] += 1;
        s_frame.drawCalls++;
        s_frame.indices += static_cast<uint64_t>(indexCount) * static_cast<uint64_t>(instanceCount);
    }

    void RecordTextureBind(uint32_t textureHandle)
    {
        rmt_ScopedCPUSample(GraphicsProfiler_RecordTextureBind, 0);
        if (!s_isEnabled || textureHandle == 0) return;
        s_frame.textures.insert(textureHandle);
        s_frame.textureBindEvents.fetch_add(1);
    }

    void RecordMaterialDraw(uint32_t materialId, uint32_t indexCount, uint32_t instanceCount)
    {
        rmt_ScopedCPUSample(GraphicsProfiler_RecordMaterialDraw, 0);
        if (!s_isEnabled || materialId == 0) return;
        s_frame.materialDraws[materialId] += 1;
        s_frame.drawCalls++;
        s_frame.indices += static_cast<uint64_t>(indexCount) * static_cast<uint64_t>(instanceCount);
    }

    void RecordDrawCall(uint32_t indexCount, uint32_t instanceCount)
    {
        rmt_ScopedCPUSample(GraphicsProfiler_RecordDrawCall, 0);
        if (!s_isEnabled) return;
        s_frame.drawCalls++;
        s_frame.indices += static_cast<uint64_t>(indexCount) * static_cast<uint64_t>(instanceCount);
    }
    }
}
