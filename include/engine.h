#pragma once
#include <memory>
#include "Perun/Core/Window.h"
#include "Perun/Graphics/Framebuffer.h" // New
#include "Perun/Graphics/Shader.h"      // New

// We don't need to include Renderer here as it is static, but we include it in cpp.
// Check if Engine needs to expose window? The original code had public window pointer.
// We should probably keep meaningful parts.

class Engine
{
private:
  bool running = false;
  std::unique_ptr<Perun::Core::Window> window;
  std::unique_ptr<Perun::Graphics::Framebuffer> m_Framebuffer;
  std::unique_ptr<Perun::Graphics::Shader> m_PostProcShader;

public:
  Engine();
  ~Engine();

  void handleInput();
  void render();
  void update(); // Added update method for Swarm

  const bool isRunning();
  
  std::pair<int, int> getMousePosition();
  bool isMouseButtonDown(int button);

  int getWidth() const;
  int getHeight() const;
  
  // draw() was presenting renderer. In Perun we swap buffers.
  void draw();
};
