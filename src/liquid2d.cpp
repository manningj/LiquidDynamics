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
float mouseConvert(float mouseVal, float windowScale);
void updateBuffers();
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

const color4 clearColour = color4(0,0,0,1); //black backround
//--------------------------------------------------------------------

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

GLuint Projection, Colour;
GLuint VAOs[2];
GLuint buffers[2];
GLuint program, vPosition;

Shaders* shaders;


float viscosity;
const float dt;
Pair Velocity, Pressure, Density;
Field Divergence;

GLuint Display;
//----------------------------------------------------------------------------

// OpenGL initialization

std::vector<point4> vertices;
void init()
{
   program = InitShader("vshader.glsl", "fshader.glsl");
   glUseProgram(program);

   vPosition = glGetAttribLocation(program, "vPosition");
   
   glGenVertexArrays(2, VAOs);


   updateBuffers();
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
   
  
   glm::mat4 projection = glm::ortho(LEFT, RIGHT, BOTTOM, TOP, Z_NEAR, Z_FAR);
   
   glUniformMatrix4fv(Projection, 1, GL_FALSE, glm::value_ptr(projection));
}
//----------------------------------------------------------------------------
//converts screen coordinates to window coordinates
GLfloat mouseConvert(GLfloat mouseVal, GLfloat windowScale){
   return (mouseVal - windowScale/2) / (windowScale/2);
}

//rebinds VAO's
 void updateBuffers(){

   // Create a vertex array object
   glGenBuffers(2, buffers);
   /*########################################################################*/

         glBindVertexArray(VAOs[0]);
         // Create and initialize a buffer object

      
         glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
         glBufferData(GL_ARRAY_BUFFER, sizeof(point4) * vertices.size(),
                                       vertices.data(), GL_DYNAMIC_DRAW);
         
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
void runtime(){

}
void initFields(){

   Velocity = createPair(windowWidth, windowHeight);
   Pressure = createPair(windowWidth, windowHeight);

   Divergence = createField(windowWidth, windowHeight);
   initShaders(shaders);
   Display = InitShader("vshader.glsl", "fshader.glsl");
}

void unbind(){

   //after each call to a shader program, 
   //we should remove the shader specific bindings.
   //we do this by bindining them to "0";
   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_2D, 0); //unbind tex2
   
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, 0); // unbind tex1

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, 0); //unbind tex0

   glBindFramebuffer(GL_FRAMEBUFFER,0); //unbind framebuffer
   glDisable(GL_BLEND);


}
void advect(Field velocity, Field position, Field destination){
   
   GLuint shaderHandle = shaders->advect;

   glUseProgram(shaderHandle);

   GLuint rdx = glGetUniformLocation(shaderHandle, "rdx");
   GLuint timeStep = glGetUniformLocation(shaderHandle, "timeStep");
   GLuint veloTex = glGetUniformLocation(shaderHandle, "veloTex");
   GLuint posTex = glGetUniformLocation(shaderHandle, "posTex");


   glUniform2f(rdx, 1.0f / cellSize, 1.0f / cellSize); // rdx is 1/dx and dy
   glUniform1f(timeStep, dt);
   glUniform1i(posTex, 1); // texture 1, a sampler2D.
   
   //bindframe buffer to the destination field
   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   //set texture 0 to be the velocity field
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, velocity.texture);

   //set texture one to be the position texture
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, position.texture);
   //obstacles stuff here
   
   //use the shaders
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
   //unbind everything
   unbind();
}

void jacobi(Field xField, Field bField, Field destination, float alphaParameter, float betaParameter){

   /*this gets called a number of times in a loop. 
      used for poisson pressure,
         and for  viscous diffusion.


   for pressure, alpha = -(dx^2)
                rbeta = 1/4
                x = pressure
                b = divergence

   for diffusion, alpha = dx^2 /v*dt 
                rbeta = 1/(4 + dx^2 /v*dt)
                x = velocity
                d = velocity
   */

   GLuint shaderHandle = shaders->jacobi;

   glUseProgram(shaderHandle);

   GLuint alpha = glGetUniformLocation(shaderHandle, "alpha");
   GLuint rBeta = glGetUniformLocation(shaderHandle, "rBeta");

   GLuint x = glGetUniformLocation(shaderHandle, "x");
   GLuint b = glGetUniformLocation(shaderHandle, "b");

   glUniform1f(alpha, alphaParameter);
   glUniform1f(rBeta, 1/betaParameter);
   glUniform1i(b, 1);


    //bindframe buffer to the destination field
   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   
   //set texture 0 to be the x field
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, xField.texture);

   //set texture one to be the b texture
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, bField.texture);
   //obstacles stuff here
   
   //use the shaders
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
   //unbind everything
   unbind();
}


void subtractGradient(Field velocityField, Field pressureField, Field destination){

   GLuint shaderHandle = shaders->subtractGradient;

   glUseProgram(shaderHandle);

   GLuint velocity=  glGetUniformLocation(shaderHandle, "velocity");
   GLuint pressure=  glGetUniformLocation(shaderHandle, "pressure");
   GLuint gradScale=  glGetUniformLocation(shaderHandle, "gradScale");


   glUniform1f(gradScale, 1/fieldWidth);
   glUniform1i(pressure, 1);

   

    //bindframe buffer to the destination field
   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   
   //set texture 0 to be the velocity field
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, velocityField.texture);

   //set texture one to be the pressure field texture
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, pressureField.texture);
   //obstacles stuff here
   
   //use the shaders
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
   //unbind everything
   unbind();

}

void divergence(Field velocityField, Field destination){
   GLuint shaderHandle = shaders->divergence;
   glUseProgram(shaderHandle);

   GLuint velocity=  glGetUniformLocation(shaderHandle, "velocity");
   GLuint halfrdx=  glGetUniformLocation(shaderHandle, "halfrdx");


   glUniform1f(halfrdx, 0.5f/cellSize);

   //bindframe buffer to the destination field
   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   
   //set texture 0 to be the velocity field
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, velocityField.texture);
   
   //use the shaders
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
   //unbind everything
   unbind();


    
}

