#include "common.h"

const char *WINDOW_TITLE = "Liquid Dynamics";
const double FRAME_RATE_MS = 1000.0 / 60.0;

typedef glm::vec4 color4; // name vec4 as color4 for clarity
typedef glm::vec4 point4; // name vec4 as point4 for clarity

typedef glm::vec3 color3; // name vec4 as color4 for clarity
typedef glm::vec3 point3; // name vec4 as point4 for clarity

typedef glm::vec2 color2; // name vec4 as color4 for clarity
typedef glm::vec2 point2; // name vec4 as point4 for clarity


//-------------------------------------------------------------------
                     //function prototypes
float mouseConvert(int mouseVal, int windowScale);
void updateBuffer();
bool cmpf(GLfloat a, GLfloat b, GLfloat epsilon = .0005f);
//-------------------------------------------------------------------

//constants and variables
const GLfloat LEFT = -1.0f;
const GLfloat RIGHT = 1.0f;
const GLfloat TOP = 1.0f;
const GLfloat BOTTOM = -1.0f;
const GLfloat Z_NEAR = -1.0f;
const GLfloat Z_FAR  = 1.0f;
//boolean used for double clicking
bool mousePressed = false;
//window sizes
GLint windowWidth = 640;
GLint windowHeight= 640;

const color4 clearColour = color4(0,0,0,1); //black background
//--------------------------------------------------------------------

//point4 vertices0[6] = {
//  point4(-1.0,  1.0,  0.0, 1.0),
//  point4(-1.0, -1.0,  0.0, 1.0),
//  point4(1.0, -1.0,  0.0, 1.0),
//  point4(-1.0,  1.0, 0.0, 1.0),
//  point4(1.0, -1.0,  0.0, 1.0),
//  point4(1.0,  1.0,  0.0, 1.0)
//};

point4 vertices[6] = {
    point4(-0.5,  0.5,  0.0, 1.0),
    point4(-0.5, -0.5,  0.0, 1.0),
    point4(0.5, -0.5,  0.0, 1.0),
    point4(-0.5,  0.5,  0.0, 1.0),
    point4(0.5, -0.5,  0.0, 1.0),
    point4(0.5,  0.5,  0.0, 1.0)
};
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

GLuint Projection, Colour;
GLuint VAO;
GLuint buffer;
GLuint program, vPosition;
//----------------------------------------------------------------------------

// OpenGL initialization

void init()
{
   program = InitShader("vshader.glsl", "fshader.glsl");
   glUseProgram(program);

   vPosition = glGetAttribLocation(program, "vPosition");
   
   glGenVertexArrays(1, &VAO);

   updateBuffer();
   Projection = glGetUniformLocation(program, "Projection");
   Colour =glGetUniformLocation(program, "colour");

   glEnable(GL_DEPTH_TEST);

   glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
}

//----------------------------------------------------------------------------

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //draw methods here


   glDrawArrays(GL_TRIANGLES, 0, 6);

   glutSwapBuffers();
}

//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y)
{
   switch (key)
   {
   case 033: // Escape Key
   case 'q':
   case 'Q':
      exit(EXIT_SUCCESS);
      break;
   case 32:
      //spacebar, in case we want it
     
      break;
   }
}

//----------------------------------------------------------------------------

void mouse(int button, int state, int x, int y)
{
   //left button click
   if(button == GLUT_LEFT_BUTTON && !mousePressed){
      //to stop double clicking
      mousePressed = !mousePressed;

   }else if(button == GLUT_LEFT_BUTTON && mousePressed){
      GLfloat mouseX = mouseConvert(x, windowWidth);
      GLfloat mouseY = mouseConvert(y, windowHeight);

      //do something with mouseX and mouseY. 
      //(maybe make a struct to save them? would be cleaner)

      mousePressed = !mousePressed;
   }
   //right button click
   if(button == GLUT_RIGHT_BUTTON && !mousePressed){
      mousePressed = !mousePressed;

   }else if(button == GLUT_RIGHT_BUTTON && mousePressed){
     
      //do something
      mousePressed = !mousePressed;
   }
}

//----------------------------------------------------------------------------

//called every frame
void update(void)
{

}

//----------------------------------------------------------------------------

//called on window resize
void reshape(int width, int height)
{
   glViewport(0, 0, width, height);
   windowHeight = height;
   windowWidth = width;
  
   glm::mat4 projection = glm::ortho(LEFT, RIGHT, BOTTOM, TOP, Z_NEAR, Z_FAR);
   
   glUniformMatrix4fv(Projection, 1, GL_FALSE, glm::value_ptr(projection));
}
//----------------------------------------------------------------------------
//converts screen coordinates to window coordinates
GLfloat mouseConvert(int mouseVal, int windowScale){
   return ((float)mouseVal - (float)windowScale/2.0) / ((float)windowScale/2.0);
}

//rebinds VAO's
 void updateBuffer(){

     // Create and initialize a buffer object
   glGenBuffers(1, &buffer);
   /*########################################################################*/
         // Bind VAO
         glBindVertexArray(VAO);
         // Bind VBO to VAO
         glBindBuffer(GL_ARRAY_BUFFER, buffer);

         glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW);
         // Set vPosition vertex attibute for shader(s)
         glEnableVertexAttribArray(vPosition);
         glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

   /*########################################################################*/

}
//----------------------------------------------------------------------------

bool cmpf(GLfloat a, GLfloat b, GLfloat epsilon){
   return fabs(a - b) < epsilon;
}

//----------------------------------------------------------------------------
// vector field . advection ...