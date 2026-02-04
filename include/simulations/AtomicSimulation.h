#pragma once
#include "Simulation.h"
#include "../physics/Particle.h"
#include <vector>
#include <SDL2/SDL.h>

struct AtomicParticle {
    Perun::Math::Vector2 Position;
    Perun::Math::Vector2 Velocity;
    int Type; // 0=Red, 1=Green, 2=Blue, 3=Yellow
    float Color[4];
};

class AtomicSimulation : public Simulation {
public:
    AtomicSimulation(int width, int height);
    virtual ~AtomicSimulation() = default;
    
    void Update(float dt) override;
    void Render() override;
    
    // Input Overrides
    void OnMouseMove(int x, int y) override { m_MouseX = x; m_MouseY = y; }
    void OnMouseDown(int button) override { 
        if (button == SDL_BUTTON_LEFT) m_LeftDown = true;
        if (button == SDL_BUTTON_RIGHT) m_RightDown = true;
    }
    void OnMouseUp(int button) override { 
        if (button == SDL_BUTTON_LEFT) m_LeftDown = false;
        if (button == SDL_BUTTON_RIGHT) m_RightDown = false;
    }

private:
    void RandomizeRules();
    void PrintRules();
    
    int m_Width, m_Height;
    std::vector<AtomicParticle> m_Particles;
    
    // Rules
    // rules[A][B] = force exerted on A by B
    float m_Rules[4][4];
    int m_Polarity[4]; // Integer Charge (-2, -1, 1, 2)
    
    // Physics Constants
    const float m_ForceConstant = 500.0f;
    const float m_CoreRadius = 8.0f;
    const float m_Friction = 0.94f; // Per sub-step
    const float m_MouseForceValue = 0.5f;
    const int m_SubSteps = 8;
    const float m_InteractionRange = 100.0f;
    const float m_MaxVelocity = 15.0f; // Soft cap per sub-step
    
    // Input State for Re-roll
    bool m_SpacePressed = false;
    
    // Input State for Interaction
    int m_MouseX = 0, m_MouseY = 0;
    bool m_LeftDown = false;
    bool m_RightDown = false;
};