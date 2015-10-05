#include "Common.h"
#include "Graphics.h"
#include "Vertex.h"

Vertex verts[] = {
	//front
	{ -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f },//Top Left
	{ -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f }, //bottom Left
	{ 0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f }, //bottom right
	{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f },//Top Right

	//back
	{ -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f },//Top Left
	{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f }, //bottom Left
	{ 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f }, //bottom right
	{ 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f },//Top Right
};

GLuint indices[] = {
	//front
	0,1,2,
	0,3,2,
	//left
	4,5,1,
	4,1,0,
	//right
	3,7,2,
	7,6,2,
	//bottom
	1,5,2,
	6,2,5,
	//top
	4,0,7,
	0,7,3,
	//back
	4,5,6,
	4,7,6
};

GLuint VBO;
GLuint EBO;

FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

Transform cubeTransform;
Transform cameraTransform;
float mposx, mposy;

void initScene() {
	//Create buffer
	glGenBuffers(1, &VBO);
	//make new VBO active
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Copy Vertex Data to VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void cleanUp() {
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void render() {
	//Set the clear colour(background)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//clear the colour and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Make the new VBO active
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//the 3 parameter is now filled out, the pipeline needs to know the size of
	//each vertex
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), NULL);
	//The Last parameter states that colours start 3 floats into 
	//each element of the array
	glColorPointer(4, GL_FLOAT, sizeof(Vertex), (void**)(3 * sizeof(float)));

	//Establish array contains vertices and colours
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//switch to model view
	glMatrixMode(GL_MODELVIEW);
	//reset using identity matrix
	glLoadIdentity();

	gluLookAt(0.0, 0.0, 0.6, cameraTransform.x, cameraTransform.y, -1.0f, 0.0, 1.0, 0.0);

	//translate to -5 on z axis
	glTranslatef(cubeTransform.x, cubeTransform.y, -5.0f + cubeTransform.z);
	glRotatef(cubeTransform.rx, 1, 0, 0);
	//begin drawing triangles
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint),GL_UNSIGNED_INT, 0);
}

void update() {
}

int main(int argc, char * arg[])
{

	bool run = true;
	bool pause = false;
	// init everything - SDL, if it is nonzero we have a problem
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "ERROR SDL_Init" << SDL_GetError() << std::endl;

		return -1;
	}
	
	SDL_Window *window = SDL_CreateWindow("SDL", // window title
		SDL_WINDOWPOS_CENTERED, //x position, centered
		SDL_WINDOWPOS_CENTERED, //y position, centered
		640, //width, in pixels
		480, //height, in pixels
		SDL_WINDOW_OPENGL //flags
		);

	//Create an OpenGL context associated with the window
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	//initialisation
	//call our InitOpenGL Function
	initOpenGL();
	//set the viewport
	setViewport(640, 480);

	initScene();
	
	//Value to hold the event generated by SDL
	SDL_Event event;
	//Game Loop
	while (run) {
		//while we still have events in the queue
		while (SDL_PollEvent(&event)) {
			//get the event
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				//set pur boolean which controls the loop to false
				run = false;
			}
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_LEFT:
					cubeTransform.x += -0.1f;
					break;
				case SDLK_RIGHT:
					cubeTransform.x += 0.1f;
					break;
				case SDLK_UP:
					cubeTransform.rx += 10.0f;
					break;
				case SDLK_DOWN:
					cubeTransform.rx += -10.0f;
					break;
				}
			}
			if (event.type == SDL_MOUSEMOTION) {
				if(event.motion.x > mposx){
					cameraTransform.x += 0.1f;
					mposx = event.motion.x;
				}
				if (event.motion.x < mposx) {
					cameraTransform.x -= 0.1f;
					mposx = event.motion.x;
				}
			}
		}

		update();
		render();

		//Call Swap so that our GL back bffer is displayed
		SDL_GL_SwapWindow(window);
	}
	//clean up, in reverse order
	cleanUp();
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

    return 0;
}
