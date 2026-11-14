#pragma once

#include "../ScenePartition.h"
#include <array>

NXS_NAMESPACE
{
    class Quadtree final : public IScenePartition
    {
    public:
        Quadtree() = delete;
        virtual ~Quadtree() = default;
        explicit Quadtree(uint32_t maxDepth, uint32_t partitionSize, const Box& worldBound);

        void AddNode(Ref<SceneNode> node, const Box& bound) override;
        void UpdateNode(Identifier id, const Box& bound) override;
        void RemoveNode(Identifier id) override;
        void RemoveAllNodes() override;
        void Query(const Frustum& frustum, Array<SceneNode*>& results) const override;
        bool IsOverlap(const Box& bound) const;

        NODISCARD uint32_t GetDepth() const override { return m_depth; }
        NODISCARD uint32_t GetMaxDepth() const override { return m_maxDepth; }
        NODISCARD uint32_t GetPartitionSize() const override { return m_partitionSize; }
        NODISCARD const Box& GetBound() const override { return m_bound; }
        NODISCARD size_t GetNodeCount() const override { return m_nodeList.size(); }
        NODISCARD const Array<Ref<Quadtree>>& GetQuadrants() const { return m_quadrants; }

    private:
        enum class Quadrant : int8_t
        {
            PlusXPlusZ,
            MinusXPlusZ,
            MinusXMinusZ,
            PlusXMinusZ,
            Num
        };

        struct TreeNode
        {
            bool create = false;
            int32_t depth = -1;
            //! World partition's boundary.
            Box bound;
            Array<PartitionEntry> nodeList;
        };

        void Subdivide();
        void QueryInternal(const Frustum& frustum, Array<SceneNode*>& results) const;

        Box m_bound;

        uint32_t m_depth = 0;
        uint32_t m_maxDepth = 0;
        /**
         * @brief The maximum number of item per-partition. The tree
         * will start sub-dividing if the number of item exceeding this number
         * unless the maximum depth is reached.
         */
        uint32_t m_partitionSize = 10;

        static constexpr int NUM_QUADRANT = INT_CAST(Quadtree::Quadrant::Num);

        Array<PartitionEntry> m_nodeList;
        Array<Ref<Quadtree>> m_quadrants;
    };
}