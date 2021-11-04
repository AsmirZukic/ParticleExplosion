#include "../include/pixel.hpp"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <ctime>

Pixel::Pixel() = default;

Pixel::Pixel( double xPos, double yPos ): xPos(xPos), yPos(yPos)
{
  angle = rand();
  velocity = ( rand () % 5 ) - 5;
  red = 255;
  green = 255;
  blue = 255;
}

void Pixel::render()
{
  SDL_SetRenderDrawColor( mRenderer->getRenderer(), getRed(), getGreen(), getBlue(), Alpha );
  SDL_RenderDrawPoint( mRenderer->getRenderer(), xPos, yPos );
}

void Pixel::move()
{
  double radAngle = angle * ( M_PI / 180 );

  double xSpeed = velocity * cos( radAngle );
  double ySpeed = velocity * sin( radAngle );

  xPos += xSpeed;
  yPos += ySpeed;

  angle += 0.4;

}

unsigned char Pixel::getRed()
{
  elapsed = SDL_GetTicks();
  red = (unsigned char) ( 1 + ( sin( elapsed * 0.0001 ) * 128 ) );
  return red;
}

unsigned char Pixel::getGreen()
{
  elapsed = SDL_GetTicks();
  green = (unsigned char) ( 1 + ( sin( elapsed * 0.0001 ) * 128 ) );
  return green;
}

unsigned char Pixel::getBlue()
{
  elapsed = SDL_GetTicks();
  blue = (unsigned char) ( 1 + ( cos( elapsed * 0.0001 ) * 128 )  );
  return blue;
}
