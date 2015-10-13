#include "Texture.h"

GLuint loadTextureFromFile(const string& filename) {
	SDL_Surface *imageSurface = IMG_Load(filename.c_str());
	if (!imageSurface) {
		cout << "Can't Load image" << filename << " " << IMG_GetError();
	}

	GLuint textureID = convertSDLSurfaceToTexture(imageSurface);
	SDL_FreeSurface(imageSurface);

	return textureID;
}

GLuint convertSDLSurfaceToTexture(SDL_Surface * surface) {
	GLuint textureID = 0;
	GLint nOfColors = surface->format->BytesPerPixel;

	GLenum textureFormat = GL_RGBA;
	GLenum internalFormat = GL_RGBA8;

	if (nOfColors== 4)// contains an alpha channel
	{
		if (surface->format->Rmask == 0x000000ff) {
			textureFormat = GL_BGRA;
			internalFormat = GL_RGBA8;
		}
	}
	else if (nOfColors == 3)//no alpha channel
	{
		if (surface->format->Rmask == 0x000000ff) {
			textureFormat = GL_RGB;
			internalFormat = GL_RGB8;
		}
		else {
			textureFormat = GL_BGR;
			internalFormat = GL_RGB8;
		}
	}
	else {
		cout << "warning: The image is not truecolor... this will probably break";
		return 0;
	}
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);
	return textureID;
}