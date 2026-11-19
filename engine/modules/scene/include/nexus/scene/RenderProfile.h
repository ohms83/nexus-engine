#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Color.h"
#include "nexus/graphics/GraphicsConst.h"
#include "nexus/reflection/Reflection.h"

NXS_NAMESPACE
{
    enum class CullingFlags : uint32_t
    {
        None    = 0x00,
        Opaque  = 0x01,
        Alpha   = 0x02,
        Overlay = 0x04,
        All     = 0xFFFFFFFF,
    };

    struct RenderProfile : public IReflection
    {
        IMPLEMENT_REFLECTION_STRUCT(RenderProfile);

        VariantData Serialize() const override
        {
            VariantData data(VariantData::Map{});
            data["clearFlags"] = static_cast<int64_t>(static_cast<uint32_t>(clearFlags));
            data["clearColor"] = clearColor;
            data["clearDepth"] = clearDepth;
            data["clearStencil"] = static_cast<int64_t>(clearStencil);
            data["depthFill"] = depthFill;
            data["culling"] = static_cast<int64_t>(static_cast<uint32_t>(culling));
            return data;
        }

        bool Deserialize(const VariantData& data) override
        {
            if (!data.IsMap()
                || !data.HasKey("clearFlags")
                || !data.HasKey("clearColor")
                || !data.HasKey("clearDepth")
                || !data.HasKey("clearStencil")
                || !data.HasKey("depthFill")
                || !data.HasKey("culling"))
            {
                return false;
            }

            clearFlags = CAST<ClearFlags>(static_cast<uint32_t>(data["clearFlags"].GetInt()));
            clearColor = CAST<Color3F>(data["clearColor"].GetVec3());
            clearDepth = static_cast<float>(data["clearDepth"].GetFloat());
            clearStencil = static_cast<uint32_t>(data["clearStencil"].GetInt());
            depthFill = data["depthFill"].GetBool();
            culling = CAST<CullingFlags>(static_cast<uint32_t>(data["culling"].GetInt()));
            return true;
        }

        //!
        ClearFlags clearFlags = ClearFlags::All;
        Color3F clearColor { 0x303030FF };
        float clearDepth = 1.0f;
        uint32_t clearStencil = 0;
        bool depthFill = false;
        CullingFlags culling = CullingFlags::None;

        void AcceptReflector(IReflector& r) override
        {
            r.SetMarker("Render Profile");
            r.VisitColor3("Clear Color", clearColor);
            r.VisitFloat("Clear Depth", clearDepth);
            r.VisitInt("Clear Stencil", R_CAST<int32_t&>(clearStencil));
            r.VisitBool("Depth Fill", depthFill);
            r.VisitBitfield("Culling", culling, {
                {"Opaque", static_cast<uint32_t>(CullingFlags::Opaque)},
                {"Alpha", static_cast<uint32_t>(CullingFlags::Alpha)},
                {"Overlay", static_cast<uint32_t>(CullingFlags::Overlay)}
            });
        }
    };
}