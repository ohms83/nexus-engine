//
// Created by nutta on 7/30/2025.
//

#pragma once

#include "nexus/NxsDefine.h"

NXS_NAMESPACE
{
    class ITimeSource
    {
    public:
        virtual ~ITimeSource() = default;

        /**
         * Get the current value of the timer counter in second.
         * @note The counter only meaningful relative to each others and might not be meaningful using as-is.
         */
        virtual double Now() = 0;
        /**
         * Get the current value of the timer counter in millisecond.
        * @note The counter only meaningful relative to each others and might not be meaningful using as-is.
         */
        virtual double NowMs() = 0;
    };
}
