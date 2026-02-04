#include "simulations/AtomicSimulation.h"
#include "physics/Quadtree.h"
#include "Perun/Graphics/Renderer.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <SDL2/SDL.h> // For Key state

AtomicSimulation::AtomicSimulation(int width, int height)
    : m_Width(width), m_Height(height)
{
    RandomizeRules();
    
    int types = 4;
    int countPerType = 400; 
    
    for (int t = 0; t < types; ++t) {
        float color[4] = {0, 0, 0, 1};
        if (t == 0) { color[0] = 1.0f; color[1] = 0.2f; color[2] = 0.2f; } // Red
        if (t == 1) { color[0] = 0.2f; color[1] = 1.0f; color[2] = 0.2f; } // Green
        if (t == 2) { color[0] = 0.2f; color[1] = 0.2f; color[2] = 1.0f; } // Blue
        if (t == 3) { color[0] = 1.0f; color[1] = 1.0f; color[2] = 0.2f; } // Yellow
        
        for (int i = 0; i < countPerType; ++i) {
            AtomicParticle p;
            p.Position = {(float)(rand() % width), (float)(rand() % height)};
            p.Velocity = {0, 0};
            p.Type = t;
            p.Color[0] = color[0]; p.Color[1] = color[1]; p.Color[2] = color[2]; p.Color[3] = 1.0f;
            m_Particles.push_back(p);
        }
    }
}

void AtomicSimulation::RandomizeRules() {
    // Assign random charges to each particle type
    // Range: -2 to +2, excluding 0
    for (int i = 0; i < 4; ++i) {
        int c = (rand() % 5) - 2; // -2, -1, 0, 1, 2
        if (c == 0) c = 1; // Avoid inert particles
        m_Polarity[i] = c;
    }
    
    // Fill Interaction Matrix based on Coulomb's Law
    // g = (q1 * q2)
    // Same charge (q1*q2 > 0) -> Repel (g > 0)
    // Opposite charge (q1*q2 < 0) -> Attract (g < 0)
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m_Rules[i][j] = (float)(m_Polarity[i] * m_Polarity[j]);
        }
    }
}

void AtomicSimulation::PrintRules() {
    std::cout << "--- New Charges ---" << std::endl;
    for (int i = 0; i < 4; ++i) {
        std::cout << "Type " << i << ": " << m_Polarity[i] << std::endl;
    }
}

void AtomicSimulation::Update(float dt) {
    // Re-roll Check
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_SPACE]) {
        if (!m_SpacePressed) {
             RandomizeRules();
             PrintRules();
             m_SpacePressed = true;
        }
    } else {
        m_SpacePressed = false;
    }
    
    // PRECISION PHYSICS: Sub-stepping
    float subDt = dt / m_SubSteps;
    
    for (int step = 0; step < m_SubSteps; ++step) {
        // Build Quadtree for this substep
        AABB boundary = {0, 0, (float)m_Width, (float)m_Height};
        Quadtree<AtomicParticle> qt(boundary, 4);
        for (auto& p : m_Particles) {
            qt.Insert(&p);
        }

        for (size_t i = 0; i < m_Particles.size(); ++i) {
            float fx = 0, fy = 0;
            AtomicParticle& a = m_Particles[i];
            
            // Mouse Interaction (scaled by substeps to maintain consistent force)
            if (m_LeftDown || m_RightDown) {
                float dx = a.Position.x - m_MouseX;
                float dy = a.Position.y - m_MouseY;
                float distSq = dx * dx + dy * dy;
                if (distSq > 0.001f) {
                     float dist = std::sqrt(distSq);
                     if (dist < 250.0f) {
                         // Scale mouse force by substeps to maintain consistent interaction
                         float force = (250.0f - dist) * m_MouseForceValue / m_SubSteps;
                         if (m_LeftDown) {
                             fx += (dx / dist) * force;
                             fy += (dy / dist) * force;
                         } 
                         if (m_RightDown) {
                             fx -= (dx / dist) * force;
                             fy -= (dy / dist) * force;
                         }
                     }
                }
            }
            
            // Query Neighbors using Quadtree
            // We need to handle Toroidal Wrapping by querying multiple regions if near edge.
            std::vector<AtomicParticle*> neighbors;
            float r = m_InteractionRange;
            
            // Helper lambda to query safely
            auto queryRegion = [&](float qx, float qy) {
                 AABB range = {qx - r, qy - r, r * 2, r * 2};
                 qt.Query(range, neighbors);
            };
            
            // Center query
            queryRegion(a.Position.x, a.Position.y);
            
            // Toroidal queries (if near edge)
            bool nearLeft = a.Position.x < r;
            bool nearRight = a.Position.x > m_Width - r;
            bool nearTop = a.Position.y < r;
            bool nearBottom = a.Position.y > m_Height - r;
            
            if (nearLeft) queryRegion(a.Position.x + m_Width, a.Position.y);
            if (nearRight) queryRegion(a.Position.x - m_Width, a.Position.y);
            if (nearTop) queryRegion(a.Position.x, a.Position.y + m_Height);
            if (nearBottom) queryRegion(a.Position.x, a.Position.y - m_Height);
            
            // Corners
            if (nearLeft && nearTop) queryRegion(a.Position.x + m_Width, a.Position.y + m_Height);
            if (nearRight && nearTop) queryRegion(a.Position.x - m_Width, a.Position.y + m_Height);
            if (nearLeft && nearBottom) queryRegion(a.Position.x + m_Width, a.Position.y - m_Height);
            if (nearRight && nearBottom) queryRegion(a.Position.x - m_Width, a.Position.y - m_Height);

            // Iterate over found neighbors
            for (auto* otherPtr : neighbors) {
                if (otherPtr == &a) continue;
                AtomicParticle& b = *otherPtr;
                
                float dx = b.Position.x - a.Position.x;
                float dy = b.Position.y - a.Position.y;
                
                // Toroidal Wrap applied to distance vector
                if (dx > m_Width * 0.5f) dx -= m_Width;
                if (dx < -m_Width * 0.5f) dx += m_Width;
                if (dy > m_Height * 0.5f) dy -= m_Height;
                if (dy < -m_Height * 0.5f) dy += m_Height;
                
                float distSq = dx * dx + dy * dy;

                if (distSq > 0 && distSq < m_InteractionRange * m_InteractionRange) {
                    float dist = std::sqrt(distSq);
                    
                    // Coulomb's Law with Equilibrium Distance (Lennard-Jones style)
                    // Same charges (+ and +, or - and -): q1*q2 > 0 → repel
                    // Opposite charges (+ and -): q1*q2 < 0 → attract to equilibrium
                    int q1 = m_Polarity[a.Type];
                    int q2 = m_Polarity[b.Type];
                    float g = (float)(q1 * q2);
                    
                    // Equilibrium bond distance for opposite charges
                    const float bondDistance = 8.0f;
                    
                    float forceMag;
                    if (g < 0) {
                        // ATTRACTIVE: Use gentle spring-like force with equilibrium
                        // Scale by charge magnitude for realistic bonding
                        float chargeMagnitude = std::abs(g);
                        float displacement = dist - bondDistance;
                        
                        // Gentle spring force (much weaker than Coulomb)
                        forceMag = displacement * 20.0f * chargeMagnitude;
                        
                        // Extra gentle attraction at medium range to capture particles
                        if (dist > bondDistance && dist < 40.0f) {
                            forceMag -= 80.0f * chargeMagnitude / dist;
                        }
                    } else {
                        // REPULSIVE: Standard 1/r^2 Coulomb repulsion
                        float softDist = std::max(dist, 2.0f);
                        forceMag = g * m_ForceConstant / (softDist * softDist);
                    } 
                    
                    // CORE REPULSION (Pauli Exclusion)
                    // Only applies to same-charge particles (g > 0)
                    // Prevents same charges from overlapping
                    if (g > 0 && dist < m_CoreRadius) {
                        float corePush = (m_CoreRadius - dist) * 50.0f * std::abs(g);
                        forceMag += corePush;
                    }
                    
                    // Force direction: dx points from a to b
                    // Positive forceMag (repel) should push a AWAY from b (negative direction)
                    // Negative forceMag (attract) should pull a TOWARD b (positive direction)
                    fx -= (dx / dist) * forceMag;
                    fy -= (dy / dist) * forceMag;
                }
            }
            
            // Velocity Integration
            a.Velocity.x += fx * subDt;
            a.Velocity.y += fy * subDt;
            
            // Velocity Clamping (soft cap to prevent extreme speeds)
            float speed = std::sqrt(a.Velocity.x * a.Velocity.x + a.Velocity.y * a.Velocity.y);
            if (speed > m_MaxVelocity) {
                float scale = m_MaxVelocity / speed;
                a.Velocity.x *= scale;
                a.Velocity.y *= scale;
            }
        }
        
        // Position Update & Damping
        for (auto& p : m_Particles) {
            p.Position.x += p.Velocity.x * subDt;
            p.Position.y += p.Velocity.y * subDt;
            
            // Friction (Viscosity)
            p.Velocity.x *= m_Friction; 
            p.Velocity.y *= m_Friction;
            
            // Wrap Positions (Toroidal Topology)
            if (p.Position.x < 0) p.Position.x += m_Width;
            else if (p.Position.x >= m_Width) p.Position.x -= m_Width;
            
            if (p.Position.y < 0) p.Position.y += m_Height;
            else if (p.Position.y >= m_Height) p.Position.y -= m_Height;
        }
    }
}

void AtomicSimulation::Render() {
    for( const auto &p : m_Particles )
    {
        Perun::Renderer::DrawCircle(p.Position, 2.0f, p.Color, 1.0f, 0.05f);
    }
}