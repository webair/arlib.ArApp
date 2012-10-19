#ifndef AR_OPENGL_RENDERER
#define AR_OPENGL_RENDERER

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "arLib/model/Model.h"
#include <vector>

using namespace std;

class Renderer {
public:
	Renderer();
	~Renderer();

	void setViewport(float offsetX, float offsetY, float width, float height);



	void setImageSize(int w, int h);
	void loadTexture(GLubyte* pixels);
	void renderFrame();

	//models to draw
	void addModel(Model* m);

private:
	// openGL ref values
	GLuint cameraProgramRef;
	GLuint objectProgramRef;

	GLuint vsPositionHandle;
	GLuint vsTexCoordHandle;
	GLuint vsProjectionHandle;
	GLuint vsModelViewMatrixHandle;
	GLuint fsTextureHandle;

	GLuint textureHandle;

	// screen dimension save this as members so we don't calculate them every time
	float viewPortWidth;
	float viewPortHeight;
	float viewPortRatio;

	float imageWidth;
	float imageHeight;
	float imageRatio;

	float frustumNear;
	float frustumFar;
	float frustumAngle;
	float frustumDistanceRatio;


	static void printGLString(const char *name, GLenum s);
	static void checkGlError(const char* op);

	//initalization
	void setupGraphics();
	GLuint loadShader(GLenum shaderType, const char* pSource);
	void linkProgram(GLuint programHandler);

	//fps counter
	double lastTime;

	//models
	vector<Model*>* models;
};
#endif
