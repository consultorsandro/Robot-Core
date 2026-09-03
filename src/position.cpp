#include "robot_core/position.hpp"

namespace robot_core
{

Position::Position(Coordinate x, Coordinate y, Coordinate z) noexcept
    : x_(x),
      y_(y),
      z_(z)
{
}

Coordinate Position::x() const noexcept
{
    return x_;
}

Coordinate Position::y() const noexcept
{
    return y_;
}

Coordinate Position::z() const noexcept
{
    return z_;
}

bool Position::operator==(const Position& other) const noexcept
{
    return x_ == other.x_ && y_ == other.y_ && z_ == other.z_;
}

bool Position::operator!=(const Position& other) const noexcept
{
    return !(*this == other);
}

} // namespace robot_core
