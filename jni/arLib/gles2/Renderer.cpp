#include "Renderer.h"
#include "Shaders.h"
#include "util/Logger.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 * static arrays to hold the vertices and texure vertices
 * for rendering the camera image
 *
 */
GLfloat const gCameraQuad[] = {
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
};

GLfloat const gCameraTexCoord[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
};

Renderer::Renderer()
{
	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);

	GLuint cameraVerticeShaderRef;
	GLuint cameraFragmentShaderRef;

	GLuint objectVerticeShaderRef;
	GLuint objectFragmentShaderRef;

	GLuint searchPatternVerticeShaderRef;
	GLuint searchPatternFragmentShaderRef;

	/*
	 * Load the three shaders for rendering the camera image, models and the
	 * search pattern
	 */
	cameraVerticeShaderRef = loadShader(GL_VERTEX_SHADER, cameraVerticeShader);
	cameraFragmentShaderRef = loadShader(GL_FRAGMENT_SHADER, cameraFragmentShader);
	objectVerticeShaderRef = loadShader(GL_VERTEX_SHADER, objectVerticeShader);
	objectFragmentShaderRef = loadShader(GL_FRAGMENT_SHADER, objectFragmentShader);
	searchPatternVerticeShaderRef = loadShader(GL_VERTEX_SHADER, searchPatternVerticeShader);
	searchPatternFragmentShaderRef = loadShader(GL_FRAGMENT_SHADER, searchPatternFragmentShader);


	// Create the program the camera image
	cameraProgramRef = glCreateProgram();
	if (cameraProgramRef == 0) {
		LOGE("setup graphics failed !!!");
		return;
	}
	// attach the camera shaders
	glAttachShader(cameraProgramRef, cameraVerticeShaderRef);
	glAttachShader(cameraProgramRef, cameraFragmentShaderRef);
	this->linkProgram(cameraProgramRef);

	glUseProgram(cameraProgramRef);

	// Use tightly packed data
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Create the program the search pattern
	searchPatternProgramRef = glCreateProgram();
	if (searchPatternProgramRef == 0) {
		LOGE("setup graphics failed !!!");
		return;
	}
	// attach the search pattern shaders
	glAttachShader(searchPatternProgramRef, searchPatternVerticeShaderRef);
	glAttachShader(searchPatternProgramRef, searchPatternFragmentShaderRef);
	this->linkProgram(searchPatternProgramRef);

	// Create the program the objects
	objectProgramRef = glCreateProgram();
	if (objectProgramRef == 0) {
		LOGE("setup graphics failed !!!");
		return;
	}
	// attach the object shaders
	glAttachShader(objectProgramRef, objectVerticeShaderRef);
	glAttachShader(objectProgramRef, objectFragmentShaderRef);
	this->linkProgram(objectProgramRef);

	// set the opengl References to the member of the class for further use
	vsCamPosRef = glGetAttribLocation(cameraProgramRef, "a_position");
	vsCamTexRef = glGetAttribLocation(cameraProgramRef, "a_texCoord");
	fsCamTextureRef = glGetUniformLocation(cameraProgramRef, "u_textureSampler");
	vsObjPosRef = glGetAttribLocation(objectProgramRef, "a_position");
	vsObjNormalRef = glGetAttribLocation(objectProgramRef, "a_normal");
	vsObjTexRef = glGetAttribLocation(objectProgramRef, "a_texCoord");
	vsObjProjectionViewRef = glGetUniformLocation(objectProgramRef, "u_projectionView");
	vsObjModelViewRef = glGetUniformLocation(objectProgramRef, "u_modelView");
	fsObjTextureRef = glGetUniformLocation(objectProgramRef, "u_textureSampler");
	vsSearchPosRef = glGetAttribLocation(searchPatternProgramRef, "a_position");
	vsSearchNormalRef = glGetAttribLocation(searchPatternProgramRef, "a_normal");
	vsSearchProjectionViewRef = glGetUniformLocation(searchPatternProgramRef, "u_projectionView");
	vsSearchModelViewRef = glGetUniformLocation(searchPatternProgramRef, "u_modelView");

	//enable the needed opengl features for the object and search pattern program
    glUseProgram(objectProgramRef);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(searchPatternProgramRef);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

Renderer::~Renderer()
{
}

void Renderer::setViewport(float offsetX, float offsetY, float width, float height)
{
	glViewport(offsetX, offsetY, width, height);
    checkGlError("glViewport");
}

/*
 * GL ES2 program & shader handling
 */
GLuint Renderer::loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {

    	int shaderStringLength = strlen(pSource);
        glShaderSource(shader, 1, &pSource, &shaderStringLength);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                	//print error message for debugging purpose
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                    exit(-1);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

void Renderer::linkProgram(GLuint programHandler) {
	GLint linked;
	// Link the program
	glLinkProgram(programHandler);
	// Check the link status
	glGetProgramiv(programHandler, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint infoLen = 0;
		glGetProgramiv(programHandler, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1) {
			LOGE("Error linking program",LOG_TAG);
		}
		glDeleteProgram(programHandler);
		return;
	}
}

/*
 * Texture handling
 */
GLuint Renderer::generateTexture() {
	GLuint textureRef;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureRef);
    checkGlError("glGenTextures");
	return textureRef;
}

void Renderer::loadTexture(GLuint glRef, GLubyte* pixels, float width, float height) {
    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    checkGlError("glActiveTexture");

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, glRef);
    checkGlError("glBindTexture");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
             GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    checkGlError("glGetAttribLocation");
}

GLubyte* Renderer::createSearchPattern(EnvironmentData *envData) {

	int width = envData->getImageDimension().width;
	int height =  envData->getImageDimension().height;


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(searchPatternProgramRef);
    glClear( GL_DEPTH_BUFFER_BIT);
    checkGlError("glClearColor");

    vector<Model*> *models = envData->getModels();

    glm::mat4 PM = envData->getProjectionMatrix() * envData->getViewMatrix();
    glUniformMatrix4fv(vsSearchProjectionViewRef, 1, GL_FALSE, glm::value_ptr(PM));

    for(vector<Model*>::size_type i = 0; i != models->size(); i++) {
    	if (i==0){continue;}
    	Model* m = models->at(i);
    	glUniformMatrix4fv(vsSearchModelViewRef, 1, GL_FALSE, glm::value_ptr(m->getModelMatrix()));

		glVertexAttribPointer(vsSearchPosRef, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, m->getVNT());
		checkGlError("glVertexAttribPointer");

		glEnableVertexAttribArray(vsSearchPosRef);
		checkGlError("glEnableVertexAttribArray");

		glDrawElements(GL_TRIANGLES, m->getNumberOfFaces(), GL_UNSIGNED_SHORT, m->getFaces());
		checkGlError("glDrawElemnts");
    }
    // read the rendered image data from the framebuffer to return the data
    GLubyte *buffer = new GLubyte[width * height * 4];
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

	return buffer;
}


void Renderer::renderFrame(EnvironmentData *envData) {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(cameraProgramRef);
    checkGlError("glUseProgram");


    glVertexAttribPointer(vsCamPosRef, 3, GL_FLOAT, GL_FALSE, 0, gCameraQuad);
    checkGlError("glVertexAttribPointer");
    glVertexAttribPointer(vsCamTexRef, 2, GL_FLOAT, GL_FALSE, 0, gCameraTexCoord);
    checkGlError("glVertexAttribPointer");

    glEnableVertexAttribArray(vsCamPosRef);
    checkGlError("glEnableVertexAttribArray");

    glEnableVertexAttribArray(vsCamTexRef);
    checkGlError("glEnableVertexAttribArray");

    // Set the sampler texture unit to 0
    glUniform1i(fsCamTextureRef, 0);
    checkGlError("fsCameraTextureRef");
    glBindTexture(GL_TEXTURE_2D, envData->cameraTextrueRef);
    checkGlError("glBindTexture");
    glDrawArrays(GL_TRIANGLES, 0, 6);
    checkGlError("glDrawArrays");

    glUseProgram(objectProgramRef);
    glClear( GL_DEPTH_BUFFER_BIT);

    vector<Model*> *models = envData->getModels();

    glm::mat4 PM = envData->getProjectionMatrix() * envData->getViewMatrix();
    glUniformMatrix4fv(vsObjProjectionViewRef, 1, GL_FALSE, glm::value_ptr(PM));

    for(vector<Model*>::size_type i = 0; i != models->size(); i++) {
    	if (i==0){continue;}
    	Model* m = models->at(i);

    	glUniformMatrix4fv(vsObjModelViewRef, 1, GL_FALSE, glm::value_ptr(m->getModelMatrix()));

		glVertexAttribPointer(vsObjPosRef, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, m->getVNT());
		checkGlError("glVertexAttribPointer");

		glVertexAttribPointer(vsObjTexRef, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, m->getVNT()+6);
		checkGlError("glVertexAttribPointer");

		glEnableVertexAttribArray(vsObjPosRef);
		checkGlError("glEnableVertexAttribArray");

		// get the materials and draw them
		Material *materials = m->getMaterials();
		for (int i=0; i < m->getNumberOfMaterials(); i++) {
				Material mat = materials[i];
				// bind the texture
				glUniform1i(fsObjTextureRef, 0);
				checkGlError("glUniform1i");
				glBindTexture(GL_TEXTURE_2D, mat.textureReference);
				checkGlError("glBindTexture");
				glDrawElements(GL_TRIANGLES, mat.length*3, GL_UNSIGNED_SHORT,m->getFaces() + mat.startIndex*3);
				checkGlError("glDrawElemnts");
		}
    }
}

void Renderer::printGLString(const char *name, GLenum s) {
	const char *v = (const char *) glGetString(s);
	LOGI("GL %s = %s\n", name, v);

}

void Renderer::checkGlError(const char* op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		LOGI("after %s() glError (0x%x)\n", op, error);
	}
}
