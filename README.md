# Robot Core

A C++ robotics software project focused on building a modular and testable software foundation for robotic systems.

This project is part of my long-term development roadmap toward becoming a Robotics Software Engineer, with an emphasis on modern C++, software architecture, testing, simulation, and robotics-oriented development practices.

## Project Status

**Version:** 0.1.0  
**Status:** Initial development

The current version establishes the initial CMake-based project structure and implements the `Length` and `Coordinate` domain types.

## Current Features

- C++20 project configuration
- CMake-based build system
- `robot_core` static library
- Validated `Length` domain type
- Validated signed spatial `Coordinate` domain type
- GoogleTest unit tests registered with CTest
- Microsoft Visual C++ compiler support
- Initial executable application
- Git version control
- Automated build artifact exclusion through `.gitignore`

## Technologies

- C++20
- CMake
- MSVC
- Git
- GitHub

## Build

The project uses CMake as its build system.

Create a build directory:

```text
mkdir build
cd build
```

Configure the project:

```text
cmake ..
```

Build the project:

```text
cmake --build . --config Debug
```

Run the application:

```text
Debug\robot_core_app.exe
```

Run the tests:

```text
ctest -C Debug --output-on-failure
```

## Project Structure

```text
robot-core/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── app/
│   └── main.cpp
├── include/
│   └── robot_core/
│       ├── coordinate.hpp
│       └── length.hpp
├── src/
│   ├── coordinate.cpp
│   └── length.cpp
├── tests/
│   ├── coordinate_test.cpp
│   └── length_test.cpp
└── docs/
    └── architecture/
        ├── robot-core-architecture-baseline.md
        ├── robot-core-architecture-baseline-v1.1.md
        └── robot-core-architecture-baseline-v1.2.md
```

The build/ directory is generated locally and is intentionally excluded from version control.

# Roadmap

The project will evolve incrementally toward a more realistic robotics software architecture.

Planned areas include:

Robot state management
Object-oriented design
Sensor abstraction
Actuator abstraction
Battery and power management
Error handling
Unit testing
Simulation
Telemetry
Robotics-oriented software architecture
Integration with robotics frameworks

## Development Philosophy

This project is developed incrementally, with emphasis on understanding the underlying engineering decisions rather than simply producing working code.

The development process follows a cycle of:

Design → Implement → Build → Test → Debug → Refactor → Document

## License

License information will be added in a future release.
