#include "robot_core/coordinate.hpp"

#include <cmath>
#include <stdexcept>

namespace robot_core
{

Coordinate::Coordinate(double meters)
    : meters_(meters)
{
    if (!std::isfinite(meters))
    {
        throw std::invalid_argument("Coordinate must be finite.");
    }
}

double Coordinate::meters() const noexcept
{
    return meters_;
}

bool Coordinate::operator==(const Coordinate& other) const noexcept
{
    return meters_ == other.meters_;
}

bool Coordinate::operator!=(const Coordinate& other) const noexcept
{
    return !(*this == other);
}

} // namespace robot_core
