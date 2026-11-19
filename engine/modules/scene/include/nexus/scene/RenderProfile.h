#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Color.h"
#include "nexus/graphics/GraphicsConst.h"
#include "nexus/reflection/Reflection.h"

NXS_NAMESPACE
{
    /**
     * @brief Bitmask of render queues excluded from a view.
     *
     * These flags control render pass selection, not geometric frustum culling.
     * Multiple flags may be combined; excluding a queue does not affect the
     * optional depth-fill pass.
     */
    enum class CullingFlags : uint32_t
    {
        None    = 0x00,       //!< Do not exclude any queue.
        Opaque  = 0x01,       //!< Exclude the opaque queue.
        Alpha   = 0x02,       //!< Exclude the alpha queue.
        Overlay = 0x04,       //!< Exclude the overlay queue.
        All     = 0xFFFFFFFF, //!< Set all bits, including currently undefined bits.
    };

    /**
     * @brief Per-view clear settings and render queue selection.
     *
     * A camera stores a profile which is copied into its ViewContext. The
     * renderer uses that snapshot to clear the view and select registered,
     * enabled passes. Profiles support reflection and serialization.
     *
     * By default, clearing is enabled for all clear flags, depth fill is
     * disabled, and no render queues are excluded.
     */
    struct RenderProfile : public IReflection
    {
        IMPLEMENT_REFLECTION_STRUCT(RenderProfile);

        /**
         * @brief Serialize all profile settings to a VariantData map.
         * @return A map containing clearFlags, clearColor, clearDepth,
         * clearStencil, depthFill, and culling.
         */
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

        /**
         * @brief Restore profile settings from a serialized map.
         * @return False if the value is not a map or a required key is missing;
         * otherwise, true.
         */
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

        //! Buffers requested for clearing before the view's passes.
        ClearFlags clearFlags = ClearFlags::All;
        //! Color used when clearing the color buffer.
        Color3F clearColor { 0x303030FF };
        //! Value used when clearing the depth buffer.
        float clearDepth = 1.0f;
        //! Stored stencil clear value; generic view clearing does not yet apply it.
        uint32_t clearStencil = 0;
        //! Select the depth-fill pass when it is registered and enabled.
        bool depthFill = false;
        //! Render queues to exclude from this view.
        CullingFlags culling = CullingFlags::None;

        //! Expose editable clear, depth-fill, and culling settings to a reflector.
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

        /**
         * @brief Test whether any bit in @p flag is set in the culling mask.
         * @param flag A single flag or a combination of flags to test.
         * @return True if at least one requested bit is set.
         */
        bool IsCull(CullingFlags flag) const
        {
            return (UINT_CAST(culling) & UINT_CAST(flag)) > 0;
        }
    };
}