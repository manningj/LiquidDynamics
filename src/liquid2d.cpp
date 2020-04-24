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
const GLfloat LEFT = -0.5f;
const GLfloat RIGHT = 639.5f;
const GLfloat TOP = -0.5f;
const GLfloat BOTTOM = 639.5f;
const GLfloat Z_NEAR = -1.0f;
const GLfloat Z_FAR  = 1.0f;
//boolean used for double clicking
bool mousePressed = false;

const color4 clearColour = color4(0,0,0,1); //black background
//--------------------------------------------------------------------

// point4 verticesSquare[6] = {
//   point4(-1.0,  1.0,  0.0, 1.0),
//   point4(-1.0, -1.0,  0.0, 1.0),
//   point4(1.0, -1.0,  0.0, 1.0),
//   point4(-1.0,  1.0, 0.0, 1.0),
//   point4(1.0, -1.0,  0.0, 1.0),
//   point4(1.0,  1.0,  0.0, 1.0)
// };

// point4 verticesLine[2]{
//     point4(0.0,  0.0,  0.0, 1.0),
//     point4(0.0,  0.0,  0.0, 1.0),
// };

// point4 verticesBoundary[6] = {
//   point4(-1.0,  1.0,  0.1, 1.0),
//   point4(-1.0, -1.0,  0.1, 1.0),
//   point4(1.0, -1.0,  0.1, 1.0),
//   point4(-1.0,  1.0, 0.1, 1.0),
//   point4(1.0, -1.0,  0.1, 1.0),
//   point4(1.0,  1.0,  0.1, 1.0)
// };



//point4 verticesBoundary[8] = {
//    point4(-1.0,  1.0,  0.0, 1.0),
//    point4(-1.0,  -1.0,  0.0, 1.0),
//
//    point4(-1.0,  -1.0,  0.0, 1.0),
//    point4(1.0,  -1.0,  0.0, 1.0),
//
//    point4(1.0,  -1.0,  0.0, 1.0),
//    point4(1.0,  1.0,  0.0, 1.0),
//
//    point4(1.0,  1.0,  0.0, 1.0),
//    point4(-1.0,  1.0,  0.0, 1.0),
//};




point4 verticesSquare[6] = {
  point4(1.0,  638.0,  0.0, 1.0),
  point4(1.0, 1.0,  0.0, 1.0),
  point4(638.0, 1.0,  0.0, 1.0),
  point4(1.0,  638.0, 0.0, 1.0),
  point4(638.0, 1.0,  0.0, 1.0),
  point4(638.0,  638.0,  0.0, 1.0)
};

point4 verticesLine[2]{
    point4(0.0,  0.0,  0.0, 1.0),
    point4(0.0,  0.0,  0.0, 1.0),
};

point4 verticesBoundary[6] = {
point4(0.0,  639.0,  0.0, 1.0),
  point4(0.0, 0.0,  0.0, 1.0),
  point4(639.0, 0.0,  0.0, 1.0),
  point4(0.0,  639.0, 0.0, 1.0),
  point4(639.0, 0.0,  0.0, 1.0),
  point4(639.0,  639.0,  0.0, 1.0)

};


// point4(0.0,  639.0,  0.0, 1.0),
//   point4(0.0, 0.0,  0.0, 1.0),
//   point4(639.0, 0.0,  0.0, 1.0),
//   point4(0.0,  639.0, 0.0, 1.0),
//   point4(639.0, 0.0,  0.0, 1.0),
//   point4(639.0,  639.0,  0.0, 1.0)

// point4 verticesBoundary[8] = {
//    point4(0.0,  639.0,  0.0, 1.0),
//    point4(0.0,  0.0,  0.0, 1.0),

//    point4(0.0,  0.0,  0.0, 1.0),
//    point4(639.0,  0.0,  0.0, 1.0),

//    point4(639.0,  0.0,  0.0, 1.0),
//    point4(639.0,  639.0,  0.0, 1.0),

//    point4(639.0,  639.0,  0.0, 1.0),
//    point4(0.0,  639.0,  0.0, 1.0),
//};


//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

// GLuint program[2]; // Not needed since we have shader struct
// 0 is drawTexture, 1 is addedForce -> May need to restructure data structure to handle all shaders?
// GLuint VAO;
// GLuint buffer;
GLuint vPosition[7];
GLuint Projection[7];

// enum programs {drawTexture, addedForce};

// GLuint Projection, Colour;
GLuint VAOs[2];
GLuint buffers[2];
// GLuint program, vPosition;
Shaders temp = {0,0,0,0,0,0,0};
Shaders* shaders = &temp;

float viscosity = 0.5f;
float dt = 1.0f/60.0f;
Pair Velocity, Pressure, Density;
Field Divergence;

// Velocity settings when adding force
const float VELOCITY_SCALE = 10.0f; // Lower number increases intensity of force
float forceRadius = 500.0f; // Affects radius of sqrt this value

float prevTime; // General time (updated per frame)
float prevTimeDrag; // Time when adding forces

//----------------------------------------------------------------------------

// OpenGL initialization

void init()
{
   initFields(); // Set up fields and initialize all shaders
  
   printf("INIT");

   // Set up draw texture program

   vPosition[0] = glGetAttribLocation(shaders->drawTexture, "vPosition");
   vPosition[1] = glGetAttribLocation(shaders->advect, "vPosition");
   vPosition[2] = glGetAttribLocation(shaders->jacobi, "vPosition");
   vPosition[3] = glGetAttribLocation(shaders->divergence, "vPosition");
   vPosition[4] = glGetAttribLocation(shaders->subtractGradient, "vPosition");
   vPosition[5] = glGetAttribLocation(shaders->addedForce, "vPosition");
   vPosition[6] = glGetAttribLocation(shaders->boundaries, "vPosition");


   Projection[0] = glGetUniformLocation(shaders->drawTexture, "Projection");
   Projection[1] = glGetUniformLocation(shaders->advect, "Projection");
   Projection[2] = glGetUniformLocation(shaders->jacobi, "Projection");
   Projection[3] = glGetUniformLocation(shaders->divergence, "Projection");
   Projection[4] = glGetUniformLocation(shaders->subtractGradient, "Projection");
   Projection[5] = glGetUniformLocation(shaders->addedForce, "Projection");  
   Projection[6] = glGetUniformLocation(shaders->boundaries, "Projection");   
 
   

   // Set up added force program
   
   //glGenVertexArrays(2, VAOs);

   updateBuffer();

   glEnable(GL_DEPTH_TEST);

   glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
   //prevTime = std::clock();

}

//----------------------------------------------------------------------------

void display(void)
{
   //dt = (std::clock() - prevTime)/(float)CLOCKS_PER_SEC;
   //prevTime = std::clock();

   runtime();
   
   GLuint shaderHandle = shaders->drawTexture;
   // Clear output framebuffer
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Draw texture
   glUseProgram(shaderHandle);
   glBindVertexArray(VAOs[0]); // Drawing square
   glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);

   // Set uniforms
   GLint scale = glGetUniformLocation(shaderHandle, "Scale");
   glUniform2f(scale, 1.0f / windowWidth, 1.0f / windowHeight);

   // Bind texture and draw

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, Velocity.foo.texture);
   
   
   glDrawArrays(GL_TRIANGLES, 0, 6);


   // DRAW SECOND SQUARE

   // shaderHandle = shaders->boundaries;
   // glUseProgram(shaderHandle);
   // glBindVertexArray(VAOs[1]);
   // glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
   // //GLint fillColor = glGetUniformLocation(program2, "FillColor");
   // //GLint scale = glGetUniformLocation(program2, "Scale");
   // //printf("%d %d\n", fillColor, scale);
   // //glUniform3f(fillColor, 1, 1, 1);
   // //glUniform2f(scale, 1.0f / windowWidth, 1.0f / windowHeight);
   // glBindTexture(GL_TEXTURE_2D, 0);
   // glDrawArrays(GL_LINES, 0, 6);


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
      verticesLine[0] = point4((float)x, (float)windowHeight - 1.0 - (float)y, 0.0, 1.0);
   }
   //right button click
   if(button == GLUT_RIGHT_BUTTON) {
       // Add ink if right click?
   }
}

//----------------------------------------------------------------------------

void mouseDrag(int x, int y) { // Add force
   verticesLine[1] = point4((float)x, (float)windowHeight-1.0-(float)y, 0.0, 1.0);
   printf("%d, %d\n", x, windowHeight - 1 - y);
   addedForce(Velocity.foo, Velocity.bar);
   swapField(&Velocity);
   verticesLine[0] = verticesLine[1]; // Start point for next force
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
   glViewport(0, 0, windowWidth, windowHeight);
   
  
   glm::mat4 projection = glm::ortho(LEFT, RIGHT, BOTTOM, TOP, Z_NEAR, Z_FAR);
   
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


   glUseProgram(shaders->boundaries);
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

   glGenVertexArrays(2, VAOs);
     // Create and initialize a buffer object
   glGenBuffers(2, buffers);
   /*########################################################################*/
         // Bind VAO
         glBindVertexArray(VAOs[0]);
         // Bind VBO to VAO
         glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);

         glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSquare), verticesSquare, GL_STATIC_DRAW);
      assignAttrib();
         
         // glBindVertexArray(VAOs[1]);
         // // Bind VBO to VAO
         // glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);

         // glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBoundary), verticesBoundary, GL_STATIC_DRAW);

         // Set vPosition vertex attibute for shader(s)

         

         
   glUseProgram(shaders->boundaries);
   vPosition[6] = glGetAttribLocation(shaders->boundaries, "vPosition");
//   glGenVertexArrays(1, &VAO2);
   //vPosition[6] = glGetUniformLocation(shaders->boundaries, "Projection");

//   glGenBuffers(1, &buffer2);
   /*########################################################################*/
         // Bind VAO
         glBindVertexArray(VAOs[1]);
         // Bind VBO to VAO
         glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);

         glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBoundary), verticesBoundary, GL_STATIC_DRAW);
         // Set vPosition vertex attibute for shader(s)
         glEnableVertexAttribArray(vPosition[6]);
         glVertexAttribPointer(vPosition[6], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));




         glBindVertexArray(VAOs[0]);
         glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);


   /*########################################################################*/

}

void assignAttrib(){
   int i = 0;
   
   // glBindVertexArray(VAOs[0]);
   // glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);


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

   // glBindVertexArray(VAOs[1]);
   // glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
   
   // glUseProgram(shaders->boundaries);

   //    glEnableVertexAttribArray(vPosition[i]);
   //    glVertexAttribPointer(vPosition[i], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
}
//----------------------------------------------------------------------------

bool cmpf(GLfloat a, GLfloat b, GLfloat epsilon){
   return fabs(a - b) < epsilon;
}

//----------------------------------------------------------------------------

void runtime(){
   // float  diffusionAlpha =(cellSize*cellSize) /(viscosity*dt);
   // float  diffusionBeta = (4 + (cellSize*cellSize) /(viscosity*dt));

   // float  pressureAlpha = (cellSize*cellSize) * -1.0f;
   // float  pressureBeta = 4.0f;

   //advect(Velocity.foo, Velocity.foo,Velocity.bar);
   //swapField(&Velocity);

   // for(int i = 0; i < jacobiIterations; ++i){
   //    jacobi(Velocity.foo, Velocity.foo, Velocity.bar, diffusionAlpha,diffusionBeta);
   //    swapField(&Velocity);
   // }

   // divergence(Velocity.foo, Divergence);
   
   // clearField(Pressure.foo, 0);

   // for(int i = 0; i < jacobiIterations; ++i){
   //    jacobi(Pressure.foo, Divergence, Pressure.bar, pressureAlpha,pressureBeta);
   //    swapField(&Pressure);
   // }

   //  subtractGradient(Velocity.foo, Pressure.foo, Velocity.bar);
   //  swapField(&Velocity);

    boundaries(Velocity.foo, Velocity.foo, true);
    //swapField(&Velocity);

   // boundaries(Pressure.foo, Pressure.bar, false);
   // swapField(&Pressure);

   
   // GPU Gems pseudocode:
   // Apply the first 3 operators in Equation 12.
   // u = advect(u);
   // u = diffuse (u) ;
   // u = addForces(u);
   // Now apply the projection operator to the result.
   // p = computePressure(u);
   // u = subtractPressureGradient(u, p); 

}


void initFields(){
      std::cout << "-> init fields started" << "\n";

   Velocity = createPair(fieldWidth, fieldHeight);
      std::cout << "-> init velocity complete"<< "\n";
   clearField(Velocity.foo, 0.5);

   Pressure = createPair(fieldWidth, fieldHeight);
      std::cout << "-> init Pressure complete"<< "\n";

   Divergence = createField(fieldWidth, fieldHeight);
      std::cout << "-> init Divergence complete"<< "\n";

   initShaders(shaders);
      std::cout << "-> init shaders complete"<< "\n";
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

void addedForce(Field velocity, Field destination) {

   GLuint shaderHandle = shaders->addedForce;
   
   glUseProgram(shaderHandle);

   GLint newForce = glGetUniformLocation(shaderHandle, "NewForce");
   GLuint timeStep = glGetUniformLocation(shaderHandle, "TimeStep");
   GLint impulseRadius = glGetUniformLocation(shaderHandle, "ImpulseRadius");
   GLint impulsePosition = glGetUniformLocation(shaderHandle, "ImpulsePosition");
   GLint scale = glGetUniformLocation(shaderHandle, "Scale");

   glUniform2f(newForce, ((float)verticesLine[1].x - (float)verticesLine[0].x)/ VELOCITY_SCALE, ((float)verticesLine[1].y - (float)verticesLine[0].y) / VELOCITY_SCALE);
   glUniform1f(timeStep, dt);
   glUniform1f(impulseRadius, forceRadius);
   glUniform2f(impulsePosition, verticesLine[0].x, verticesLine[0].y);
   glUniform2f(scale, 1.0f / windowWidth, 1.0f / windowHeight);

   printf("\nnewForce: %f, %f\n", ((float)verticesLine[1].x - (float)verticesLine[0].x) / VELOCITY_SCALE, ((float)verticesLine[1].y - (float)verticesLine[0].y) / VELOCITY_SCALE);
   printf("timeStep: %f,\n", dt);
   printf("impulseRadius: %f\n", forceRadius);
   printf("impulsePosition: %f, %f\n", verticesLine[0].x, verticesLine[0].y);
   printf("scale: %f, %f\n\n", 1.0f / windowWidth, 1.0f / windowHeight);

   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, velocity.texture);

   glBindVertexArray(VAOs[0]);
   glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
   glDrawArrays(GL_TRIANGLES, 0, 6);;
   unbind();
}

void advect(Field velocity, Field position, Field destination){
   
   GLuint shaderHandle = shaders->advect;

   glUseProgram(shaderHandle);

   GLuint scale = glGetUniformLocation(shaderHandle, "Scale");
   GLuint timeStep = glGetUniformLocation(shaderHandle, "timeStep");
   GLuint posTex = glGetUniformLocation(shaderHandle, "posTex");


   glUniform2f(scale, 1.0f / fieldWidth, 1.0f / fieldHeight); // rdx is 1/dx and dy

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
   glDrawArrays(GL_TRIANGLES, 0, 6);
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

   //GLuint x = glGetUniformLocation(shaderHandle, "x");
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
   glDrawArrays(GL_TRIANGLES, 0, 6);
   //unbind everything
   unbind();
}


void subtractGradient(Field velocityField, Field pressureField, Field destination){

   GLuint shaderHandle = shaders->subtractGradient;

   glUseProgram(shaderHandle);

  // GLuint velocity=  glGetUniformLocation(shaderHandle, "velocity");
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
   glDrawArrays(GL_TRIANGLES, 0, 6);
   //unbind everything
   unbind();

}

void divergence(Field velocityField, Field destination){
   GLuint shaderHandle = shaders->divergence;
   glUseProgram(shaderHandle);

   //GLuint velocity=  glGetUniformLocation(shaderHandle, "velocity");
   GLuint halfrdx=  glGetUniformLocation(shaderHandle, "halfrdx");


   glUniform1f(halfrdx, 0.5f/cellSize);

   //bindframe buffer to the destination field
   glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
   
   //set texture 0 to be the velocity field
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, velocityField.texture);
   
   //use the shaders
   glDrawArrays(GL_TRIANGLES, 0, 6);
   //unbind everything
   unbind();


    
}

void boundaries(Field stateField, Field destination, bool isVelo){
   
   GLuint shaderHandle = shaders->boundaries;

   glUseProgram(shaderHandle);

   glBindVertexArray(VAOs[1]);
   glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);


   // GLuint offset =   glGetUniformLocation(shaderHandle, "Offset");
   GLuint scale = glGetUniformLocation(shaderHandle, "Scale");
   // GLuint isVelocity = glGetUniformLocation(shaderHandle, "isVelocity");




   glUniform2f(scale, 1.0f/fieldWidth, 1.0f/fieldHeight);
   // glUniform1i(isVelocity, isVelo);
   
   // glm::ivec2 offsetArray[4] = {

   // glm::ivec2(1, 0), 
   // glm::ivec2(0, 1),
   // glm::ivec2(-1, 0),
   // glm::ivec2(0, -1)`

   // };

   // glm::ivec2 off;
      glBindFramebuffer(GL_FRAMEBUFFER, destination.fbo);
      
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, stateField.texture);




   // shaderHandle = shaders->boundaries;
   // glUseProgram(shaderHandle);
   // glBindVertexArray(VAOs[1]);
   // glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  //glBindTexture(GL_TEXTURE_2D, 0);
   glDrawArrays(GL_LINES, 0, 8);





   // for(int i = 0; i < 4; i++){
   //    off = offsetArray[i];
   //    glUniform2i(offset, off.x, off.y);
   //    glDrawArrays(GL_LINES,i * sizeof(point4), 2);
   // }
   unbind();

   glBindVertexArray(VAOs[0]);
   glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);

}

