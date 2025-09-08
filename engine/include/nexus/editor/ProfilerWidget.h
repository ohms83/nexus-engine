//
// Created by nutta on 7/17/2025.
//

#pragma once

#include <vector>
#include <nexus/NxsDefine.h>

#include "EditorWidget.h"

NXS_NAMESPACE
{
    class ProfilerWidget final : public EditorWidget
    {
    public:
        ProfilerWidget();

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;

    private:
        std::vector<uint32> m_frameTimes;
        std::vector<uint32> m_frameCounters;
    };
}