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
	/*
	 * constructor to create the Renderer instance, instantiates
	 * the needed shaders for rendering and prints out the current
	 * open gl version for debugging purpose
	 *
	 */
	Renderer();
	~Renderer();

	/*
	 * creates the viewport for the given values
	 */
	void setViewport(float offsetX, float offsetY, float width, float height);

	/*
	 * generates a texture in the openGL context and returns the reference to it
	 */
	GLuint generateTexture();

	/*
	 * loads a bitmap image into the opengl context and for the given reference
	 */
	void loadTexture(GLuint glRef, GLubyte* pixels, float width, float height);

	/*
	 * renders the models and camera image, which are delieverd with the
	 * enviroment data object
	 */
	void renderFrame(EnvironmentData *envData);

	/*
	 * renders the search pattern image and returns the buffer data as a byte array
	 */
	GLubyte* createSearchPattern(EnvironmentData *envData);


private:
	// program references
	GLuint cameraProgramRef;
	GLuint objectProgramRef;
	GLuint searchPatternProgramRef;

	//camera shader references
	GLuint vsCamPosRef;
	GLuint vsCamTexRef;
	GLuint fsCamTextureRef;

	//object shader references
	GLuint vsObjPosRef;
	GLuint vsObjNormalRef;
	GLuint vsObjTexRef;
	GLuint vsObjProjectionViewRef;
	GLuint vsObjModelViewRef;
	GLuint fsObjTextureRef;

	//search pattern references
	GLuint vsSearchPosRef;
	GLuint vsSearchNormalRef;
	GLuint vsSearchProjectionViewRef;
	GLuint vsSearchModelViewRef;

	// debug method to display openGL errors
	static void printGLString(const char *name, GLenum s);
	static void checkGlError(const char* op);

	//method for instantiation
	void loadPrograms();
	GLuint loadShader(GLenum shaderType, const char* pSource);
	void linkProgram(GLuint programHandler);
};

#endif
