#pragma once

namespace robot_core
{

// A finite signed coordinate in meters relative to one World Frame axis.
class Coordinate
{
public:
    // Throws std::invalid_argument if meters is non-finite.
    explicit Coordinate(double meters);

    [[nodiscard]] double meters() const noexcept;

    [[nodiscard]] bool operator==(const Coordinate& other) const noexcept;
    [[nodiscard]] bool operator!=(const Coordinate& other) const noexcept;

private:
    double meters_;
};

} // namespace robot_core
