// Based on: http://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/SIXTH_EDITION/CODE/CHAPTER03/WINDOWS_VERSIONS/example2.cpp
// Modified to isolate the main program and use GLM
#ifndef COMMON
#define COMMON

#include <GL/glew.h>
#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/gl.h>
#  include <GLUT/glut.h>
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  define glutInitDisplayMode(X) glutInitDisplayMode( (X) | GLUT_3_2_CORE_PROFILE )
#  define glutInitContextVersion(X,Y)
#  define glutInitContextProfile(X)
#else // non-Mac OS X operating systems
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define cellSize 1.25f
#define windowWidth 640
#define windowHeight 640
#define fieldWidth (windowWidth/2)
#define fieldHeight (windowHeight/2)
 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <chrono>
#include <vector>
#include <iostream>
#include <string.h>

extern GLuint InitShader(const char* vShaderFile, const char* fShaderFile);

// Implement the following...

extern const char *WINDOW_TITLE;
extern const double FRAME_RATE_MS;

extern void init(void);
extern void update(void);
extern void display(void);
extern void keyboard(unsigned char key, int x, int y);
extern void mouse(int button, int state, int x, int y);
extern void reshape(int width, int height);


typedef struct field_struct {
    GLuint fbo;
    GLuint texture;
} Field;

typedef struct pair_struct {
    Field foo;
    Field bar;
} Pair;

typedef struct shaders_struct {
    GLuint advect;
    GLuint diffuse;
    GLuint jacobi;
    GLuint divergence;
    GLuint subtractGradient;
    //more items here eventually
} Shaders;

// utilities.cpp
void initShaders(Shaders* shaders);
Field createField(GLint width, GLint height);
Pair createPair(int width, int height);
void clearField(Field field, float clearColor);
void swapField(Pair* fieldPair);
void unbind();
// liquid.cpp
// Function for every item in shader struct (to calculate)
// Will likely be in liquid2d.cpp

void initFields();

void advect(Field velocity, Field pressure, Field destination);
void subtractGradient(Field velocity, Field pressure, Field destination);
void divergence(Field velocityField, Field destination);
void jacobi(Field xField, Field bField, Field destination,float alphaParameter, float betaParameter);


#endif
