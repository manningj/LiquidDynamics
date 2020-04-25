//----------------------------------------------------------------------------
// NAMES		       : Mark Robitaille
// COURSE		    : COMP 4490
// INSTRUCTOR	    : John Braico
// PROJECT         : Fluid Dynamics Demonstration
//
// REMARKS	: This program shows basic fluid dynamics by utitizing a solution to the
//         Navier Stokes Equations as described in GPU Gems 1, Chapter 38. Our implementation
//         has all of the main parts of the equation implemented, but pressure does
//         behave as expected.
//
// CONTROLS	: Click and drag to add force and color/ink to the fluid in the window.
//         Q or escape key to exit program.
//         0 to 9 keys will select the current color to add to the fluid.
//         Space bar to swap between viewing velocity field (u) or color/ink field.
//			  A key to reset the program to initial state
//			  W key to increase viscosity
//			  S key to decrease viscosity
//			  E key to increase ink color strength
//			  D key to decrease ink color strength
//			  R key to increase ink color dissipation
//			  F key to decrease ink color dissipation
//			  T key to increase ink color radius
//			  G key to decrease ink color radius
//			  Y key to increase velocity radius (value is squared radius)
//			  H key to increase velocity radius (value is squared radius)
//			  Z key to clear velocity field
//			  X key to clear ink color field
//			  C key to clear currently displayed field
//			  
//----------------------------------------------------------------------------

#include "common.h"

const char *WINDOW_TITLE = "Liquid Dynamics";
const double FRAME_RATE_MS = 1000.0 / 60.0;

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
const GLfloat Z_FAR = 1.0f;

const color4 clearColour = color4(0,0,0,1); //black background

GLuint vPosition[7];
GLuint Projection[7];

GLuint VAO;
GLuint buffer;

Shaders temp = {0,0,0,0,0,0,0};
Shaders* shaders = &temp;

float viscosity = 0.7f;
float dt = 1.0f/60.0f;
Pair Velocity, Pressure, Ink;
Field Divergence;

bool showVelocity = false;

// Color settings when adding ink/color
float inkDis = inkDissipation;
float inkRadius = 25.0f;
color3 inkColors[10] = {
   color3(1.0,1.0,1.0), // White
   color3(1.0,0.0,0.0), // Red
   color3(1.0,0.5,0.0), // Orange
   color3(1.0,1.0,0.0), // Yellow
   color3(0.0,1.0,0.0), // Green
   color3(0.0,1.0,1.0), // Light Blue
   color3(0.0,0.0,1.0), // Blue
   color3(0.5,0.2,1.0), // Purple
   color3(1.0,0.6,0.8), // Pink
   color3(0.8,0.7,0.6), // Beige
};
float inkStrength = 0.15;
int selectedColor = 0;

// Velocity settings when adding force
const float VELOCITY_SCALE = 10.0f; // Lower number increases intensity of force
float veloDis = 1.0f;
float forceRadius = 500.0f; // Affects radius of sqrt this value

//--------------------------------------------------------------------

// Coordinates used for drawing and adding force/ink

 point4 verticesSquare[6] = {
   point4(-1.0,  1.0,  0.0, 1.0),
   point4(-1.0, -1.0,  0.0, 1.0),
   point4(1.0, -1.0,  0.0, 1.0),
   point4(-1.0,  1.0, 0.0, 1.0),
   point4(1.0, -1.0,  0.0, 1.0),
   point4(1.0,  1.0,  0.0, 1.0)
 };

point4 verticesLine[2]{
    point4(0.0,  0.0,  0.0, 1.0),
    point4(0.0,  0.0,  0.0, 1.0),
};

//----------------------------------------------------------------------------

// OpenGL initialization

void init()
{
   initFields(); // Set up fields and initialize all shaders
  
   printf("INIT\n");

   // Grab the vertex attributes and projection uniforms
   vPosition[0] = glGetAttribLocation(shaders->drawTexture, "vPosition");
   vPosition[1] = glGetAttribLocation(shaders->advect, "vPosition");
   vPosition[2] = glGetAttribLocation(shaders->jacobi, "vPosition");
   vPosition[3] = glGetAttribLocation(shaders->divergence, "vPosition");
   vPosition[4] = glGetAttribLocation(shaders->subtractGradient, "vPosition");
   vPosition[5] = glGetAttribLocation(shaders->addedForce, "vPosition");
   vPosition[6] = glGetAttribLocation(shaders->addedInk, "vPosition");

   Projection[0] = glGetUniformLocation(shaders->drawTexture, "Projection");
   Projection[1] = glGetUniformLocation(shaders->advect, "Projection");
   Projection[2] = glGetUniformLocation(shaders->jacobi, "Projection");
   Projection[3] = glGetUniformLocation(shaders->divergence, "Projection");
   Projection[4] = glGetUniformLocation(shaders->subtractGradient, "Projection");
   Projection[5] = glGetUniformLocation(shaders->addedForce, "Projection");   
   Projection[6] = glGetUniformLocation(shaders->addedInk, "Projection");   
 
   // Set up the VAO/VBO
   updateBuffer();

   glEnable(GL_DEPTH_TEST);

   glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
}

//----------------------------------------------------------------------------

void display(void)
{
   runtime(); // Solve the Navier Stokes Equation
      
   GLuint shaderHandle = shaders->drawTexture;

   // Clear output framebuffer
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Draw texture
   glUseProgram(shaderHandle);
   glBindVertexArray(VAO); // Drawing square
   glBindBuffer(GL_ARRAY_BUFFER, buffer);

   // Set uniforms
   GLint scale = glGetUniformLocation(shaderHandle, "Scale");
   glUniform2f(scale, 1.0f / (windowWidth), 1.0f / (windowHeight));

   // Bind selected texture and draw
   glActiveTexture(GL_TEXTURE0);

   if (showVelocity) {
      glBindTexture(GL_TEXTURE_2D, Velocity.foo.texture);
   } else {
      glBindTexture(GL_TEXTURE_2D, Ink.foo.texture);
   }
   
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
   case ' ': //spacebar
      showVelocity = !showVelocity;
      printf("Changing views between velocity and ink.\n");
      break;
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
      selectedColor = key - '0';
      break;
   case 'a':
      showVelocity = false;
      viscosity = 0.7f;
      inkDis = inkDissipation;
      inkStrength = 0.15f;
      inkRadius = 25.0f;
      forceRadius = 500.0f;
      clearField(Velocity.foo, 0.5);
      clearField(Ink.foo, 0.0);
      printf("Settings reset.\n");
      break;
   case 'w':
      viscosity += 0.1;
      printf("Viscosity: %f\n", viscosity);
      break;
   case 's':
      viscosity = glm::max(0.1, viscosity - 0.1);
      printf("Viscosity: %f\n", viscosity);
      break;
   case 'e':
      inkStrength += 0.05;
      printf("Ink Strength: %f\n", inkStrength);
      break;
   case 'd':
      inkStrength = glm::max(0.0, inkStrength - 0.05);
      printf("Ink Strength: %f\n", inkStrength);
      break;
   case 'r':
      inkDis += 0.01;
      printf("Ink Dissipation: %f\n", inkDis);
      break;
   case 'f':
      inkDis = glm::max(0.0, inkDis - 0.01);
      printf("Ink Dissipation: %f\n", inkDis);
      break;
   case 't':
      inkRadius += 10.0f;
      printf("Ink Radius: %f\n", inkRadius);
      break;
   case 'g':
      inkRadius = glm::max(0.0, inkRadius - 10.0);
      printf("Ink Radius: %f\n", inkRadius);
      break;
   case 'y':
      forceRadius += 100.0f;
      printf("Velocity Radius (note: essentially squared radius): %f\n", forceRadius);
      break;
   case 'h':
      forceRadius = glm::max(0.0, forceRadius - 100.0);
      printf("Velocity Radius (note: essentially squared radius): %f\n", forceRadius);
      break;
   case 'z':
      clearField(Velocity.foo, 0.5);
      printf("Cleared velocity.\n");
      break;
   case 'x':
      clearField(Ink.foo, 0.0);
      printf("Cleared ink.\n");
      break;
   case 'c':
      if (showVelocity) {
         clearField(Velocity.foo, 0.5);
         printf("Cleared velocity.\n");
      } else {
         clearField(Ink.foo, 0.0);
         printf("Cleared ink.\n");
      }
      break;
   }
}

//----------------------------------------------------------------------------

void mouse(int button, int state, int x, int y)
{
   //left button click
   if(button == GLUT_LEFT_BUTTON) {
      // Get initial point for adding force/ink
      verticesLine[0] = point4((float)x-1.0, (float)windowHeight -1.0 -(float)y, 0.0, 1.0);
   }
   //right button click
   if(button == GLUT_RIGHT_BUTTON) {
      // Get initial point for adding force/ink
       verticesLine[0] = point4((float)x-1.0, (float)windowHeight -1.0 -(float)y, 0.0, 1.0);
   }
}

//----------------------------------------------------------------------------

void mouseDrag(int x, int y) { // Add force
   verticesLine[1] = point4((float)x-1.0, (float)windowHeight- 1.0  -(float)y, 0.0, 1.0);
   // Add ink and force
   addedInk(Ink.foo, Ink.bar);
   swapField(&Ink);
   addedForce(Velocity.foo, Velocity.bar);
   swapField(&Velocity);
   verticesLine[0] = verticesLine[1]; // Start point for next force
}

//----------------------------------------------------------------------------

void update(void)
{ }

//----------------------------------------------------------------------------

//called on window resize
void reshape(int width, int height)
{
   printf("Viewport set to %d, %d\n", windowWidth, windowHeight);
   glViewport(0, 0, windowWidth, windowHeight);
   
   // Set the orthoprojection matrix
   glm::mat4 projection = glm::ortho(LEFT, RIGHT, BOTTOM, TOP, Z_NEAR, Z_FAR);
   
   // Set the projection uniforms to all of the shaders
   int i = 0;
   glUseProgram(shaders->drawTexture);
   glUniformMatrix4fv(Projection[i++], 1, GL_FALSE, glm::value_ptr(projection));
  
   glUseProgram(shaders->advect);
   glUniformMatrix4fv(Projection[i++], 1, GL_FALSE, glm::value_ptr(projection));

   glUseProgram(shaders->jacobi);
   glUniformMatrix4fv(Projection[i++], 1, GL_FALSE, glm::value_ptr(projection));

   glUseProgram(shaders->divergence);
   glUniformMatrix4fv(Projection[i++], 1, GL_FALSE, glm::value_ptr(projection));

   glUseProgram(shaders->subtractGradient);
   glUniformMatrix4fv(Projection[i++], 1, GL_FALSE, glm::value_ptr(projection));

   glUseProgram(shaders->addedForce);
   glUniformMatrix4fv(Projection[i++], 1, GL_FALSE, glm::value_ptr(projection));

   glUseProgram(shaders->addedInk);
   glUniformMatrix4fv(Projection[i], 1, GL_FALSE, glm::value_ptr(projection));
}
//----------------------------------------------------------------------------
//converts screen coordinates to window coordinates
GLfloat mouseConvert(int mouseVal, int windowScale){
   return ((float)mouseVal - (float)windowScale/2.0) / ((float)windowScale/2.0);
}

//----------------------------------------------------------------------------
//converts screen coordinates to window coordinates
point2 mouseConvert(int mouseValX, int mouseValY, int windowScaleX, int windowScaleY) {
   return point2(((float)mouseValX - (float)windowScaleX/2.0) / ((float)windowScaleX/2.0), -((float)mouseValY - (float)windowScaleY / 2.0) / ((float)windowScaleY / 2.0));
}

//rebinds VAO's
 void updateBuffer(){

   glGenVertexArrays(1, &VAO);
     // Create and initialize a buffer object
   glGenBuffers(1, &buffer);
   /*########################################################################*/
         // Bind VAO
         glBindVertexArray(VAO);
         // Bind VBO to VAO
         glBindBuffer(GL_ARRAY_BUFFER, buffer);

         glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSquare), verticesSquare, GL_STATIC_DRAW);

         // Set vPosition vertex attibute for shader(s)
         assignAttrib();

   /*########################################################################*/
}

void assignAttrib(){
   int i = 0;

   glUseProgram(shaders->drawTexture);

      glEnableVertexAttribArray(vPosition[i]);
      glVertexAttribPointer(vPosition[i++], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

   glUseProgram(shaders->advect);

      glEnableVertexAttribArray(vPosition[i]);
      glVertexAttribPointer(vPosition[i++], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

   glUseProgram(shaders->jacobi);

      glEnableVertexAttribArray(vPosition[i]);
      glVertexAttribPointer(vPosition[i++], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

   glUseProgram(shaders->divergence);

      glEnableVertexAttribArray(vPosition[i]);
      glVertexAttribPointer(vPosition[i++], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

   glUseProgram(shaders->subtractGradient);

      glEnableVertexAttribArray(vPosition[i]);
      glVertexAttribPointer(vPosition[i++], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

   glUseProgram(shaders->addedForce);

      glEnableVertexAttribArray(vPosition[i]);
      glVertexAttribPointer(vPosition[i++], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

   glUseProgram(shaders->addedInk);

      glEnableVertexAttribArray(vPosition[i]);
      glVertexAttribPointer(vPosition[i++], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
}
//----------------------------------------------------------------------------

bool cmpf(GLfloat a, GLfloat b, GLfloat epsilon){
   return fabs(a - b) < epsilon;
}

//----------------------------------------------------------------------------

void runtime(){
   // GPU Gems 1 Chapter 38 pseudocode:
   //
   // "Apply the first 3 operators in Equation 12.
   // u = advect(u);
   // u = diffuse (u) ;
   // u = addForces(u);
   // Now apply the projection operator to the result.
   // p = computePressure(u);
   // u = subtractPressureGradient(u, p);"

   float v = viscosity;

   float  diffusionAlpha =(cellSize*cellSize) /(v*dt);
   float  diffusionBeta = (4 + (cellSize*cellSize) /(v*dt));

   float  pressureAlpha = (-cellSize*cellSize);
   float  pressureBeta = 4.0f;

   // Advect velocity
   advect(Velocity.foo, Velocity.foo, Velocity.bar, veloDis, false);
   swapField(&Velocity);

   // Advent the ink based on the velocity
   advect(Velocity.foo, Ink.foo, Ink.bar, inkDis, true);
   swapField(&Ink);

   // Calculate viscous diffusion
   for(int i = 0; i < jacobiIterations; ++i){
      jacobi(Velocity.foo, Velocity.foo, Velocity.bar, true, diffusionAlpha,diffusionBeta);
      swapField(&Velocity);
   }

   // Find divergence
   divergence(Velocity.foo, Divergence);
   
   clearField(Pressure.foo, 0.0);

   // Apply divergence on pressure field
   for(int i = 0; i < jacobiIterations/2; ++i){
      jacobi(Pressure.foo, Divergence, Pressure.bar, false, pressureAlpha,pressureBeta);
      swapField(&Pressure);
   }

   // Subtract the gradient from velocity to get final velocity field
   subtractGradient(Velocity.foo, Pressure.foo, Velocity.bar);
   swapField(&Velocity);
}

void initFields(){
      std::cout << "-> init fields started" << "\n";

   Velocity = createPair(fieldWidth, fieldHeight);
      std::cout << "-> init Velocity complete"<< "\n";
   clearField(Velocity.foo, 0.5);

   Pressure = createPair(fieldWidth, fieldHeight);
      std::cout << "-> init Pressure complete"<< "\n";
      clearField(Pressure.foo, 0.2);

   Ink = createPair(fieldWidth, fieldHeight);
      std::cout << "-> init Ink complete"<< "\n";
      clearField(Ink.foo, 0.0);

   Divergence = createField(fieldWidth, fieldHeight);
      std::cout << "-> init Divergence complete"<< "\n";

   initShaders(shaders);
      std::cout << "-> init shaders complete"<< "\n";
}

void unbind() {
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
}

void addedForce(Field velocity, Field destination) {

   GLuint shaderHandle = shaders->addedForce;
   
   glUseProgram(shaderHandle);

   GLuint newForce = glGetUniformLocation(shaderHandle, "NewForce");
   GLuint timeStep = glGetUniformLocation(shaderHandle, "TimeStep");
   GLuint impulseRadius = glGetUniformLocation(shaderHandle, "ImpulseRadius");
   GLuint impulsePosition = glGetUniformLocation(shaderHandle, "ImpulsePosition");
   GLuint scale = glGetUniformLocation(shaderHandle, "Scale");
   GLuint interiorRangeMin = glGetUniformLocation(shaderHandle, "InteriorRangeMin");
   GLuint interiorRangeMax = glGetUniformLocation(shaderHandle, "InteriorRangeMax");

   glUniform2f(newForce, ((float)verticesLine[1].x - (float)verticesLine[0].x)/ VELOCITY_SCALE, ((float)verticesLine[1].y - (float)verticesLine[0].y) / VELOCITY_SCALE);
   glUniform1f(timeStep, dt);
   glUniform1f(impulseRadius, forceRadius);
   glUniform2f(impulsePosition, verticesLine[0].x, verticesLine[0].y);
   glUniform2f(scale, 1.0f / (fieldWidth), 1.0f / (fieldHeight));
   glUniform2f(interiorRangeMin, interiorRangeMinX, interiorRangeMinY);
   glUniform2f(interiorRangeMax, interiorRangeMaxX, interiorRangeMaxY);

   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, velocity.texture);

   glBindVertexArray(VAO);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glDrawArrays(GL_TRIANGLES, 0, 6);;
   unbind();
}

void advect(Field velocity, Field position, Field destination, float dissipationVal, bool advectingInk) {
   
   GLuint shaderHandle = shaders->advect;

   glUseProgram(shaderHandle);

   GLuint scale = glGetUniformLocation(shaderHandle, "Scale");
   GLuint timeStep = glGetUniformLocation(shaderHandle, "timeStep");
   GLuint dissipation = glGetUniformLocation(shaderHandle, "dissipation");
   GLuint isInk = glGetUniformLocation(shaderHandle, "isInk");
   GLuint interiorRangeMin = glGetUniformLocation(shaderHandle, "InteriorRangeMin");
   GLuint interiorRangeMax = glGetUniformLocation(shaderHandle, "InteriorRangeMax");

   GLuint posTex = glGetUniformLocation(shaderHandle, "posTex");

   glUniform2f(scale, 1.0f / (fieldWidth), 1.0f / (fieldHeight)); // rdx is 1/dx and dy

   glUniform1f(timeStep, dt);
   glUniform1f(dissipation, dissipationVal);
   glUniform1i(isInk, advectingInk);
   glUniform2f(interiorRangeMin, interiorRangeMinX, interiorRangeMinY);
   glUniform2f(interiorRangeMax, interiorRangeMaxX, interiorRangeMaxY);

   glUniform1i(posTex, 1); // texture 1, a sampler2D.
   // bind framebuffer to the destination field
   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   // set texture 0 to be the velocity field
   
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, velocity.texture);

   //set texture one to be the position texture
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, position.texture);

   //use the shaders
   glDrawArrays(GL_TRIANGLES, 0, 6);
   //unbind everything
   unbind();
}

void jacobi(Field xField, Field bField, Field destination, bool isVelo, float alphaParameter, float betaParameter){
   /* This gets called a number of times in a loop. 
      Used for poisson pressure and for viscous diffusion.

   For pressure, alpha = -(dx^2)
                rbeta = 1/4
                x = pressure
                b = divergence

   For diffusion, alpha = dx^2 /v*dt 
                rbeta = 1/(4 + dx^2 /v*dt)
                x = velocity
                d = velocity
   */

   GLuint shaderHandle = shaders->jacobi;

   glUseProgram(shaderHandle);

   GLuint alpha = glGetUniformLocation(shaderHandle, "alpha");
   GLuint rBeta = glGetUniformLocation(shaderHandle, "rBeta");
   GLuint isVeloPtr = glGetUniformLocation(shaderHandle, "Velocity");
   GLuint interiorRangeMin = glGetUniformLocation(shaderHandle, "InteriorRangeMin");
   GLuint interiorRangeMax = glGetUniformLocation(shaderHandle, "InteriorRangeMax");

   GLuint b = glGetUniformLocation(shaderHandle, "b");

   glUniform1f(alpha, alphaParameter);
   glUniform1f(rBeta, 1.0f/betaParameter);
   glUniform1f(isVeloPtr, isVelo);
   glUniform2f(interiorRangeMin, interiorRangeMinX, interiorRangeMinY);
   glUniform2f(interiorRangeMax, interiorRangeMaxX, interiorRangeMaxY);
   glUniform1i(b, 1);

   // bind frame buffer to the destination field
   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   
   // set texture 0 to be the x field
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, xField.texture);

   // set texture one to be the b texture
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, bField.texture);

   // use the shaders
   glDrawArrays(GL_TRIANGLES, 0, 6);
   // unbind everything
   unbind();
}


void subtractGradient(Field velocityField, Field pressureField, Field destination){

   GLuint shaderHandle = shaders->subtractGradient;

   glUseProgram(shaderHandle);

  // GLuint velocity=  glGetUniformLocation(shaderHandle, "velocity");
   GLuint pressure=  glGetUniformLocation(shaderHandle, "pressure");
   GLuint gradScale=  glGetUniformLocation(shaderHandle, "gradScale");
   GLuint interiorRangeMin = glGetUniformLocation(shaderHandle, "InteriorRangeMin");
   GLuint interiorRangeMax = glGetUniformLocation(shaderHandle, "InteriorRangeMax");

   //glUniform1f(gradScale, 0.5/(fieldWidth));
   glUniform1f(gradScale, 0.5/(float)cellSize);
   glUniform1i(pressure, 1);
   glUniform2f(interiorRangeMin, interiorRangeMinX, interiorRangeMinY);
   glUniform2f(interiorRangeMax, interiorRangeMaxX, interiorRangeMaxY);

    //bindframe buffer to the destination field
   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   
   //set texture 0 to be the velocity field
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, velocityField.texture);

   //set texture one to be the pressure field texture
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, pressureField.texture);

   //use the shaders
   glDrawArrays(GL_TRIANGLES, 0, 6);
   //unbind everything
   unbind();

}

void divergence(Field velocityField, Field destination){
   GLuint shaderHandle = shaders->divergence;
   glUseProgram(shaderHandle);

   GLuint halfrdx=  glGetUniformLocation(shaderHandle, "halfrdx");
   GLuint interiorRangeMin = glGetUniformLocation(shaderHandle, "InteriorRangeMin");
   GLuint interiorRangeMax = glGetUniformLocation(shaderHandle, "InteriorRangeMax");

   // glUniform1f(halfrdx, 0.5f/(float)fieldWidth); 
   // Textbook wording makes it sound like above but it's only 0.00078125 at width of 640
   // 0.5/cellSize gives a more reasonable value
   glUniform1f(halfrdx, 0.5f/(float)cellSize);
   glUniform2f(interiorRangeMin, interiorRangeMinX, interiorRangeMinY);
   glUniform2f(interiorRangeMax, interiorRangeMaxX, interiorRangeMaxY);

   // bind frame buffer to the destination field
   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   
   // set texture 0 to be the velocity field
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, velocityField.texture);
   
   // use the shaders
   glDrawArrays(GL_TRIANGLES, 0, 6);
   // unbind everything
   unbind();
}

void addedInk(Field canvas, Field destination) {
   GLuint shaderHandle = shaders->addedInk;
   
   glUseProgram(shaderHandle);

   GLuint newInk = glGetUniformLocation(shaderHandle, "NewInk");
   GLuint inkRadiusPtr = glGetUniformLocation(shaderHandle, "InkRadius");
   GLuint inkStrengthPtr = glGetUniformLocation(shaderHandle, "InkStrength");
   GLuint inkPosition = glGetUniformLocation(shaderHandle, "InkPosition");
   GLuint scale = glGetUniformLocation(shaderHandle, "Scale");
   GLuint interiorRangeMin = glGetUniformLocation(shaderHandle, "InteriorRangeMin");
   GLuint interiorRangeMax = glGetUniformLocation(shaderHandle, "InteriorRangeMax");

   glUniform3f(newInk, inkColors[selectedColor].r, inkColors[selectedColor].g, inkColors[selectedColor].b);
   glUniform1f(inkRadiusPtr, inkRadius);
   glUniform1f(inkStrengthPtr, inkStrength);
   glUniform2f(inkPosition, verticesLine[0].x, verticesLine[0].y);
   glUniform2f(scale, 1.0f / (fieldWidth), 1.0f / (fieldHeight));
   glUniform2f(interiorRangeMin, interiorRangeMinX, interiorRangeMinY);
   glUniform2f(interiorRangeMax, interiorRangeMaxX, interiorRangeMaxY);

   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, canvas.texture);

   glBindVertexArray(VAO);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glDrawArrays(GL_TRIANGLES, 0, 6);
   unbind();
}