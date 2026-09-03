#pragma once

#include "robot_core/coordinate.hpp"

namespace robot_core
{

// A three-dimensional Cartesian position in the fixed World Frame.
class Position
{
public:
    Position(Coordinate x, Coordinate y, Coordinate z) noexcept;

    [[nodiscard]] Coordinate x() const noexcept;
    [[nodiscard]] Coordinate y() const noexcept;
    [[nodiscard]] Coordinate z() const noexcept;

    [[nodiscard]] bool operator==(const Position& other) const noexcept;
    [[nodiscard]] bool operator!=(const Position& other) const noexcept;

private:
    Coordinate x_;
    Coordinate y_;
    Coordinate z_;
};

} // namespace robot_core
