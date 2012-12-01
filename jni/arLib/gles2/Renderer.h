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
	//render without displaying
	GLubyte* createSearchPattern(EnvironmentData *envData);
private:
	// openGL ref values
	GLuint cameraProgramRef;
	GLuint objectProgramRef;
	GLuint vsModelViewRef;
	GLuint vsProjectionViewRef;
	GLuint vsColorRef;
	GLuint vsPositionRef;
	GLuint vsCameraPositionRef;
	GLuint vsNormalRef;

	GLuint searchPatternBufferRef;

	GLuint vsTexCoordHandle;
	GLuint vsObjectTexCoordHandle;


	// camera specific
	GLuint fsCameraTextureRef;
	GLuint fsObjectTextureRef;

	static void printGLString(const char *name, GLenum s);
	static void checkGlError(const char* op);

	//initalization
	void loadPrograms();
	GLuint loadShader(GLenum shaderType, const char* pSource);
	void linkProgram(GLuint programHandler);

	//fps counter
	double lastTime;
};
#endif
