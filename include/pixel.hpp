#pragma once

#include "renderer.hpp"
#include <iostream>

class Pixel
{
private:

  double xPos, yPos;
  double velocity, angle;
  unsigned char red, green, blue, Alpha = 255;
  Uint32 elapsed;
  Renderer* mRenderer = Renderer::getInstance();

public:

  Pixel();
  Pixel( double xPos, double yPos);
  void initPixel();

  void render();

  void move();

  unsigned char getRed();
  unsigned char getGreen();
  unsigned char getBlue();

};
