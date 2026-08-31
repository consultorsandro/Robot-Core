#include "robot_core/length.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace robot_core
{
namespace
{

static_assert(!std::is_convertible_v<double, Length>);
static_assert(!std::is_convertible_v<Length, double>);
static_assert(!std::is_default_constructible_v<Length>);

TEST(LengthTest, ZeroConstructionSucceeds)
{
    EXPECT_NO_THROW((void)Length{0.0});
}

TEST(LengthTest, PositiveFiniteConstructionSucceeds)
{
    EXPECT_NO_THROW((void)Length{2.5});
}

TEST(LengthTest, MetersReturnsStoredValue)
{
    const Length length{2.5};
    EXPECT_EQ(length.meters(), 2.5);
}

TEST(LengthTest, NegativeValueThrows)
{
    EXPECT_THROW((void)Length{-1.0}, std::invalid_argument);
}

TEST(LengthTest, NaNThrows)
{
    EXPECT_THROW((void)Length{std::numeric_limits<double>::quiet_NaN()},
                 std::invalid_argument);
}

TEST(LengthTest, PositiveInfinityThrows)
{
    EXPECT_THROW((void)Length{std::numeric_limits<double>::infinity()},
                 std::invalid_argument);
}

TEST(LengthTest, NegativeInfinityThrows)
{
    EXPECT_THROW((void)Length{-std::numeric_limits<double>::infinity()},
                 std::invalid_argument);
}

TEST(LengthTest, EqualValuesCompareEqual)
{
    const Length first{2.5};
    const Length second{2.5};
    EXPECT_TRUE(first == second);
    EXPECT_FALSE(first != second);
}

TEST(LengthTest, DifferentValuesCompareUnequal)
{
    const Length first{2.5};
    const Length second{3.0};
    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
}

TEST(LengthTest, NegativeZeroIsValidAndEqualsZero)
{
    const Length length{-0.0};
    EXPECT_EQ(length.meters(), 0.0);
    EXPECT_EQ(length, Length{0.0});
}

TEST(LengthTest, LargestFiniteValueIsValid)
{
    const double maximum = std::numeric_limits<double>::max();
    EXPECT_EQ(Length{maximum}.meters(), maximum);
}

TEST(LengthTest, AdjacentFiniteValuesCompareUnequal)
{
    const Length first{1.0};
    const Length second{std::nextafter(1.0, 2.0)};
    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
}

} // namespace
} // namespace robot_core
