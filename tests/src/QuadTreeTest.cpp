#include "quad_tree.hpp"
using namespace qtree;

#include "gtest/gtest.h"
using namespace testing;

#include <algorithm>

namespace
{
    using TCoordinate = float;
    using TElement = int;

    template<typename TQuadTree>
    class QuadTreeTest : public Test
    {
    protected:

        using TElementsContainer = qnode<TElement, TCoordinate>::TElementRefContainer;

        QuadTreeTest()
            : _element()
            , _bounds(_left, _top, _right, _bottom)
            , _qtree(_bounds)
        {
        }

        void insertShouldFailForBounds(const rect<TCoordinate>& bounds)
        {
            ASSERT_FALSE(this->_qtree.contains(bounds));
            ASSERT_FALSE(this->_qtree.insert(this->_element, bounds));
            ASSERT_TRUE(this->_qtree.empty());
            ASSERT_EQ(this->_qtree.size(), 0);
        }

        template<std::size_t Location>
        rect<TCoordinate> getCornerBounds() const
        {
            TCoordinate left = this->_left;
            TCoordinate top = this->_top;
            TCoordinate right = this->_right;
            TCoordinate bottom = this->_bottom;
            
            // iterate to the deepest node
            for (std::size_t i = 0; i < TQuadTree::depth(); i++)
            {
                const auto corner = child_bounds<TCoordinate, Location>({ left, top, right, bottom });
                left = corner.left;
                top = corner.top;
                right = corner.right;
                bottom = corner.bottom;
            }

            return rect<TCoordinate>(left, top, right, bottom);
        }

        const TCoordinate _left = 10;
        const TCoordinate _top = 10;
        const TCoordinate _right = 20;
        const TCoordinate _bottom = 20;

        const TElement _element;
        const rect<TCoordinate> _bounds;

        TQuadTree _qtree;
    };

    // Test multiple depths
    using QuadTreeTypes = Types<
        quad_tree<TElement, TCoordinate, 1>,
        quad_tree<TElement, TCoordinate, 2>,
        quad_tree<TElement, TCoordinate, 3>,
        quad_tree<TElement, TCoordinate, 4>,
        quad_tree<TElement, TCoordinate, 5>,
        quad_tree<TElement, TCoordinate, 5>,
        quad_tree<TElement, TCoordinate, 6>,
        quad_tree<TElement, TCoordinate, 8>,
        quad_tree<TElement, TCoordinate, 9>,
        quad_tree<TElement, TCoordinate, 10>>;

    TYPED_TEST_CASE(QuadTreeTest, QuadTreeTypes);
}

TYPED_TEST(QuadTreeTest, ShouldGetChildrenBounds)
{
    const TCoordinate left = 0;
    const TCoordinate top = 0;
    const TCoordinate right = 10;
    const TCoordinate bottom = 10;

    const rect<TCoordinate> parent(left, top, right, bottom);

    // north-west
    const rect<TCoordinate> nw = child_bounds<TCoordinate, NorthWest()>(parent);
    EXPECT_EQ(0, nw.left);
    EXPECT_EQ(0, nw.top);
    EXPECT_EQ(5, nw.right);
    EXPECT_EQ(5, nw.bottom);

    // north-east
    const rect<TCoordinate> ne = child_bounds<TCoordinate, NorthEast()>(parent);
    EXPECT_EQ(5, ne.left);
    EXPECT_EQ(0, ne.top);
    EXPECT_EQ(10, ne.right);
    EXPECT_EQ(5, ne.bottom);

    // south-east
    const rect<TCoordinate> se = child_bounds<TCoordinate, SouthEast()>(parent);
    EXPECT_EQ(5, se.left);
    EXPECT_EQ(5, se.top);
    EXPECT_EQ(10, se.right);
    EXPECT_EQ(10, se.bottom);

    // south-west
    const rect<TCoordinate> sw = child_bounds<TCoordinate, SouthWest()>(parent);
    EXPECT_EQ(0, sw.left);
    EXPECT_EQ(5, sw.top);
    EXPECT_EQ(5, sw.right);
    EXPECT_EQ(10, sw.bottom);
}

TYPED_TEST(QuadTreeTest, ShouldConstruct)
{
}

TYPED_TEST(QuadTreeTest, ShouldGetBounds)
{
    EXPECT_EQ(this->_bounds, this->_qtree.get_bounds());
}

TYPED_TEST(QuadTreeTest, ShouldHaveNoElementsByDefault)
{
    EXPECT_TRUE(this->_qtree.empty());
    EXPECT_EQ(this->_qtree.size(), 0);
}

TYPED_TEST(QuadTreeTest, ShouldClear)
{
    // should not change the size when clear
    this->_qtree.clear();
    ASSERT_TRUE(this->_qtree.empty());
    ASSERT_EQ(this->_qtree.size(), 0);

    const std::size_t count = 1000;

    for (std::size_t i = 0; i < count; i++)
    {
        ASSERT_TRUE(this->_qtree.insert(this->_element, this->_bounds));
    }

    ASSERT_FALSE(this->_qtree.empty());
    ASSERT_EQ(this->_qtree.size(), count);

    this->_qtree.clear();
    ASSERT_TRUE(this->_qtree.empty());
    ASSERT_EQ(this->_qtree.size(), 0);
}

TYPED_TEST(QuadTreeTest, ShouldFailInsertingAnElementTooBig)
{
    // left
    this->insertShouldFailForBounds({ this->_left - 1, this->_top, this->_right, this->_bottom });
    // top
    this->insertShouldFailForBounds({ this->_left, this->_top - 1, this->_right, this->_bottom });
    // right
    this->insertShouldFailForBounds({ this->_left, this->_top, this->_right + 1, this->_bottom });
    // bottom
    this->insertShouldFailForBounds({ this->_left, this->_top, this->_right, this->_bottom + 1 });
}

TYPED_TEST(QuadTreeTest, ShouldInsert)
{
    std::size_t count = 0;

    for (TCoordinate x = this->_left; x <= this->_right; x++)
    {
        for (TCoordinate y = this->_top; y <= this->_bottom; y++)
        {
            const rect<TCoordinate> inner(x, y, this->_right, this->_bottom);
            ASSERT_TRUE(this->_qtree.contains(inner));
            EXPECT_TRUE(this->_qtree.insert(this->_element, inner));
            ASSERT_FALSE(this->_qtree.empty());
            ASSERT_EQ(this->_qtree.size(), ++count);
        }
    }
}

TYPED_TEST(QuadTreeTest, ShouldQuery)
{
    TElement element{};

    // insert elements
    for (TCoordinate x = this->_left; x <= this->_right; x++)
    {
        for (TCoordinate y = this->_top; y <= this->_bottom; y++)
        {
            const auto right = std::min(x + 1, this->_right);
            const auto bottom = std::min(y + 1, this->_bottom);
            const rect<TCoordinate> inner(x, y, right, bottom);
            ASSERT_TRUE(this->_qtree.insert(element++, inner));
        }
    }

    // get all the elements stored
    typename QuadTreeTest<TypeParam>::TElementsContainer elements;
    this->_qtree.query(elements);
    ASSERT_EQ(elements.size(), this->_qtree.size());

    element = TElement();
    // the queried elements are not returned in the same order they were inserted
    std::sort(std::begin(elements), std::end(elements));

    // check elements
    for (const auto e : elements)
    {
        ASSERT_EQ(element++, e);
    }
}

TYPED_TEST(QuadTreeTest, ShouldQueryArea)
{
    TElement element{};

    // insert elements in the quad tree corners
    const auto nw = this-> template getCornerBounds<NorthWest()>();
    ASSERT_TRUE(this->_qtree.insert(element++, nw));
    const auto ne = this-> template getCornerBounds<NorthEast()>();
    ASSERT_TRUE(this->_qtree.insert(element++, ne));
    const auto se = this-> template getCornerBounds<SouthEast()>();
    ASSERT_TRUE(this->_qtree.insert(element++, se));
    const auto sw = this-> template getCornerBounds<SouthWest()>();
    ASSERT_TRUE(this->_qtree.insert(element++, sw));

    ASSERT_EQ(4, this->_qtree.size());

    // query elements
    element = TElement();
    typename QuadTreeTest<TypeParam>::TElementsContainer elements;

    this->_qtree.query(nw, elements);
    ASSERT_EQ(1, elements.size());
    ASSERT_EQ(element++, elements.front());
    elements.clear();

    this->_qtree.query(ne, elements);
    ASSERT_EQ(1, elements.size());
    ASSERT_EQ(element++, elements.front());
    elements.clear();

    this->_qtree.query(se, elements);
    ASSERT_EQ(1, elements.size());
    ASSERT_EQ(element++, elements.front());
    elements.clear();

    this->_qtree.query(sw, elements);
    ASSERT_EQ(1, elements.size());
    ASSERT_EQ(element++, elements.front());
}
