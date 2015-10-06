#include "Graphics.h"
#include "Common.h"

//function to initialise OpenGL
void initOpenGL() {
	//smooth shading
	glShadeModel(GL_SMOOTH);

	//clear the background to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Clear the depth buffer to 1.0
	glClearDepth(1.0f);

	//Enable Depth testing
	glEnable(GL_DEPTH_TEST);

	//the depth test to use
	glDepthFunc(GL_LEQUAL);

	//turn on the best perspective correction
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		// problem glewinit failed, something is seriously wrong
		std::cout << "Error: "<<glewGetErrorString(err) << std::endl;
	}
}
//function set/reset viewport
void setViewport(int width, int height) {
	//setup viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}