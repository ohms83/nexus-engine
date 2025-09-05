//
// Created by nutta on 9/5/2025.
//
#pragma once

#include "nexus/NxsDefine.h"
#include "Runnable.h"

NXS_NAMESPACE
{
    class LambdaTask final : public IRunnable
    {
    public:
        // ReSharper disable once CppPassValueParameterByConstReference
        explicit LambdaTask(std::function<bool()> task)
            : m_task(task)
        {}

        bool Update() override
        {
            return m_task();
        }

    private:
        std::function<bool()> m_task;
    };
}
