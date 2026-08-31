#pragma once

namespace robot_core
{

// A finite, non-negative physical length in meters.
class Length
{
public:
    // Throws std::invalid_argument if meters is negative or non-finite.
    explicit Length(double meters);

    [[nodiscard]] double meters() const noexcept;

    [[nodiscard]] bool operator==(const Length& other) const noexcept;
    [[nodiscard]] bool operator!=(const Length& other) const noexcept;

private:
    double meters_;
};

} // namespace robot_core
