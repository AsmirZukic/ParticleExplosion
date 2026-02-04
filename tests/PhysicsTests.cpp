#include <gtest/gtest.h>
#include "physics/Quadtree.h"
#include "physics/Particle.h"

TEST(PhysicsTest, ParticleCollision) {
    Particle p1;
    p1.Position = {10.0f, 10.0f};
    p1.Velocity = {1.0f, 0.0f};
    p1.Radius = 1.0f;
    p1.Mass = 1.0f;
    
    Particle p2;
    p2.Position = {11.5f, 10.0f}; // Overlapping radius sum (1+1=2 > 1.5)
    p2.Velocity = {-1.0f, 0.0f};
    p2.Radius = 1.0f;
    p2.Mass = 1.0f;
    
    // Head on collision
    Particle::ResolveCollision(p1, p2);
    
    // Velocities should flip (approximately, with restitution < 1)
    EXPECT_LT(p1.Velocity.x, 0.0f);
    EXPECT_GT(p2.Velocity.x, 0.0f);
}

TEST(PhysicsTest, QuadtreeInsertion) {
    AABB boundary = {0, 0, 100, 100};
    Quadtree<Particle> qt(boundary, 4);
    
    Particle p1;
    p1.Position = {10, 10};
    
    EXPECT_TRUE(qt.Insert(&p1));
    
    Particle pOutside;
    pOutside.Position = {150, 150};
    EXPECT_FALSE(qt.Insert(&pOutside));
}
