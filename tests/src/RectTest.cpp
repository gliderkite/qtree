#include "rect.hpp"

#include "gtest/gtest.h"
using namespace testing;

#define DECLARE_COORDINATES(Type)   \
    const Type left = 10;           \
    const Type top = 10;            \
    const Type right = 20;          \
    const Type bottom = 20;         \
    ASSERT_GE(right, left);         \
    ASSERT_GE(bottom, top);         \
    ASSERT_GT(left, 0);             \
    ASSERT_GT(top, 0);              \
    ASSERT_GE(left, right - left);  \
    ASSERT_GE(top, bottom - top);

namespace
{
    template<typename T>
    class RectTest : public Test
    {
    protected:

        const qtree::rect<T> _zeroRect;
    };

    using RectElementT = Types<
        short, unsigned short,
        int, unsigned,
        long, unsigned long,
        long long, unsigned long long,
        float, double, long double>;

    TYPED_TEST_CASE(RectTest, RectElementT);
}

TYPED_TEST(RectTest, ShouldConstruct)
{
}

TYPED_TEST(RectTest, ShouldBeZeroInitializedByDefault)
{
    ASSERT_EQ(0, TypeParam());
    EXPECT_EQ(this->_zeroRect.left, 0);
    EXPECT_EQ(this->_zeroRect.top, 0);
    EXPECT_EQ(this->_zeroRect.right, 0);
    EXPECT_EQ(this->_zeroRect.bottom, 0);
}

TYPED_TEST(RectTest, ShouldBeInitializedByValues)
{
    DECLARE_COORDINATES(TypeParam);
    const qtree::rect<TypeParam> rect(left, top, right, bottom);

    EXPECT_EQ(rect.left, left);
    EXPECT_EQ(rect.top, top);
    EXPECT_EQ(rect.right, right);
    EXPECT_EQ(rect.bottom, bottom);
}

TYPED_TEST(RectTest, ShouldCompare)
{
    DECLARE_COORDINATES(TypeParam);
    const qtree::rect<TypeParam> rect(left, top, right, bottom);

    EXPECT_EQ(qtree::rect<TypeParam>(), this->_zeroRect);
    EXPECT_NE(rect, this->_zeroRect);
    EXPECT_EQ(rect, rect);
}

TYPED_TEST(RectTest, ShouldThrowWhenCoordinateIsInvalid)
{
    DECLARE_COORDINATES(TypeParam);
    const TypeParam invalidRight = left - 1;
    const TypeParam invalidBottom = top - 1;

    EXPECT_THROW(qtree::rect<TypeParam>(left, top, invalidRight, bottom), std::invalid_argument);
    EXPECT_THROW(qtree::rect<TypeParam>(left, top, right, invalidBottom), std::invalid_argument);
    EXPECT_THROW(qtree::rect<TypeParam>(left, top, invalidRight, invalidBottom), std::invalid_argument);
}

TYPED_TEST(RectTest, ShouldGetWidthAndHeight)
{
    DECLARE_COORDINATES(TypeParam);
    const qtree::rect<TypeParam> rect(left, top, right, bottom);

    const auto width = right - left;
    const auto height = bottom - top;
    EXPECT_EQ(width, rect.width());
    EXPECT_EQ(height, rect.height());
}

TYPED_TEST(RectTest, ShouldContainAndOverlap)
{
    const qtree::rect<TypeParam> zeroRect;
    EXPECT_TRUE(this->_zeroRect.contains(zeroRect));
    EXPECT_TRUE(zeroRect.contains(this->_zeroRect));

    DECLARE_COORDINATES(TypeParam);
    const qtree::rect<TypeParam> rect(left, top, right, bottom);
    ASSERT_TRUE(rect.contains(rect));
    ASSERT_TRUE(rect.overlaps(rect));

    for (TypeParam x = left; x < right; x++)
    {
        for (TypeParam y = top; y < bottom; y++)
        {
            const qtree::rect<TypeParam> inner(x, y, right, bottom);
            ASSERT_TRUE(rect.contains(inner));
            ASSERT_TRUE(inner.overlaps(rect));
            ASSERT_TRUE(rect.overlaps(inner));
            ASSERT_TRUE(inner.overlaps(inner));
        }
    }
}

TYPED_TEST(RectTest, ShouldOverlap)
{
    DECLARE_COORDINATES(TypeParam);
    const qtree::rect<TypeParam> r1(left, top, right, bottom);

    for (TypeParam x = left - r1.width() + 1; x < right; x++)
    {
        for (TypeParam y = top - r1.height() + 1; y < bottom; y++)
        {
            const qtree::rect<TypeParam> r2(x, y, x + r1.width(), y + r1.height());
            EXPECT_TRUE(r2.overlaps(r1));
            EXPECT_TRUE(r1.overlaps(r2));
        }
    }
}
