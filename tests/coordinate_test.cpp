#include "robot_core/coordinate.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace robot_core
{
namespace
{

static_assert(!std::is_convertible_v<double, Coordinate>);
static_assert(!std::is_convertible_v<Coordinate, double>);
static_assert(!std::is_default_constructible_v<Coordinate>);

TEST(CoordinateTest, NegativeFiniteConstructionSucceeds)
{
    EXPECT_NO_THROW((void)Coordinate{-2.5});
}

TEST(CoordinateTest, ZeroConstructionSucceeds)
{
    EXPECT_NO_THROW((void)Coordinate{0.0});
}

TEST(CoordinateTest, PositiveFiniteConstructionSucceeds)
{
    EXPECT_NO_THROW((void)Coordinate{2.5});
}

TEST(CoordinateTest, MetersReturnsStoredNegativeValue)
{
    const Coordinate coordinate{-2.5};
    EXPECT_EQ(coordinate.meters(), -2.5);
}

TEST(CoordinateTest, MetersReturnsStoredPositiveValue)
{
    const Coordinate coordinate{2.5};
    EXPECT_EQ(coordinate.meters(), 2.5);
}

TEST(CoordinateTest, NaNThrows)
{
    EXPECT_THROW((void)Coordinate{std::numeric_limits<double>::quiet_NaN()},
                 std::invalid_argument);
}

TEST(CoordinateTest, PositiveInfinityThrows)
{
    EXPECT_THROW((void)Coordinate{std::numeric_limits<double>::infinity()},
                 std::invalid_argument);
}

TEST(CoordinateTest, NegativeInfinityThrows)
{
    EXPECT_THROW((void)Coordinate{-std::numeric_limits<double>::infinity()},
                 std::invalid_argument);
}

TEST(CoordinateTest, EqualValuesCompareEqual)
{
    const Coordinate first{2.5};
    const Coordinate second{2.5};
    EXPECT_TRUE(first == second);
    EXPECT_FALSE(first != second);
}

TEST(CoordinateTest, DifferentValuesCompareUnequal)
{
    const Coordinate first{-2.5};
    const Coordinate second{2.5};
    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
}

TEST(CoordinateTest, NegativeZeroIsValidAndEqualsZero)
{
    const Coordinate coordinate{-0.0};
    EXPECT_EQ(coordinate.meters(), 0.0);
    EXPECT_EQ(coordinate, Coordinate{0.0});
}

TEST(CoordinateTest, LargestFinitePositiveValueIsValid)
{
    const double maximum = std::numeric_limits<double>::max();
    EXPECT_EQ(Coordinate{maximum}.meters(), maximum);
}

TEST(CoordinateTest, MostNegativeFiniteValueIsValid)
{
    const double minimum = std::numeric_limits<double>::lowest();
    EXPECT_EQ(Coordinate{minimum}.meters(), minimum);
}

TEST(CoordinateTest, AdjacentFiniteValuesCompareUnequal)
{
    const Coordinate first{1.0};
    const Coordinate second{std::nextafter(1.0, 2.0)};
    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
}

} // namespace
} // namespace robot_core
