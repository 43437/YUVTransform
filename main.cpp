#include <iostream>
#include "ImageClass.hpp"

int main(int argc, char **argv) {
  
  ImageClass imageClass("../test1280x542.yuv");
  imageClass.draw();
  
  return 0;
}
