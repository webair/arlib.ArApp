#ifndef AR_OPENGL_RENDERER
#define AR_OPENGL_RENDERER

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "arLib/model/Model.h"
#include "arLib/EnvironmentData.h"
#include <vector>

using namespace std;

class Renderer {
public:
	Renderer();
	~Renderer();

	void setViewport(float offsetX, float offsetY, float width, float height);

	// texture handling
	GLuint generateTexture();
	void loadTexture(GLuint glRef, GLubyte* pixels, float width, float height);
	void renderFrame(EnvironmentData *envData);

	//obsolete
	void addModel(Model* m);

private:
	// openGL ref values
	GLuint cameraProgramRef;
	GLuint objectProgramRef;

	GLuint vsPositionHandle;
	GLuint vsTexCoordHandle;
	GLuint vsProjectionHandle;
	GLuint vsModelViewMatrixHandle;

	// camera specific
	GLuint fsCameraTextureRef;

	static void printGLString(const char *name, GLenum s);
	static void checkGlError(const char* op);

	//initalization
	void loadPrograms();
	GLuint loadShader(GLenum shaderType, const char* pSource);
	void linkProgram(GLuint programHandler);

	//fps counter
	double lastTime;

	//models
	vector<Model*>* models;
};
#endif
