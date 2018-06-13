#ifndef QTREE_RECT_H_
#define QTREE_RECT_H_

#include <stdexcept>

namespace qtree
{
    template<typename T>
    class rect
    {
    public:

        /// <summary>
        /// Zero initializes the rect's coordinates.
        /// </summary>
        /// <remarks>The origin of the coordinates is supposed to be on
        /// the top-left corner, going to to bottom and left to right.</remarks>
        constexpr rect() noexcept
            : left{}
            , top{}
            , right{}
            , bottom{}
        {
        }

        /// <summary>
        /// Initialises the instance with the given coordinates.
        /// Throws std::invalid_argument if right is smaller than left
        /// or bottom is smaller than top.
        /// </summary>
        /// <param name="left">Left coordinate.</param>
        /// <param name="top">Top coordinate.</param>
        /// <param name="right">Right coordinate.</param>
        /// <param name="bottom">Bottom coordinate.</param>
        /// <remarks>The origin of the coordinates is supposed to be on
        /// the top-left corner going to to bottom and left to right.</remarks>
        constexpr rect(T left, T top, T right, T bottom)
            : left{left}
            , top{top}
            , right{right < left ? throw std::invalid_argument("Invalid coordinates.") : right}
            , bottom{bottom < top ? throw std::invalid_argument("Invalid coordinates.") : bottom}
        {
        }

        /// <summary>
        /// Returns true only if the given rect is different
        /// from this, otherwise returns false.
        /// </summary>
        constexpr bool operator!=(const rect& rect) const noexcept
        {
            return (left != rect.left || top != rect.top || right != rect.right || bottom != rect.bottom);
        }

        /// <summary>
        /// Returns true only if the given rect is equal
        /// from this, otherwise returns false.
        /// </summary>
        constexpr bool operator==(const rect& rect) const noexcept
        {
            return !(*this != rect);
        }

        /// <summary>
        /// Gets the width of the rectangle.
        /// </summary>
        constexpr T width() const noexcept
        {
            return right - left;
        }

        /// <summary>
        /// Gets the height of the rectangle.
        /// </summary>
        constexpr T height() const noexcept
        {
            return bottom - top;
        }

        /// <summary>
        /// Returns true only if the given rect can fit inside this, otherwise returns false.
        /// </summary>
        constexpr bool contains(const rect& rect) const noexcept
        {
            return (left <= rect.left && right >= rect.right && top <= rect.top && bottom >= rect.bottom) ? true : false;
        }

        /// <summary>
        /// Returns true only if the given rect overlaps this, otherwise returns false.
        /// </summary>
        constexpr bool overlaps(const rect& rect) const noexcept
        {
            return (left < rect.right && right > rect.left && bottom > rect.top && top < rect.bottom) ? true : false;
        }

        const T left;
        const T top;
        const T right;
        const T bottom;
    };
}

#endif
