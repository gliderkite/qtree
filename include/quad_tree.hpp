#ifndef QTREE_QUADTREE_H_
#define QTREE_QUADTREE_H_

#include "qnode.hpp"

#include <array>
#include <memory>

namespace qtree
{
    constexpr std::size_t NorthWest() { return 0; }
    constexpr std::size_t NorthEast() { return 1; }
    constexpr std::size_t SouthEast() { return 2; }
    constexpr std::size_t SouthWest() { return 3; }

    /// <summary>
    /// Gets the bounds of a quad tree child node in the given position (North-West,
    /// North-East, South-East, South-West).
    /// </summary>
    /// <param name="parentBounds">Bounds of the parent quad tree node.</param>
    template<typename TCoordinate, std::size_t Location>
    rect<TCoordinate> child_bounds(const rect<TCoordinate>& parentBounds)
    {
        switch (Location)
        {
        case NorthWest():
            return rect<TCoordinate>(
                parentBounds.left,
                parentBounds.top,
                parentBounds.left + (parentBounds.right - parentBounds.left) / static_cast<TCoordinate>(2),
                parentBounds.top + (parentBounds.bottom - parentBounds.top) / static_cast<TCoordinate>(2)
            );
        case NorthEast():
            return rect<TCoordinate>(
                parentBounds.left + (parentBounds.right - parentBounds.left) / static_cast<TCoordinate>(2),
                parentBounds.top,
                parentBounds.right,
                parentBounds.top + (parentBounds.bottom - parentBounds.top) / static_cast<TCoordinate>(2)
            );
        case SouthEast():
            return rect<TCoordinate>(
                parentBounds.left + (parentBounds.right - parentBounds.left) / static_cast<TCoordinate>(2),
                parentBounds.top + (parentBounds.bottom - parentBounds.top) / static_cast<TCoordinate>(2),
                parentBounds.right,
                parentBounds.bottom
            );
        case SouthWest():
            return rect<TCoordinate>(
                parentBounds.left,
                parentBounds.top + (parentBounds.bottom - parentBounds.top) / static_cast<TCoordinate>(2),
                parentBounds.left + (parentBounds.right - parentBounds.left) / static_cast<TCoordinate>(2),
                parentBounds.bottom
            );
        }
    }

    template<typename TElement, typename TCoordinate, std::size_t Depth>
    class quad_tree : public qnode<TElement, TCoordinate>
    {
        /// The parent node can access these private members.
        friend class quad_tree<TElement, TCoordinate, Depth + 1>;

        /// This node children will be created in the heap store (in order to avoid
        /// possible stack overflow for high levels of depth).
        using TNodePtr = std::unique_ptr<quad_tree<TElement, TCoordinate, Depth - 1>>;


    public:

        /// <summary>
        /// Initializes the instance with the given bounds.
        /// </summary>
        /// <param name="bounds">Quad tree bounds.</param>
        explicit quad_tree(rect<TCoordinate> bounds)
            : qnode<TElement, TCoordinate>(std::move(bounds))
        {
            init_children();
        }

        /// <summary>
        /// Gets the node depth.
        /// </summary>
        constexpr static std::size_t depth()
        {
            return Depth;
        }

        /// <summary>
        /// Gets the number of elements belonging to this node
        /// and to all is children nodes.
        /// </summary>
        std::size_t size() const override
        {
            auto n = qnode<TElement, TCoordinate>::size();

            for (const auto& child : _children)
            {
                n += child->size();
            }

            return n;
        }

        /// <summary>
        /// Returns true only if this node is empty and all of its
        /// children are empty, otherwise returns false.
        /// </summary>
        bool empty() const override
        {
            return size() == 0;
        }

        /// <summary>
        /// Removes all the element from the quad tree.
        /// </summary>
        void clear() override
        {
            for (const auto& child : _children)
            {
                child->clear();
            }

            qnode<TElement, TCoordinate>::clear();
        }

        /// <summary>
        /// Insert the given element into the quad tree.
        /// </summary>
        /// <param name="element">Element to be inserted.</param>
        /// <param name="bounds">Element bounds.</param>
        /// <returns>Returns true only if the element has been inserted,
        /// otherwise returns false.</returns>
        bool insert(TElement element, rect<TCoordinate> bounds) override
        {
            if (!this->contains(bounds))
            {
                // the given element cannot be contained by this node
                return false;
            }

            for (const auto& child : _children)
            {
                if (child->contains(bounds))
                {
                    return child->insert(std::move(element), std::move(bounds));
                }
            }

            // at this point none of the children completely contained the item.
            // add the element to this node.
            return qnode<TElement, TCoordinate>::insert(std::move(element), std::move(bounds));
        }

        /// <summary>
        /// Gets all the elements of the quad tree.
        /// </summary>
        /// <param name="elements">References to the elements of this quad tree.</param>
        void query(typename qnode<TElement, TCoordinate>::TElementRefContainer& elements) const override
        {
            for (const auto& child : _children)
            {
                child->query(elements);
            }

            qnode<TElement, TCoordinate>::query(elements);
        }

        /// <summary>
        /// Gets all the elements of the node that intersect the given area.
        /// </summary>
        /// <param name="area">Area to overlaps.</param>
        /// <param name="elements">References to the elements of this node that intersect
        /// the given area.</param>
        void query(const rect<TCoordinate>& area, typename qnode<TElement, TCoordinate>::TElementRefContainer& elements) const override
        {
            // this node may contain items that are not entirely contained by its children
            qnode<TElement, TCoordinate>::query(area, elements);

            for (const auto& child : _children)
            {
                // case 1: search area completely contained by child node
                // if a node completely contains the query area, go down that branch
                // and skip the remaining nodes
                if (child->contains(area))
                {
                    child->query(area, elements);
                    break;
                }

                // case 2: Child node completely contained by search area 
                // if the query area completely contains a child node,
                // add all the contents of that quad and its children.
                if (child->inside(area))
                {
                    child->query(elements);
                    continue;
                }

                // case 3: search area overlaps with child node
                // traverse into this quad, continue the loop to search other quads
                if (child->overlaps(area))
                {
                    child->query(area, elements);
                }
            }
        }


    private:

        /// <summary>
        /// Initializes the all the children nodes, according to the quad tree depth.
        /// </summary>
        void init_children()
        {
            const auto bounds = this->get_bounds();

            _children[NorthWest()].reset(
                new quad_tree<TElement, TCoordinate, Depth - 1>(child_bounds<TCoordinate, NorthWest()>(bounds)));

            _children[NorthEast()].reset(
                new quad_tree<TElement, TCoordinate, Depth - 1>(child_bounds<TCoordinate, NorthEast()>(bounds)));

            _children[SouthEast()].reset(
                new quad_tree<TElement, TCoordinate, Depth - 1>(child_bounds<TCoordinate, SouthEast()>(bounds)));

            _children[SouthWest()].reset(
                new quad_tree<TElement, TCoordinate, Depth - 1>(child_bounds<TCoordinate, SouthWest()>(bounds)));
        }

        std::array<TNodePtr, 4> _children;
    };

    /* quad_tree template specialization for Depth 0. */
    template<typename TElement, typename TCoordinate>
    class quad_tree<TElement, TCoordinate, 0> : public qnode<TElement, TCoordinate>
    {
        /// The parent node can access these private members.
        friend class quad_tree<TElement, TCoordinate, 1>;

        /// <summary>
        /// Initializes the instance with the given bounds.
        /// </summary>
        /// <param name="bounds">Quad tree bounds.</param>
        constexpr explicit quad_tree(rect<TCoordinate> bounds)
            : qnode<TElement, TCoordinate>(std::move(bounds))
        {
        }
    };
}

#endif
