#include "robot_core/position.hpp"

#include <gtest/gtest.h>

#include <type_traits>

namespace robot_core
{
namespace
{

static_assert(!std::is_default_constructible_v<Position>);
static_assert(std::is_constructible_v<Position, Coordinate, Coordinate, Coordinate>);
static_assert(!std::is_constructible_v<Position, double, double, double>);
static_assert(std::is_nothrow_constructible_v<Position,
                                              Coordinate,
                                              Coordinate,
                                              Coordinate>);

TEST(PositionTest, ConstructionPreservesXCoordinate)
{
    const Position position{Coordinate{1.0}, Coordinate{2.0}, Coordinate{3.0}};
    EXPECT_EQ(position.x(), Coordinate{1.0});
}

TEST(PositionTest, ConstructionPreservesYCoordinate)
{
    const Position position{Coordinate{1.0}, Coordinate{2.0}, Coordinate{3.0}};
    EXPECT_EQ(position.y(), Coordinate{2.0});
}

TEST(PositionTest, ConstructionPreservesZCoordinate)
{
    const Position position{Coordinate{1.0}, Coordinate{2.0}, Coordinate{3.0}};
    EXPECT_EQ(position.z(), Coordinate{3.0});
}

TEST(PositionTest, NegativeCoordinatesArePreserved)
{
    const Position position{Coordinate{-1.0}, Coordinate{-2.0}, Coordinate{-3.0}};
    EXPECT_EQ(position.x(), Coordinate{-1.0});
    EXPECT_EQ(position.y(), Coordinate{-2.0});
    EXPECT_EQ(position.z(), Coordinate{-3.0});
}

TEST(PositionTest, MixedSignedCoordinatesAreSupported)
{
    const Position position{Coordinate{-1.0}, Coordinate{0.0}, Coordinate{1.0}};
    EXPECT_EQ(position.x(), Coordinate{-1.0});
    EXPECT_EQ(position.y(), Coordinate{0.0});
    EXPECT_EQ(position.z(), Coordinate{1.0});
}

TEST(PositionTest, ExplicitOriginIsRepresentable)
{
    const Position origin{Coordinate{0.0}, Coordinate{0.0}, Coordinate{0.0}};
    EXPECT_EQ(origin.x(), Coordinate{0.0});
    EXPECT_EQ(origin.y(), Coordinate{0.0});
    EXPECT_EQ(origin.z(), Coordinate{0.0});
}

TEST(PositionTest, IdenticalPositionsCompareEqual)
{
    const Position first{Coordinate{1.0}, Coordinate{2.0}, Coordinate{3.0}};
    const Position second{Coordinate{1.0}, Coordinate{2.0}, Coordinate{3.0}};
    EXPECT_TRUE(first == second);
    EXPECT_FALSE(first != second);
}

TEST(PositionTest, PositionsDifferingOnlyInXCompareUnequal)
{
    const Position first{Coordinate{1.0}, Coordinate{2.0}, Coordinate{3.0}};
    const Position second{Coordinate{4.0}, Coordinate{2.0}, Coordinate{3.0}};
    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
}

TEST(PositionTest, PositionsDifferingOnlyInYCompareUnequal)
{
    const Position first{Coordinate{1.0}, Coordinate{2.0}, Coordinate{3.0}};
    const Position second{Coordinate{1.0}, Coordinate{4.0}, Coordinate{3.0}};
    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
}

TEST(PositionTest, PositionsDifferingOnlyInZCompareUnequal)
{
    const Position first{Coordinate{1.0}, Coordinate{2.0}, Coordinate{3.0}};
    const Position second{Coordinate{1.0}, Coordinate{2.0}, Coordinate{4.0}};
    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
}

} // namespace
} // namespace robot_core
