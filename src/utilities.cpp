#include "common.h"

void initShaders(Shaders * shaders) {
	//shaders->advection = InitShader("shaders//vshader.glsl", "shaders//fshader.avection.glsl");
}

Field createField(int width, int height) {
	// Create frame buffer
	GLuint fbo;

	// Create texture
	GLuint texture;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
	
	// Create color buffer
	GLuint color;

	// Assign texture to frame buffer


	// Assign field variables
	Field newField = { fbo, texture };
	return newField;
}

Pair createPair(int width, int height) {
	Pair newPair;
	newPair.foo = createField(width, height);
	newPair.bar = createField(width, height);
	return newPair;
}

void clearField(Field field, float clearColor) {
	glBindFramebuffer(GL_FRAMEBUFFER, field.fbo);
	glClearColor(clearColor, clearColor, clearColor, clearColor);
	glClear(GL_COLOR_BUFFER_BIT);
}

void swapField(Pair *fieldPair) {
	Field temp = fieldPair->foo;
	fieldPair->foo = fieldPair->bar;
	fieldPair->bar = temp;
}