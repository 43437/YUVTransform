#ifndef __IMAGE_CLASS__
#define __IMAGE_CLASS__

#include <cstdlib>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <vector>
#include "LoadShaders.h"

extern "C"{
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>  
}

// Include GLM
#include <glm/glm.hpp>  


using namespace glm;

#define ATTRIB_VERTEX 3  
#define ATTRIB_TEXTURE 4 

class ImageClass{
  
  GLFWwindow* window;
  
  GLuint id_y;
  GLuint id_u;
  GLuint id_v; // Texture id  
  GLuint textureUniformY, textureUniformU,textureUniformV; 
  GLuint vertexbuffer, uvbuffer;
  std::string file;
  
  int pixel_w = 0, pixel_h = 0;
  
  uint8_t *buf;
  uint8_t *plane[3]; 
  
  std::FILE *infile = NULL;  

  int createWindow(int width, int height);
  void InitShaders();
  void init(std::string file="../test426x240.yuv");
  
public:
  ImageClass();
  ImageClass(std::string file);
  virtual ~ImageClass();
  void draw();
};


#if TEXTURE_ROTATE  
    static const GLfloat vertexVertices[] = {  
        -1.0f, -0.5f,  
         0.5f, -1.0f,  
        -0.5f,  1.0f,  
         1.0f,  0.5f,  
    };      
#else  
    static const GLfloat vertexVertices[] = {  
	
      -1.0f, -1.0f, 0.0f,
      1.0f, -1.0f,  0.0f,
      1.0f,  1.0f,  0.0f,
      -1.0f,  1.0f,  0.0f,
    };      
#endif  
  
#if TEXTURE_HALF  
    static const GLfloat textureVertices[] = {  
        0.0f,  1.0f,  
        0.5f,  1.0f,  
        0.0f,  0.0f,  
        0.5f,  0.0f,  
    };   
#else  
    static const GLfloat textureVertices[] = {  
      1.0f, 1.0f,
      0.0f, 1.0f,
      0.0f,  0.0f,  
      1.0f,  0.0f,  
    };   
#endif  

        static short VERTEX_INDEX[] = { 0, 1, 2, 0, 2, 3 };
#endif