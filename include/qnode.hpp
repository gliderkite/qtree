#ifndef QTREE_QNODE_H_
#define QTREE_QNODE_H_

#include "rect.hpp"

#include <functional>
#include <utility>
#include <vector>

namespace qtree
{
    template<typename TElement, typename TCoordinate>
    class qnode
    {
    public:

        /// Vector of references to the node items.
        using TElementRefContainer = std::vector<std::reference_wrapper<TElement>>;

        /// <summary>
        /// Gets the node bounds.
        /// </summary>
        constexpr rect<TCoordinate> get_bounds() const
        {
            return _bounds;
        }

        /// <summary>
        /// Returns true only if the given rect can fit inside this node, otherwise returns false.
        /// </summary>
        constexpr bool contains(const rect<TCoordinate>& rect) const
        {
            return _bounds.contains(rect);
        }

        /// <summary>
        /// Returns true only if the given rect can fit inside this node, otherwise returns false.
        /// </summary>
        constexpr bool inside(const rect<TCoordinate>& rect) const
        {
            return rect.contains(_bounds);
        }

        /// <summary>
        /// Returns true only if this node overlaps the given rect, otherwise returns false.
        /// </summary>
        constexpr bool overlaps(const rect<TCoordinate>& rect) const
        {
            return _bounds.overlaps(rect);
        }


    protected:

        /// <summary>
        /// Initializes the instance with the given bounds.
        /// </summary>
        /// <param name="bounds">Node bounds.</param>
        constexpr explicit qnode(rect<TCoordinate> bounds)
            : _bounds(std::move(bounds))
        {
        }

        virtual ~qnode() noexcept = default;

        /// <summary>
        /// Gets the number of elements belonging to this node.
        /// </summary>
        virtual std::size_t size() const
        {
            return _elements.size();
        }

        /// <summary>
        /// Returns true only if this node is empty, otherwise returns false.
        /// </summary>
        virtual bool empty() const
        {
            return _elements.empty();
        }

        /// <summary>
        /// Removes all the element from the node.
        /// </summary>
        virtual void clear()
        {
            _elements.clear();
        }

        /// <summary>
        /// Insert the given element into the node.
        /// </summary>
        /// <param name="element">Element to be inserted.</param>
        /// <param name="bounds">Element bounds.</param>
        /// <returns>Returns true only if the element has been inserted,
        /// otherwise returns false.</returns>
        virtual bool insert(TElement element, rect<TCoordinate> bounds)
        {
            if (!contains(bounds))
            {
                return false;
            }

            _elements.emplace_back(std::move(element), std::move(bounds));
            return true;
        }

        /// <summary>
        /// Gets all the elements of the node.
        /// </summary>
        /// <param name="elements">References to the elements of this node.</param>
        virtual void query(TElementRefContainer& elements) const
        {
            for (const auto& e : _elements)
            {
                elements.emplace_back(const_cast<TElement&>(e.first));
            }
        }

        /// <summary>
        /// Gets all the elements of the node that intersect the given area.
        /// </summary>
        /// <param name="area">Area to overlaps.</param>
        /// <param name="elements">References to the elements of this node that intersect
        /// the given area.</param>
        virtual void query(const rect<TCoordinate>& area, TElementRefContainer& elements) const
        {
            for (const auto& e : _elements)
            {
                if (area.overlaps(e.second))
                {
                    elements.emplace_back(const_cast<TElement&>(e.first));
                }
            }
        }


    private:

        //// Each node is a pair where the first element is
        /// the node item and the second element is the rect
        /// the represents the bounds of the item.
        using TElementWrapper = std::pair<TElement, rect<TCoordinate>>;

        const rect<TCoordinate> _bounds;
        std::vector<TElementWrapper> _elements;
    };
}

#endif
