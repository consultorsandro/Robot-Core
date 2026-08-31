cmake_minimum_required(VERSION 3.20)

project(robot_core
    VERSION 0.1.0
    DESCRIPTION "C++ robotics software core"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(robot_core STATIC
    src/length.cpp
)
target_include_directories(robot_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_features(robot_core PUBLIC cxx_std_20)

add_executable(robot_core_app
    app/main.cpp
)
target_link_libraries(robot_core_app PRIVATE robot_core)

include(CTest)

if(BUILD_TESTING)
    include(FetchContent)
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()

    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        URL_HASH SHA256=40d4ec942217dcc84a9ebe2a68584ada7d4a33a8ee958755763278ea1c5e18ff
    )
    # Match the application's default MSVC runtime library.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(robot_core_tests
        tests/length_test.cpp
    )
    target_link_libraries(robot_core_tests PRIVATE robot_core GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(robot_core_tests)
endif()
