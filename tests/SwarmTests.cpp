#include <gtest/gtest.h>
#include "../include/swarm.hpp"

TEST(SwarmTest, Update) {
    Swarm swarm(800, 600);
    // This should exist and update pixels.
    swarm.update(1.0f, 0, 0, false);
}
