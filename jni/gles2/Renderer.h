#ifndef AR_OPENGL_RENDERER
#define AR_OPENGL_RENDERER

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class Renderer {
public:
	Renderer();
	~Renderer();

	void setScreenSize(int w, int h);
	void setImageSize(int w, int h);
	void loadTexture(GLubyte* pixels);
	void renderFrame();

	int imageWidth;
	int imageHeight;

private:

	GLuint vsPositionHandle;
	GLuint vsTexCoordHandle;
	GLuint vsProjectionHandle;
	GLuint fsTextureHandle;
	GLuint programHandle;

	GLuint textureHandle;
	GLuint textureHandle2;

	float width;
	float height;



	static void printGLString(const char *name, GLenum s);
	static void checkGlError(const char* op);

	void setupGraphics();
	GLuint loadShader(GLenum shaderType, const char* pSource);

	double lastTime;
};
#endif
