#include "../include/engine.h"
#include "simulations/CollisionSimulation.h"
#include "simulations/AtomicSimulation.h"
#include <memory>
#include <SDL2/SDL.h> // For Key codes (SDL_SCANCODE_1)

int main( int argc, char* argv[] )
{
  Engine engine;

  std::unique_ptr<Simulation> simulation = std::make_unique<CollisionSimulation>(engine.getWidth(), engine.getHeight());

  while( engine.isRunning() )
  {
    engine.handleInput();
    
    // Switching
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_1]) {
        if (dynamic_cast<CollisionSimulation*>(simulation.get()) == nullptr)
            simulation = std::make_unique<CollisionSimulation>(engine.getWidth(), engine.getHeight());
    }
    if (state[SDL_SCANCODE_2]) {
        if (dynamic_cast<AtomicSimulation*>(simulation.get()) == nullptr)
            simulation = std::make_unique<AtomicSimulation>(engine.getWidth(), engine.getHeight());
    }
    
    // Input Handling
    std::pair<int, int> mousePos = engine.getMousePosition();
    bool mouseDown = engine.isMouseButtonDown(1);
    
    simulation->OnMouseMove(mousePos.first, mousePos.second);
    if(mouseDown) simulation->OnMouseDown(1);
    else simulation->OnMouseUp(1);
    
    // Render Loop (Atomic might need different dt)
    simulation->Update(1.0f);
    
    engine.render();
    simulation->Render();
    engine.draw();
  }
}
