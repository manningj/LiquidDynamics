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
void updateBuffer(int index);
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

point4 verticesSquare[6] = {
  point4(-1.0,  1.0,  0.0, 1.0),
  point4(-1.0, -1.0,  0.0, 1.0),
  point4(1.0, -1.0,  0.0, 1.0),
  point4(-1.0,  1.0, 0.0, 1.0),
  point4(1.0, -1.0,  0.0, 1.0),
  point4(1.0,  1.0,  0.0, 1.0)
};

point4 verticesLine[2] = {
    point4(0.0,  0.0,  0.0, 1.0),
    point4(0.0, 0.0,  0.0, 1.0),
};

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

GLuint program[2];
GLuint VAO[2];
GLuint buffer[2];
GLuint vPosition[2];
GLuint Projection[2];

Pair addedForceFields;

enum programs {drawTexture, addedForce};

//----------------------------------------------------------------------------

// OpenGL initialization

void init()
{
   
   program[0] = InitShader("vshader.glsl", "drawTexture.glsl");
   program[1] = InitShader("vshader.glsl", "addedForce.glsl");

   glUseProgram(program[0]);

   // Set up draw texture program
   vPosition[0] = glGetAttribLocation(program[0], "vPosition");
   glGenVertexArrays(1, &VAO[0]);
   updateBuffer(0);
   Projection[0] = glGetUniformLocation(program[0], "Projection");
   
   // Set up added force program
   glUseProgram(program[1]);
   vPosition[1] = glGetAttribLocation(program[1], "vPosition");
   glGenVertexArrays(1, &VAO[1]);
   updateBuffer(1);
   Projection[1] = glGetUniformLocation(program[1], "Projection");

   addedForceFields = createPair(windowWidth, windowHeight);
   clearField(addedForceFields.foo, 1.0);

   glEnable(GL_DEPTH_TEST);

   glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
}

//----------------------------------------------------------------------------

void display(void)
{
   // Clear output framebuffer
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Draw texture
   glUseProgram(program[0]);
   glBindVertexArray(VAO[0]);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

   // Set uniforms
   GLint fillColor = glGetUniformLocation(program[0], "FillColor");
   GLint scale = glGetUniformLocation(program[0], "Scale");
   glUniform3f(fillColor, 1, 1, 1);
   glUniform2f(scale, 1.0f / windowWidth, 1.0f / windowHeight);

   // Bind texture and draw
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, addedForceFields.foo.texture);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
   glDrawArrays(GL_TRIANGLES, 0, 6);
   glBindTexture(GL_TEXTURE_2D, 0);

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
      //swapField(&temp);
      break;
   }
}

//----------------------------------------------------------------------------

void mouse(int button, int state, int x, int y)
{
   //left button click
   if(button == GLUT_LEFT_BUTTON) {
      // Get initial point for adding force
      verticesLine[0] = point4(mouseConvert(x, windowWidth), -mouseConvert(y, windowHeight), 0, 1.0);
   }
   //right button click
   if(button == GLUT_RIGHT_BUTTON) {
       // Add ink if right click?
   }
}

//----------------------------------------------------------------------------

void mouseDrag(int x, int y) { // Add force
   point4 converted = point4(mouseConvert(x, windowWidth), -mouseConvert(y, windowHeight), 0, 1.0);
   verticesLine[1] = converted;
   glBindFramebuffer(GL_FRAMEBUFFER, addedForceFields.foo.fbo);
   glUseProgram(program[1]);
   glBindVertexArray(VAO[1]);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLine), verticesLine, GL_DYNAMIC_DRAW);
   glVertexAttribPointer(vPosition[1], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
   GLint newForce = glGetUniformLocation(program[1], "NewForce");
   glUniform2f(newForce, verticesLine[1].x - verticesLine[0].x, verticesLine[1].y - verticesLine[0].y);
   glDrawArrays(GL_LINES, 0, 2);
   verticesLine[0] = converted; // Start point for next force
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
   
   glUseProgram(program[0]);
   glUniformMatrix4fv(Projection[0], 1, GL_FALSE, glm::value_ptr(projection));
   glUseProgram(program[1]);
   glUniformMatrix4fv(Projection[1], 1, GL_FALSE, glm::value_ptr(projection));
}
//----------------------------------------------------------------------------
//converts screen coordinates to window coordinates
GLfloat mouseConvert(int mouseVal, int windowScale){
   return ((float)mouseVal - (float)windowScale/2.0) / ((float)windowScale/2.0);
}

//----------------------------------------------------------------------------
//converts screen coordinates to window coordinates
point2 mouseConvert(int mouseValX, int mouseValY, int windowScaleX, int windowScaleY){
   return point2(((float)mouseValX - (float)windowScaleX/2.0) / ((float)windowScaleX/2.0), -((float)mouseValY - (float)windowScaleY / 2.0) / ((float)windowScaleY / 2.0));
}

//rebinds VAO's
 void updateBuffer(int index){

     // Create and initialize a buffer object
   glGenBuffers(1, &buffer[index]);
   /*########################################################################*/
         // Bind VAO
         glBindVertexArray(VAO[index]);
         // Bind VBO to VAO
         glBindBuffer(GL_ARRAY_BUFFER, buffer[index]);
         if (index == 0) {
             glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSquare), verticesSquare, GL_STATIC_DRAW);
         }
         else if (index == 1) {
             glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLine), verticesLine, GL_STATIC_DRAW);
         }
         
         // Set vPosition vertex attibute for shader(s)
         glEnableVertexAttribArray(vPosition[index]);
         glVertexAttribPointer(vPosition[index], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

   /*########################################################################*/

}
//----------------------------------------------------------------------------

bool cmpf(GLfloat a, GLfloat b, GLfloat epsilon){
   return fabs(a - b) < epsilon;
}

//----------------------------------------------------------------------------
// vector field . advection ...