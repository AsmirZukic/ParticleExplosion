#include "../include/swarm.hpp"
#include <stdlib.h>

Swarm::Swarm()
{
  for( auto i = 0; i < numOfPixels; i++ )
  {
    swarm.push_back( new Pixel( ( window->getWindowWidth() / 2 ), ( window->getWindowHeigt() / 2 ) ) );
  }

}

Swarm::~Swarm()
{
  for( auto &it : swarm )
  {
    delete it;
  }
}

void Swarm::render()
{
  for( auto &it : swarm )
  {
    it->render();
    it->move();
  }
}
