#pragma once

#include "pixel.hpp"
#include "window.hpp"
#include <vector>

class Swarm
{
private:
  int numOfPixels = 200;
  std::vector< Pixel* > swarm;
  Window* window = Window::getInstance();

public:
  Swarm();
  ~Swarm();

  void render();
};
