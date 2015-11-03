#include "Common.h"
#include "Graphics.h"
#include "Vertices.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "FileSystem.h"
#include "FBXLoader.h"

float vertices[] = {
	-1,-1,
	1,-1,
	-1,1,
	1,1
};

//matrices
mat4 viewMatrix;
mat4 projMatrix;
mat4 worldMatrix;
mat4 MVPMatrix;

GLuint VBO;
GLuint EBO;
GLuint VAO;
GLuint FBO;
GLuint FBOTexture;
GLuint FBODepthBuffer;
GLuint FVAO;
GLuint FVBO;
GLuint fullscreenShaderProgram;
GLuint shaderProgram;

const int FRAME_BUFFER_WIDTH = 640;
const int FRAME_BUFFER_HEIGHT = 480;

MeshData currentMesh;
GLuint textureMap;
vec3 cameraTransform(0.0, 0.0, 0.0);
vec3 cameraRotation(0.0, 0.0, 0.0);
float mposx, mposy;

vec4 ambientMaterialColour(0.3f, 0.3f, 0.3f, 1.0f);
vec4 ambientLightColour(1.0f, 1.0f, 1.0f, 1.0f);
vec4 diffuseMaterialColour(0.3f, 0.3f, 0.3f, 1.0f);
vec4 diffuseLightColour(1.0f, 1.0f, 1.0f, 1.0f);
vec4 specularMaterialColour(0.3f, 0.3f, 0.3f, 1.0f);
vec4 specularLightColour(1.0f, 1.0f, 1.0f, 1.0f);
float specPower = 1.0f;

vec3 LightDir = vec3(0.0f, 0.0f, 1.0f);


void createFramebuffer() {
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &FBOTexture);
	glBindTexture(GL_TEXTURE_2D, FBOTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glGenRenderbuffers(1, &FBODepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, FBODepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FBODepthBuffer);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(status!=GL_FRAMEBUFFER_COMPLETE){
		cout << "Issue with Framebuffers" << endl;
	}
}

void initScene()
{
	//load mesh and bind it
	string modelPath = ASSET_PATH + MODEL_PATH + "/Utah-Teapot.fbx";
	loadFBXFromFile(modelPath, &currentMesh);

	//load texture & bind it
	string texturePath = ASSET_PATH + TEXTURE_PATH + "/armoredrecon_diff_white.png";
	textureMap = loadTextureFromFile(texturePath);

	glBindTexture(GL_TEXTURE_2D, textureMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	createFramebuffer();
	glGenVertexArrays(1, &FVAO);
	glBindVertexArray(FVAO);
	glGenBuffers(1, &FVBO);
	glBindBuffer(GL_ARRAY_BUFFER, FVBO);

	glVertexPointer(2, GL_FLOAT, 0, NULL);

	GLuint fvertexShaderProgram = 0;
	string vsPath = ASSET_PATH + SHADER_PATH + "/simplePostProcessVS.glsl";
	fvertexShaderProgram = loadShaderFromFile(vsPath, VERTEX_SHADER);
	checkForCompilerErrors(fvertexShaderProgram);

	GLuint ffragmentShaderProgram = 0;
	string fsPath = ASSET_PATH + SHADER_PATH + "/simplePostProcessFS.glsl";
	ffragmentShaderProgram = loadShaderFromFile(fsPath, FRAGMENT_SHADER);
	checkForCompilerErrors(ffragmentShaderProgram);

	fullscreenShaderProgram = glCreateProgram();
	glAttachShader(fullscreenShaderProgram, fvertexShaderProgram);
	glAttachShader(fullscreenShaderProgram, ffragmentShaderProgram);

	//Link attributes
	glBindAttribLocation(shaderProgram, 0, "vertexPosition");

	glLinkProgram(fullscreenShaderProgram);
	checkForLinkErrors(fullscreenShaderProgram);
	//now we can delete the VS & FS Programs
	glDeleteShader(fvertexShaderProgram);
	glDeleteShader(ffragmentShaderProgram);

	glGenVertexArrays(1,&VAO);
	glBindVertexArray( VAO );
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, currentMesh.getNumVerts()*sizeof(Vertex), &currentMesh.vertices[0], GL_STATIC_DRAW);

	//create buffer
	glGenBuffers(1, &EBO);
	//Make the EBO active
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//Copy Index data to the EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentMesh.getNumIndices()*sizeof(int), &currentMesh.indices[0], GL_STATIC_DRAW);

	//Tell the shader that 0 is the position element
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void**)(sizeof(vec3)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void**)((sizeof(vec3) + (sizeof(vec4)))));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void**)((sizeof(vec3) + (sizeof(vec4)) + (sizeof(vec2)))));

	GLuint vertexShaderProgram=0;
	vsPath = ASSET_PATH + SHADER_PATH + "/specularVS.glsl";
	vertexShaderProgram = loadShaderFromFile(vsPath, VERTEX_SHADER);
	checkForCompilerErrors(vertexShaderProgram);

	GLuint fragmentShaderProgram=0;
    fsPath = ASSET_PATH + SHADER_PATH + "/specularFS.glsl";
	fragmentShaderProgram = loadShaderFromFile(fsPath, FRAGMENT_SHADER);
	checkForCompilerErrors(fragmentShaderProgram);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShaderProgram);
	glAttachShader(shaderProgram, fragmentShaderProgram);

	//Link attributes
	glBindAttribLocation(shaderProgram, 0, "vertexPosition");
	glBindAttribLocation(shaderProgram, 1, "vertexColour");
	glBindAttribLocation(shaderProgram, 2, "vertexTexCoords");

	glLinkProgram(shaderProgram);
	checkForLinkErrors(shaderProgram);
	//now we can delete the VS & FS Programs
	glDeleteShader(vertexShaderProgram);
	glDeleteShader(fragmentShaderProgram);
}

void cleanUp()
{
	glDeleteTextures(1, &textureMap);

	glDeleteProgram(shaderProgram);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1,&VAO);
}

void

void update()
{
  projMatrix = glm::perspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

  viewMatrix = glm::lookAt(cameraTransform, cameraRotation, vec3(0.0f, 1.0f, 0.0f));

  worldMatrix= glm::translate(mat4(1.0f), vec3(0.0f,0.0f,0.0f));

  MVPMatrix=projMatrix*viewMatrix*worldMatrix;
}

void render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    //old imediate mode!
    //Set the clear colour(background)
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    //clear the colour and depth buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram);

    GLint MVPLocation = glGetUniformLocation(shaderProgram, "MVP");
	glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));

	GLint modelLocation = glGetUniformLocation(shaderProgram, "Model");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(worldMatrix));

	GLint amcLocation = glGetUniformLocation(shaderProgram, "ambientMaterialColour");
	glUniform4fv(amcLocation, 1, value_ptr(ambientMaterialColour));

	GLint alcLocation = glGetUniformLocation(shaderProgram, "ambientLightColour");
	glUniform4fv(alcLocation, 1, value_ptr(ambientLightColour));

	GLint dmcLocation = glGetUniformLocation(shaderProgram, "diffuseMaterialColour");
	glUniform4fv(dmcLocation, 1, value_ptr(diffuseMaterialColour));

	GLint dlcLocation = glGetUniformLocation(shaderProgram, "diffuseLightColour");
	glUniform4fv(dlcLocation, 1, value_ptr(diffuseLightColour));

	GLint smcLocation = glGetUniformLocation(shaderProgram, "specularMaterialColour");
	glUniform4fv(smcLocation, 1, value_ptr(specularMaterialColour));

	GLint slcLocation = glGetUniformLocation(shaderProgram, "specularLightColour");
	glUniform4fv(slcLocation, 1, value_ptr(specularLightColour));

	GLint lightDirLocation = glGetUniformLocation(shaderProgram, "lightDirection");
	glUniform3fv(lightDirLocation, 1, value_ptr(LightDir));

	GLint camPosLoc = glGetUniformLocation(shaderProgram, "CameraPosition");
	glUniform3fv(camPosLoc, 1, value_ptr(cameraTransform));

	GLint specPowerLoc = glGetUniformLocation(shaderProgram, "specularPower");
	glUniform1f(specPowerLoc, specPower);

	/*GLint texture0Location = glGetUniformLocation(shaderProgram, "texture0");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMap);
	glUniform1i(texture0Location, 0);*/

    glBindVertexArray( VAO );

    glDrawElements(GL_TRIANGLES, currentMesh.getNumIndices(),GL_UNSIGNED_INT,0);
}

int main(int argc, char * arg[])
{
	ChangeWorkingDirectory();
    //Controls the game loop
    bool run=true;

    // init everyting - SDL, if it is nonzero we have a problem
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "ERROR SDL_Init " <<SDL_GetError()<< std::endl;

        return -1;
	}

	int imageInitFlags = IMG_INIT_JPG | IMG_INIT_PNG;
	int returnInitFlags = IMG_Init(imageInitFlags);
	if (((returnInitFlags)& (imageInitFlags)) != imageInitFlags) {
		cout << "ERROR SDL_Image Init" << IMG_GetError() << endl;
	}

	if (TTF_Init() == -1) {
		cout << "ERROR TTF_Init:" << TTF_GetError();
	}

	//Request opengl 4.1 context, Core Context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    //Create a window
    SDL_Window * window = SDL_CreateWindow(
                                           "SDL",             // window title
                                           SDL_WINDOWPOS_CENTERED,     // x position, centered
                                           SDL_WINDOWPOS_CENTERED,     // y position, centered
                                           640,                        // width, in pixels
                                           480,                        // height, in pixels
                                           SDL_WINDOW_OPENGL           // flags
                                           );

    // Create an OpenGL context associated with the window.
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    //Call our InitOpenGL Function
    initOpenGL();
    //Set our viewport
    setViewport(640,480);

    initScene();
    //Value to hold the event generated by SDL
    SDL_Event event;
    //Game Loop
    while(run)
    {
        //While we still have events in the queue
        while (SDL_PollEvent(&event)) {
            //Get event type
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                //set our boolean which controls the loop to false
                run = false;
            }
            if (event.type==SDL_KEYDOWN){
                switch( event.key.keysym.sym )
                {
					case SDLK_w:
						cameraTransform.x += 0.5f;
						cameraRotation.x += 0.5f;
						break;
					case SDLK_s:
						cameraTransform.x -= 0.5f;
						cameraRotation.x -= 0.5f;
						break;
					case SDLK_a:
						cameraTransform.z -= 0.5f;
						cameraRotation.z -= 0.5f;
						break;
					case SDLK_d:
						cameraTransform.z += 0.5f;
						cameraRotation.z += 0.5f;
						break;
					}
				}
				if (event.type == SDL_MOUSEMOTION) {
					if (event.motion.x > mposx) {
						cameraRotation.x += 0.1f;
						mposx = event.motion.x;
					}
					if (event.motion.x < mposx) {
						cameraRotation.x -= 0.1f;
						mposx = event.motion.x;
					}
					if (event.motion.y > mposy) {
						cameraRotation.y -= 0.1f;
						mposy = event.motion.y;
					}
					if (event.motion.y < mposy) {
						cameraRotation.y += 0.1f;
						mposy = event.motion.y;
					}
            }
        }
        //init Scene
        update();
        //render
        render();
        //Call swap so that our GL back buffer is displayed
        SDL_GL_SwapWindow(window);

    }

    // clean up, reverse order!!!
    cleanUp();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
	IMG_Quit();
	TTF_Quit();
    SDL_Quit();

    return 0;
}
