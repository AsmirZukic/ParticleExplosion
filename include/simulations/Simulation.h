#pragma once

class Simulation {
public:
    virtual ~Simulation() = default;

    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    
    // Input
    virtual void OnMouseMove(int x, int y) {}
    virtual void OnMouseDown(int button) {}
    virtual void OnMouseUp(int button) {}
};
