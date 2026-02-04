#pragma once
#include "Simulation.h"
#include "../physics/Particle.h"
#include <vector>

class CollisionSimulation : public Simulation {
public:
    CollisionSimulation(int width, int height);
    virtual ~CollisionSimulation() = default;

    void Update(float dt) override;
    void Render() override;
    
    void OnMouseMove(int x, int y) override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;

private:
    int m_Width, m_Height;
    std::vector<Particle> m_Particles;
    
    // Input State
    int m_MouseX = 0, m_MouseY = 0;
    bool m_MouseDown = false;
};
