#include "ImageClass.hpp"

int ImageClass::createWindow(int width, int height)
{
  if( !glfwInit() )
  {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    getchar();
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow( width, height, "Tutorial 02 - Red triangle", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
    getchar();
    glfwTerminate();
    return -1;
  }
  
//   glViewport(0.0, 0.0, 1024, 768);
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);
  
  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices), textureVertices, GL_STATIC_DRAW);

  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexVertices), vertexVertices, GL_STATIC_DRAW);
  
  return 0;
}

void ImageClass::init(std::string file)
{ 
  int pos = file.find_last_of('/');
  assert(pos!=std::string::npos);
  
  std::string str1 = file.substr(pos);
  pos = str1.find_first_of("0123456789");
  assert(pos != std::string::npos);
  
  int pos1 = str1.find('.');
  assert(pos1 != std::string::npos);
  
  str1=str1.substr(pos, pos1-pos);
  std::cout<<"pixel "<<str1<<std::endl;
  
  const char* pix = str1.c_str();
  char *str2;
  
  pixel_w = strtol(pix, &str2, 10);
  pixel_h = strtol(str2+1, NULL, 10);
  assert(pixel_h!=0);
  assert(pixel_w!=0);
  
  std::cout<<"pixel_w "<<pixel_w<<std::endl;
  std::cout<<"pixel_h "<<pixel_h<<std::endl;
  
  buf = (uint8_t*)malloc(pixel_h*pixel_w*3/2);
  assert(buf!=NULL);
  
  if((infile=std::fopen(file.c_str(), "rb"))==NULL){  
    std::cerr<<"cannot open this file\n"<<std::endl;
  }  
  
  plane[0] = buf;  
  plane[1] = plane[0] + pixel_w*pixel_h;  
  plane[2] = plane[1] + pixel_w*pixel_h/4;  
  
  std::cout<<"width height "<<pixel_w<<" "<<pixel_h<<std::endl;
  
  int windowWidth = 1024;
  int windowHeight = windowWidth*pixel_h/pixel_w;
  createWindow(windowWidth, windowHeight);
  InitShaders();
}

ImageClass::ImageClass()
{
  init();
}

ImageClass::ImageClass(std::__cxx11::string file)
{
  init(file);
}

ImageClass::~ImageClass()
{
  std::cout<<"free buf "<<std::endl;
  free(buf);
}

void ImageClass::draw()
{
  //Clear  
  do
  {
    glClear(GL_COLOR_BUFFER_BIT);  
    
    if (std::fread(buf, 1, pixel_w*pixel_h*3/2, infile) != pixel_w*pixel_h*3/2){  
      // Loop  
      
      std::cerr<<"read byte not enough. "<<std::endl;
      std::fseek(infile, 0, 0);  
      std::fread(buf, 1, pixel_w*pixel_h*3/2, infile);  
    }  
    
    long pos = std::ftell(infile);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
    0,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    3,                                // size : U+V => 2
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(0);
    

//     glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, textureVertices); 
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    2,                                // size : U+V => 2
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(1);
	  
    //Y  
    glActiveTexture(GL_TEXTURE0);  
    glBindTexture(GL_TEXTURE_2D, id_y);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w, pixel_h, 0, GL_RED, GL_UNSIGNED_BYTE, plane[0]);   
    glUniform1i(textureUniformY, 0);      

    //U  
    glActiveTexture(GL_TEXTURE1);  
    glBindTexture(GL_TEXTURE_2D, id_u);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w/2, pixel_h/2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[1]);         
    glUniform1i(textureUniformU, 1);  

    //V  
    glActiveTexture(GL_TEXTURE2);  
    glBindTexture(GL_TEXTURE_2D, id_v);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w/2, pixel_h/2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[2]);      
    glUniform1i(textureUniformV, 2);     

    // Draw  
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); 
    glfwSwapBuffers(window);
    glfwPollEvents();
    
  }while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
      glfwWindowShouldClose(window) == 0 );
}

//Init Shader  
void ImageClass::InitShaders()  
{  
    ShaderInfo  shaders[] =
    {
        { GL_VERTEX_SHADER, "../image.vert" },
        { GL_FRAGMENT_SHADER, "../image.frag" },
        { GL_NONE, NULL }
    };

    GLuint p = LoadShaders( shaders );
    glUseProgram(p);
    //Program: Step4  
    glUseProgram(p);  
  
    //Get Uniform Variables Location  
    textureUniformY = glGetUniformLocation(p, "tex_y");  
    textureUniformU = glGetUniformLocation(p, "tex_u");  
    textureUniformV = glGetUniformLocation(p, "tex_v");   
  
    //Init Texture  
    glGenTextures(1, &id_y);   
    glBindTexture(GL_TEXTURE_2D, id_y);      
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
      
    glGenTextures(1, &id_u);  
    glBindTexture(GL_TEXTURE_2D, id_u);     
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
      
    glGenTextures(1, &id_v);   
    glBindTexture(GL_TEXTURE_2D, id_v);      
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  

}





