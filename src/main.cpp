#include "../include/engine.h"
#include "../include/swarm.hpp"

int main( int argc, char* argv[] )
{
  Engine engine;

  Swarm swarm;

  while( engine.isRunning() )
  {
    engine.handleInput();
    swarm.render();
    engine.draw();
  }
}
