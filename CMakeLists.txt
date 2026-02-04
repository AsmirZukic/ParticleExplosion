cmake_minimum_required(VERSION 3.10)
project(ParticleExplosion)

set(CMAKE_CXX_STANDARD 17)

# Locate SDL2
find_package(SDL2 REQUIRED)

# Locate Perun (Assuming it's in a sibling directory for now, or we add it as subdirectory if we can)
# For this environment, let's assume we can add it as a subdirectory if it was a submodule, 
# but since it's a sibling, we might need to point to it. 
# However, the user said "build on top of that".
# Let's try to include it directly if possible, or just link libraries if installed.
# Given the structure:
# Projects/
#   ParticleExplosion/
#   Perun/
# We can try adding Perun as a subdirectory if we were in a parent CMake, 
# but here we are in ParticleExplosion.
# A common way is to allow fetching or just point to include dirs and link libs if not installed.
# But Perun has a CMakeLists.txt. Let's try to add it using a relative path.
add_subdirectory(../Perun perun_build)

include_directories(include)
include_directories(../Perun/include)

set(SOURCES
    src/main.cpp
    src/engine.cpp
    src/swarm.cpp
    src/simulations/CollisionSimulation.cpp
    src/simulations/AtomicSimulation.cpp
    src/physics/Particle.cpp
)

# We are going to refactor, so we might want to exclude files as we replace them.
# The plan says we will replace Window/Renderer.
# For TDD, we want a test executable first.

enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(ParticleExplosion ${SOURCES})
target_link_libraries(ParticleExplosion PRIVATE Perun SDL2::SDL2)


# Test executable
add_executable(unit_tests 
    tests/SwarmTests.cpp
    tests/PhysicsTests.cpp
    src/swarm.cpp
    src/physics/Particle.cpp
    # We might need to decouple Pixel/Swarm from full Engine/Window for easy testing
    # Use mocks or refactor so they don't depend on global Singletons in tests
)

target_link_libraries(unit_tests PRIVATE 
    Perun 
    SDL2::SDL2 
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
