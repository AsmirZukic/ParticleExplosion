#include "simulations/CollisionSimulation.h"
#include "physics/Quadtree.h"
#include "Perun/Graphics/Renderer.h"
#include <cmath>
#include <cstdlib>

CollisionSimulation::CollisionSimulation(int width, int height)
    : m_Width(width), m_Height(height)
{
    int numOfParticles = 2000;
    for( auto i = 0; i < numOfParticles; i++ )
    {
        Particle p;
        p.Position = {(float)(rand() % width), (float)(rand() % height)};
        p.Velocity = {(float)((rand() % 100) - 50) * 0.1f, (float)((rand() % 100) - 50) * 0.1f};
        p.Radius = 3.0f;
        p.Mass = 1.0f;
        
        p.Color[0] = (rand() % 256) / 255.0f;
        p.Color[1] = (rand() % 256) / 255.0f;
        p.Color[2] = (rand() % 256) / 255.0f;
        p.Color[3] = 1.0f;
        
        m_Particles.push_back(p);
    }
}

void CollisionSimulation::Update(float dt) {
    // Build Quadtree
    AABB boundary = {0, 0, (float)m_Width, (float)m_Height};
    Quadtree<Particle> qt(boundary, 4);
    
    for (auto& p : m_Particles) {
        qt.Insert(&p);
    }
    
    float mouseForce = 50.0f;
    
    for( auto &p : m_Particles )
    {
        // Mouse Interaction
        if (m_MouseDown) {
            float dx = p.Position.x - m_MouseX;
            float dy = p.Position.y - m_MouseY;
            float distSq = dx * dx + dy * dy;
            if (distSq > 0.001f) {
                 float dist = std::sqrt(distSq);
                 if (dist < 200.0f) {
                     float push = (200.0f - dist) * 0.05f;
                     p.Velocity.x += (dx / dist) * push;
                     p.Velocity.y += (dy / dist) * push;
                 }
            }
        }

        // Integration
        p.Integrate(dt);
        
        // Wall collisions
        if (p.Position.x < p.Radius) { p.Position.x = p.Radius; p.Velocity.x *= -1; }
        if (p.Position.x > m_Width - p.Radius) { p.Position.x = m_Width - p.Radius; p.Velocity.x *= -1; }
        if (p.Position.y < p.Radius) { p.Position.y = p.Radius; p.Velocity.y *= -1; }
        if (p.Position.y > m_Height - p.Radius) { p.Position.y = m_Height - p.Radius; p.Velocity.y *= -1; }
        
        // Particle Collisions
        AABB range = {p.Position.x - p.Radius * 2, p.Position.y - p.Radius * 2, p.Radius * 4, p.Radius * 4};
        std::vector<Particle*> neighbors;
        qt.Query(range, neighbors);
        
        for (auto* other : neighbors) {
            if (other != &p) {
                Particle::ResolveCollision(p, *other);
            }
        }
    }
}

void CollisionSimulation::Render() {
    for( const auto &p : m_Particles )
    {
        Perun::Renderer::DrawCircle(p.Position, p.Radius, p.Color, 1.0f, 0.05f);
    }
}

void CollisionSimulation::OnMouseMove(int x, int y) {
    m_MouseX = x;
    m_MouseY = y;
}

void CollisionSimulation::OnMouseDown(int button) {
    m_MouseDown = true;
}

void CollisionSimulation::OnMouseUp(int button) {
    m_MouseDown = false;
}
