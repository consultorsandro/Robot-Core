#include "robot_core/length.hpp"

#include <cmath>
#include <stdexcept>

namespace robot_core
{

Length::Length(double meters)
    : meters_(meters)
{
    if (!std::isfinite(meters) || meters < 0.0)
    {
        throw std::invalid_argument("Length must be finite and non-negative.");
    }
}

double Length::meters() const noexcept
{
    return meters_;
}

bool Length::operator==(const Length& other) const noexcept
{
    return meters_ == other.meters_;
}

bool Length::operator!=(const Length& other) const noexcept
{
    return !(*this == other);
}

} // namespace robot_core
