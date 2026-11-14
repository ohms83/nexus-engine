#pragma once

#include "../ScenePartition.h"
#include <array>

NXS_NAMESPACE
{
    class Quadtree final : public IScenePartition
    {
    public:
        using NodeList = Array<PartitionEntry>;

        enum class Quadrant : int8_t
        {
            None = -1,
            PlusXPlusZ = 0,
            MinusXPlusZ = 1,
            MinusXMinusZ = 2,
            PlusXMinusZ = 3,
            Num = 4
        };

        struct TreeNode
        {
            bool create = false;
            int32_t depth = -1;
            Quadrant quadrant = Quadrant::None;
            //! World partition's boundary.
            Box bound;
            NodeList nodeList;
            Array<TreeNode*> quadrants;

            NODISCARD uint32_t GetDepth() const { return depth >= 0 ? static_cast<uint32_t>(depth) : 0; }
            NODISCARD size_t GetNodeCount() const { return nodeList.size(); }
            NODISCARD const Box& GetBound() const { return bound; }
            NODISCARD const Array<TreeNode*>& GetQuadrants() const { return quadrants; }
        };

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
        NODISCARD size_t GetNodeCount() const override { return m_tree.empty() ? 0 : m_tree[0].nodeList.size(); }
        NODISCARD const Array<TreeNode*>& GetQuadrants() const
        {
            static const Array<TreeNode*> empty;
            return m_tree.empty() ? empty : m_tree[0].quadrants;
        }
        NODISCARD const Array<TreeNode>& GetTree() const { return m_tree; }

    private:
        void Subdivide(uint32_t nodeIndex);
        /**
         * @brief Subdividing a subtree from the specific quadrant and depth.
         * 
         * @param depth 
         * @param quadrant 
         */
        void Subdivide(int32_t depth, Quadrant quadrant);
        void QueryInternal(const Frustum& frustum, Array<SceneNode*>& results) const;

        static inline int32_t GetTreeIndex(int32_t depth, Quadrant quadrant)
        {
            if (depth <= 0) return 0;
            return static_cast<int32_t>(std::pow(4, depth)) + INT_CAST(quadrant) - 1;
        }

        static inline uint32_t GetChildIndex(uint32_t parentIndex, Quadrant quadrant)
        {
            return 4 * parentIndex + 1 + static_cast<uint32_t>(quadrant);
        }

        static inline uint32_t GetParentIndex(uint32_t childIndex)
        {
            return childIndex > 0 ? (childIndex - 1) / 4 : 0;
        }

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

        Array<TreeNode> m_tree;
    };
}