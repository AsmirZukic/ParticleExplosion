#pragma once

#include "physics/Particle.h"
#include <vector>

class Swarm
{
private:
  int numOfParticles = 2000;
  std::vector<Particle> m_Particles;
  int m_ScreenWidth;
  int m_ScreenHeight;

public:
  Swarm(int screenWidth, int screenHeight);
  ~Swarm();

  void render();
  void update(float dt, int mouseX, int mouseY, bool mouseDown);
};
