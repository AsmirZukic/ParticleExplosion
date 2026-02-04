#pragma once
#include "Perun/Math/Vector2.h"

struct Particle {
    Perun::Math::Vector2 Position;
    Perun::Math::Vector2 Velocity;
    float Radius = 2.0f;
    float Mass = 1.0f;
    float Color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    void Integrate(float dt) {
        Position.x += Velocity.x * dt;
        Position.y += Velocity.y * dt;
    }
    
    // Simple elastic collision resolution
    static void ResolveCollision(Particle& a, Particle& b);
};
