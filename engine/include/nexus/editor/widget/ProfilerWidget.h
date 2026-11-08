//
// Created by nutta on 7/17/2025.
//

#pragma once

#include "nexus/container/Array.h"

#include "nexus/NxsDefine.h"
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
        Array<uint32_t> m_frameTimes;
        Array<uint32_t> m_frameCounters;
    };
}