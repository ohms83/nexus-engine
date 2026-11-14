#pragma once

#include "../ScenePartition.h"
#include <array>

NXS_NAMESPACE
{
    /**
     * @brief A cache-friendly spatial partition structure implementing a 2D Quadtree on the XZ plane.
     *
     * The Quadtree partitions 3D space by subdividing bounding boxes along the horizontal XZ axes.
     * All tree nodes are pre-allocated as a single contiguous array (`m_tree`) indexed as a complete
     * 4-ary tree (where child @c q of node @c i is located at `4 * i + 1 + q`).
     *
     * Tree traversal (insertion, frustum querying, and subdivision) is implemented iteratively
     * using small stack buffers, eliminating recursion and dynamic heap allocations for high
     * cache locality and performance during visibility culling.
     */
    class Quadtree final : public IScenePartition
    {
    public:
        using NodeList = Array<PartitionEntry>;

        /**
         * @brief Identifies the spatial quadrant of a child node on the XZ plane.
         */
        enum class Quadrant : int8_t
        {
            None = -1,
            PlusXPlusZ = 0,   //!< Quadrant with +X and +Z relative to parent center.
            MinusXPlusZ = 1,  //!< Quadrant with -X and +Z relative to parent center.
            MinusXMinusZ = 2, //!< Quadrant with -X and -Z relative to parent center.
            PlusXMinusZ = 3,  //!< Quadrant with +X and -Z relative to parent center.
            Num = 4           //!< Total number of quadrants per subdivision level.
        };

        /**
         * @brief Represents a single spatial cell (node) stored in the flat tree array.
         */
        struct TreeNode
        {
            //! Whether this node has been activated and exists in the current tree hierarchy.
            bool create = false;

            //! Depth of this node in the quadtree hierarchy (0 for root, -1 if inactive).
            int32_t depth = -1;

            //! The quadrant direction of this node relative to its parent cell.
            Quadrant quadrant = Quadrant::None;

            //! World-space bounding box defining this partition cell's volume.
            Box bound;

            //! Scene node entries stored within this cell (populated for leaf nodes).
            NodeList nodeList;

            //! Pointers to child quadrants in `m_tree` if subdivided; empty if this node is a leaf.
            Array<TreeNode*> quadrants;

            /**
             * @brief Gets the depth of this node.
             * @return Zero-based depth level.
             */
            NODISCARD uint32_t GetDepth() const { return depth >= 0 ? static_cast<uint32_t>(depth) : 0; }

            /**
             * @brief Gets the number of scene node entries stored directly in this node.
             * @return The size of `nodeList`.
             */
            NODISCARD size_t GetNodeCount() const { return nodeList.size(); }

            /**
             * @brief Gets the world-space bounding box of this node.
             * @return Const reference to the bounding box.
             */
            NODISCARD const Box& GetBound() const { return bound; }

            /**
             * @brief Gets the child quadrants of this node.
             * @return Reference to the array of child node pointers (empty if leaf).
             */
            NODISCARD const Array<TreeNode*>& GetQuadrants() const { return quadrants; }
        };

        Quadtree() = delete;
        virtual ~Quadtree() = default;

        /**
         * @brief Constructs a pre-allocated Quadtree spatial partition.
         *
         * @param maxDepth The maximum allowable depth of subdivision (root is depth 0).
         * @param partitionSize The maximum number of items a leaf can hold before subdividing.
         * @param worldBound The world-space boundary enclosing the root of the quadtree.
         */
        explicit Quadtree(uint32_t maxDepth, uint32_t partitionSize, const Box& worldBound);

        /**
         * @brief Adds or updates a scene node entry in all overlapping leaf partitions.
         *
         * If an entry with the same identifier already exists in an overlapping leaf, its bound is
         * updated. If adding the entry causes a leaf to exceed `m_partitionSize` and its depth is less
         * than `m_maxDepth`, the leaf is subdivided and its items are redistributed.
         *
         * @param node Reference to the scene node.
         * @param bound World-space bounding box of the node.
         */
        void AddNode(Ref<SceneNode> node, const Box& bound) override;

        /**
         * @brief Updates the bounding box and spatial cell assignment for a scene node.
         *
         * Removes the node from its previous partition cells and re-inserts it with the new bound.
         *
         * @param id Identifier of the scene node.
         * @param bound New world-space bounding box.
         */
        void UpdateNode(Identifier id, const Box& bound) override;

        /**
         * @brief Removes a scene node by identifier from all partition cells across the tree.
         * @param id Identifier of the scene node to remove.
         */
        void RemoveNode(Identifier id) override;

        /**
         * @brief Clears all scene nodes from the tree and resets the hierarchy to root-only.
         */
        void RemoveAllNodes() override;

        /**
         * @brief Queries all visible scene nodes intersecting the specified camera frustum.
         *
         * Traverses overlapping cells iteratively. If objects span multiple quadrants, results
         * are automatically sorted and deduplicated so each node appears at most once.
         *
         * @param frustum Camera viewing frustum used for visibility culling.
         * @param results Output array populated with pointers to intersecting scene nodes.
         */
        void Query(const Frustum& frustum, Array<SceneNode*>& results) const override;

        /**
         * @brief Checks if a bounding box overlaps the root world boundary of the Quadtree.
         * @param bound The bounding box to test.
         * @return True if overlapping, false otherwise.
         */
        bool IsOverlap(const Box& bound) const;

        /**
         * @brief Gets the depth of the root partition (always 0).
         * @return Root depth.
         */
        NODISCARD uint32_t GetDepth() const override { return m_depth; }

        /**
         * @brief Gets the maximum subdivision depth configured for this tree.
         * @return Maximum depth.
         */
        NODISCARD uint32_t GetMaxDepth() const override { return m_maxDepth; }

        /**
         * @brief Gets the maximum number of items allowed in a leaf before subdivision.
         * @return Leaf capacity threshold.
         */
        NODISCARD uint32_t GetPartitionSize() const override { return m_partitionSize; }

        /**
         * @brief Gets the world-space bounding box of the entire quadtree.
         * @return Const reference to root boundary.
         */
        NODISCARD const Box& GetBound() const override { return m_bound; }

        /**
         * @brief Gets the number of items stored in the root partition.
         * @return Root node list size (0 once subdivided).
         */
        NODISCARD size_t GetNodeCount() const override { return m_tree.empty() ? 0 : m_tree[0].nodeList.size(); }

        /**
         * @brief Gets the root node's child quadrants.
         * @return Array of pointers to the 4 child tree nodes if subdivided, or an empty array.
         */
        NODISCARD const Array<TreeNode*>& GetQuadrants() const
        {
            static const Array<TreeNode*> empty;
            return m_tree.empty() ? empty : m_tree[0].quadrants;
        }

        /**
         * @brief Gets the entire pre-allocated flat array of tree nodes.
         * @return Const reference to `m_tree`.
         */
        NODISCARD const Array<TreeNode>& GetTree() const { return m_tree; }

    private:
        /**
         * @brief Subdivides the root node.
         */
        void Subdivide();

        /**
         * @brief Subdivides a specific node in `m_tree` by index.
         *
         * Activates its 4 pre-allocated children, calculates their bounding boxes, and redistributes
         * entries from the parent's `nodeList` into the children. Cascades subdivision if any child
         * exceeds capacity and has not reached `m_maxDepth`.
         *
         * @param nodeIndex Flat array index of the node to subdivide.
         */
        void Subdivide(uint32_t nodeIndex);

        /**
         * @brief Subdivides a node identified by depth and quadrant.
         * @param depth Depth level of the target node.
         * @param quadrant Spatial quadrant of the target node.
         */
        void Subdivide(int32_t depth, Quadrant quadrant);

        /**
         * @brief Internal iterative query traversal without deduplication.
         * @param frustum Camera frustum to test.
         * @param results Output collection of intersecting scene nodes.
         */
        void QueryInternal(const Frustum& frustum, Array<SceneNode*>& results) const;

        /**
         * @brief Computes a flat tree index for a given depth and quadrant.
         * @param depth Tree depth.
         * @param quadrant Child quadrant.
         * @return Computed array index.
         */
        static inline int32_t GetTreeIndex(int32_t depth, Quadrant quadrant)
        {
            if (depth <= 0) return 0;
            return static_cast<int32_t>(std::pow(4, depth)) + INT_CAST(quadrant) - 1;
        }

        /**
         * @brief Calculates the flat array index of child quadrant @p quadrant for parent @p parentIndex.
         * @param parentIndex Array index of the parent node.
         * @param quadrant Quadrant direction (0..3).
         * @return Flat array index of the child node (`4 * parentIndex + 1 + quadrant`).
         */
        static inline uint32_t GetChildIndex(uint32_t parentIndex, Quadrant quadrant)
        {
            return 4 * parentIndex + 1 + static_cast<uint32_t>(quadrant);
        }

        /**
         * @brief Calculates the flat array index of the parent node for child @p childIndex.
         * @param childIndex Array index of the child node.
         * @return Flat array index of the parent node (`(childIndex - 1) / 4`).
         */
        static inline uint32_t GetParentIndex(uint32_t childIndex)
        {
            return childIndex > 0 ? (childIndex - 1) / 4 : 0;
        }

        //! World partition boundary enclosing the entire tree.
        Box m_bound;

        //! Depth of root node (0).
        uint32_t m_depth = 0;

        //! Maximum subdivision depth allowed.
        uint32_t m_maxDepth = 0;

        //! Maximum number of items per partition leaf before subdividing.
        uint32_t m_partitionSize = 10;

        //! Number of quadrants per cell (4).
        static constexpr int NUM_QUADRANT = INT_CAST(Quadtree::Quadrant::Num);

        //! Flat pre-allocated array of all tree nodes.
        Array<TreeNode> m_tree;
    };
}