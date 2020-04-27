//----------------------------------------------------------------------------
// NAMES            : Mark Robitaille, John Manning
// COURSE           : COMP 4490
// INSTRUCTOR       : John Braico
// PROJECT          : Fluid Dynamics Demonstration
// FILE             : utilities.cpp
//
// REMARKS	: This file has functions for creating our data structures and other 
//            utility functions. 
//
//----------------------------------------------------------------------------

#include "common.h"

//----------------------------------------------------------------------------
//--- initShaders ---
// Initializes shaders and loads them into the shaders struct passed in as a parameter
void initShaders(Shaders* shaders) {
	std::cout << "->-> init shaders started.\n";
	shaders->drawTexture = InitShader("vshader.glsl", "drawTexture.glsl");
		std::cout << "->-> init drawTexture complete.\n";

	shaders->advect = InitShader("vshader.glsl", "advect.glsl");
		std::cout << "->-> init advect complete.\n";

	shaders->divergence = InitShader("vshader.glsl", "divergence.glsl");
		std::cout << "->-> init divergence complete.\n";

	shaders->jacobi = InitShader("vshader.glsl", "jacobi.glsl");
		std::cout << "->-> init jacobi complete.\n";

	shaders->subtractGradient = InitShader("vshader.glsl", "subtractGradient.glsl");
		std::cout << "->-> init subtractGradient complete.\n";

	shaders->addedForce = InitShader("vshader.glsl", "addedForce.glsl");
		std::cout << "->-> init addedForce complete.\n";
	
	shaders->addedInk = InitShader("vshader.glsl", "addedInk.glsl");
		std::cout << "->-> init addedInk complete.\n";
}

//----------------------------------------------------------------------------
//--- createField ---
// Creates a field given a provided width and height.
// Creates a texture and a framebuffer object, then attaches the texture to the framebuffer.
Field createField(GLint width, GLint height) {
	// Put texture attachment into a frame buffer, so we can easily use the data between passes of the fragment shader.
	Field newField = { 0, 0 }; // Set in case of errors during creation

	// Create frame buffer
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create texture
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Any texture coordinate are clamped between (0,0) to (1,1).
	// In 2d, this is s, t. In 3D, this is s, t, r.
	// May need to consider desired effect when in 3D? Want to clamp to edge? Border?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 

	// Do linear filtering on textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate texture to width & height
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, fieldWidth, fieldHeight, 0, GL_RGB, GL_FLOAT, 0);

	// Assign texture to frame buffer (as a color attachment)
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	// Assign field variables (assuming no errors)
	if (GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER) || GL_NO_ERROR == glGetError()) {
		newField = { fbo, texture };
	}
	else {
		printf("Field initialization error!\n");
	}

	glClearColor(0, 0, 0, 0); 
	glClear(GL_COLOR_BUFFER_BIT); 
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Set framebuffer back to default of 0
	return newField;
}

//----------------------------------------------------------------------------
//--- createPair ---
// Creates a Pair struct and sets both of the Fields stored within.
Pair createPair(int width, int height) {
	// Create 2 fields, foo is the read field and bar is write field
	Pair newPair;
	newPair.foo = createField(width, height);
	newPair.bar = createField(width, height);
	return newPair;
}

//----------------------------------------------------------------------------
//--- clearField ---
// Clear's a field by clearing the frame buffer's color.
void clearField(Field field, float clearColor) {
	// Clear the given field with a set color
	glBindFramebuffer(GL_FRAMEBUFFER, field.fbo);
	glClearColor(clearColor, clearColor, clearColor, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

//----------------------------------------------------------------------------
//--- swapField ---
// Swaps a Pair's foo and bar Fields
void swapField(Pair *fieldPair) {
	// Swap the fields foo and bar around.
	Field temp = fieldPair->foo;
	fieldPair->foo = fieldPair->bar;
	fieldPair->bar = temp;
}
