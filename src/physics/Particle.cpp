#include "physics/Particle.h"
#include <cmath>

void Particle::ResolveCollision(Particle& a, Particle& b) {
    Perun::Math::Vector2 diff = {a.Position.x - b.Position.x, a.Position.y - b.Position.y};
    float distSq = diff.x * diff.x + diff.y * diff.y;
    float radiusSum = a.Radius + b.Radius;

    if (distSq >= radiusSum * radiusSum || distSq == 0.0f) return; // No collision or same pos

    float dist = std::sqrt(distSq);
    
    // Normal
    float nx = diff.x / dist;
    float ny = diff.y / dist;
    
    // Relative velocity
    float dvx = a.Velocity.x - b.Velocity.x;
    float dvy = a.Velocity.y - b.Velocity.y;
    
    // Velocity along normal
    float velAlongNormal = dvx * nx + dvy * ny;
    
    // Separating?
    if (velAlongNormal > 0) return;
    
    // Restitution (bounciness) - let's say 0.9
    float e = 0.9f;
    
    // Impulse scalar
    float j = -(1 + e) * velAlongNormal;
    j /= (1 / a.Mass + 1 / b.Mass);
    
    // Impulse
    float ix = j * nx;
    float iy = j * ny;
    
    a.Velocity.x += 1 / a.Mass * ix;
    a.Velocity.y += 1 / a.Mass * iy;
    b.Velocity.x -= 1 / b.Mass * ix;
    b.Velocity.y -= 1 / b.Mass * iy;
    
    // Positional correction (to prevent sinking)
    float percent = 0.8f; // Slop
    float slop = 0.01f;
    float penetration = radiusSum - dist;
    if (penetration > slop) {
        float cx = (penetration * percent * nx) / (1/a.Mass + 1/b.Mass);
        float cy = (penetration * percent * ny) / (1/a.Mass + 1/b.Mass);
        
        a.Position.x += cx * (1/a.Mass);
        a.Position.y += cy * (1/a.Mass);
        b.Position.x -= cx * (1/b.Mass);
        b.Position.y -= cy * (1/b.Mass);
    }
}
