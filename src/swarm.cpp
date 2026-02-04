#include "../include/swarm.hpp"
#include "Perun/Graphics/Renderer.h"
#include "physics/Quadtree.h"
#include <stdlib.h>
#include <cmath>

Swarm::Swarm(int screenWidth, int screenHeight)
    : m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight)
{
  for( auto i = 0; i < numOfParticles; i++ )
  {
      Particle p;
      p.Position = {(float)(rand() % screenWidth), (float)(rand() % screenHeight)};
      // Random velocity
      p.Velocity = {(float)((rand() % 100) - 50) * 0.1f, (float)((rand() % 100) - 50) * 0.1f};
      p.Radius = 3.0f;
      p.Mass = 1.0f;
      
      // Random Color
      p.Color[0] = (rand() % 256) / 255.0f;
      p.Color[1] = (rand() % 256) / 255.0f;
      p.Color[2] = (rand() % 256) / 255.0f;
      p.Color[3] = 1.0f;
      
      m_Particles.push_back(p);
  }
}

Swarm::~Swarm() {}

void Swarm::render()
{
  for( const auto &p : m_Particles )
  {
      Perun::Renderer::DrawCircle(p.Position, p.Radius, p.Color, 1.0f, 0.05f);
  }
}

void Swarm::update(float dt, int mouseX, int mouseY, bool mouseDown)
{
  // Build Quadtree
  AABB boundary = {0, 0, (float)m_ScreenWidth, (float)m_ScreenHeight};
  Quadtree<Particle> qt(boundary, 4);
  
  for (auto& p : m_Particles) {
      qt.Insert(&p);
  }
  
  // dt is passed but we used fixed 1.0f before.
  // Let's use passed dt or a small fixed step if dt is too large/variable?
  // User asked for mouse force.
  
  float mouseForce = 50.0f; // Strength
  
  for( auto &p : m_Particles )
  {
    // Mouse Interaction
    if (mouseDown) {
        float dx = p.Position.x - mouseX;
        float dy = p.Position.y - mouseY;
        float distSq = dx * dx + dy * dy;
        if (distSq > 0.001f) {
             float dist = std::sqrt(distSq);
             // Repulsion (push away)
             float force = mouseForce / (dist + 1.0f); // Inverse linear or quadratic?
             // Use simpler strong push.
             if (dist < 200.0f) { // Radius of influence
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
    if (p.Position.x > m_ScreenWidth - p.Radius) { p.Position.x = m_ScreenWidth - p.Radius; p.Velocity.x *= -1; }
    if (p.Position.y < p.Radius) { p.Position.y = p.Radius; p.Velocity.y *= -1; }
    if (p.Position.y > m_ScreenHeight - p.Radius) { p.Position.y = m_ScreenHeight - p.Radius; p.Velocity.y *= -1; }
    
    // Particle Collisions via Quadtree
    // Query range: AABB around particle position with size 2*Radius
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
