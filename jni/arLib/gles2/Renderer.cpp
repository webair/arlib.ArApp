#include "Renderer.h"

#include "time.h"
#include "util/Logger.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#include "modelData/bundehaus.h"

/*
 * Constructor & Destructor
 */

static const char cameraVerticeShader[] =
		"attribute vec4 a_position;"
		"attribute vec2 a_texCoord;"
		"varying vec2 v_camTexCoord;"
		"void main()"
		"{"
			"gl_Position = a_position;"
			"v_camTexCoord = a_texCoord;"
		"}";

static const char cameraFragmentShader[] =
		"precision mediump float;"
		"uniform sampler2D u_textureSampler;"
		"varying vec2 v_camTexCoord;"
		"void main()\n"
		"{"
			"gl_FragColor = texture2D( u_textureSampler, v_camTexCoord );"
		"}"
;

static const char searchPatternVerticeShader[] =

		"attribute vec4 a_position;"
		"attribute vec4 a_normal;"

		"uniform mat4 u_projectionView;"
		"uniform mat4 u_modelView;"

		"void main()"
		"{"
			//not used yet, but has to be declared
			"vec4 newNormal = u_projectionView * u_modelView * a_normal;"
			"gl_Position = u_projectionView * u_modelView * a_position;"
		"}"
;

static const char searchPatternFragmentShader[] =

		"precision mediump float;"
		"varying float depth;"
		"void main()\n"
		"{"
			"gl_FragColor = vec4(1.0,1.0,1.0,1.0);"
		"}"
;

static const char objectVerticeShader[] =

		"attribute vec4 a_position;"
		"attribute vec4 a_normal;"
		"attribute vec2 a_texCoord;"

		"uniform mat4 u_projectionView;"
		"uniform mat4 u_modelView;"

		"varying vec2 v_objTexCoord;"

		"void main()"
		"{"
			//not used yet, but has to be declared
			"vec4 newNormal = u_projectionView * u_modelView * a_normal;"
			"v_objTexCoord = a_texCoord;"
			"gl_Position = u_projectionView * u_modelView * a_position;"
		"}"
;

static const char objectFragmentShader[] =

		"precision mediump float;"
		"uniform sampler2D u_textureSampler;"
		"varying vec2 v_objTexCoord;"
		"void main()\n"
		"{"
			"gl_FragColor = texture2D( u_textureSampler, v_objTexCoord );"
		"}"
;

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
	// 10 should be enough for the moment...
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

	// Load the vertex/fragment shaders
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
	// attach the camera shaders
	glAttachShader(searchPatternProgramRef, searchPatternVerticeShaderRef);
	glAttachShader(searchPatternProgramRef, searchPatternFragmentShaderRef);
	this->linkProgram(searchPatternProgramRef);

	// Create the program the objects
	objectProgramRef = glCreateProgram();
	if (objectProgramRef == 0) {
		LOGE("setup graphics failed !!!");
		return;
	}
	// attach the camera shaders
	glAttachShader(objectProgramRef, objectVerticeShaderRef);
	glAttachShader(objectProgramRef, objectFragmentShaderRef);
	this->linkProgram(objectProgramRef);


	vsCamPosRef = glGetAttribLocation(cameraProgramRef, "a_position");
	vsCamTexRef = glGetAttribLocation(cameraProgramRef, "a_texCoord");
	fsCamTextureRef = glGetUniformLocation(cameraProgramRef, "u_textureSampler");
	LOGI("camera attributes: pos=%d, tex=%d", vsCamPosRef, vsCamTexRef);
	LOGI("camera uniforms: tex=%d", fsCamTextureRef);

	vsObjPosRef = glGetAttribLocation(objectProgramRef, "a_position");
	vsObjNormalRef = glGetAttribLocation(objectProgramRef, "a_normal");
	vsObjTexRef = glGetAttribLocation(objectProgramRef, "a_texCoord");
	vsObjProjectionViewRef = glGetUniformLocation(objectProgramRef, "u_projectionView");
	vsObjModelViewRef = glGetUniformLocation(objectProgramRef, "u_modelView");
	fsObjTextureRef = glGetUniformLocation(objectProgramRef, "u_textureSampler");
	LOGI("object attributes: pos=%d, normal=%d, tex=%d", vsObjPosRef, vsObjNormalRef, vsObjTexRef);
	LOGI("object uniforms: projView=%d, modelView=%d, tex=%d", vsObjProjectionViewRef, vsObjModelViewRef, fsObjTextureRef);

	vsSearchPosRef = glGetAttribLocation(searchPatternProgramRef, "a_position");
	vsSearchNormalRef = glGetAttribLocation(searchPatternProgramRef, "a_normal");
	vsSearchProjectionViewRef = glGetUniformLocation(searchPatternProgramRef, "u_projectionView");
	vsSearchModelViewRef = glGetUniformLocation(searchPatternProgramRef, "u_modelView");
	LOGI("search pattern attributes: pos=%d, normal=%d", vsSearchPosRef, vsSearchNormalRef);
	LOGI("search uniforms: projView=%d, modelView=%d", vsSearchProjectionViewRef, vsSearchModelViewRef);

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

// interface methods
void Renderer::setViewport(float offsetX, float offsetY, float width, float height)
{
	glViewport(offsetX, offsetY, width, height);
    checkGlError("glViewport");

	LOGI("viewport width: %f", width);
	LOGI("viewport height: %f", height);
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
	// TODO: has to be improved when objects contains textures
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

		//glVertexAttribPointer(vsSearchNormalRef, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, m->getVNT()+3);
		//checkGlError("glVertexAttribPointer");

		glEnableVertexAttribArray(vsSearchPosRef);
		checkGlError("glEnableVertexAttribArray");

		glDrawElements(GL_TRIANGLES, m->getNumberOfFaces(), GL_UNSIGNED_SHORT, m->getFaces());
		checkGlError("glDrawElemnts");
    }

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

    bool showNormals = false;
    glm::mat4 PM = envData->getProjectionMatrix() * envData->getViewMatrix();
    glUniformMatrix4fv(vsObjProjectionViewRef, 1, GL_FALSE, glm::value_ptr(PM));

    for(vector<Model*>::size_type i = 0; i != models->size(); i++) {
    	if (i==0){continue;}
    	Model* m = models->at(i);
    	glUniformMatrix4fv(vsObjModelViewRef, 1, GL_FALSE, glm::value_ptr(m->getModelMatrix()));

		glVertexAttribPointer(vsObjPosRef, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, m->getVNT());
		checkGlError("glVertexAttribPointer");

		//glVertexAttribPointer(vsObjNormalRef, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, m->getVNT()+3);
		//checkGlError("glVertexAttribPointer");

		glVertexAttribPointer(vsObjTexRef, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, m->getVNT()+6);
		checkGlError("glVertexAttribPointer");

		glEnableVertexAttribArray(vsObjPosRef);
		checkGlError("glEnableVertexAttribArray");

        if (!showNormals) {
        	Material *materials = m->getMaterials();
        	for (int i=0; i < m->getNumberOfMaterials(); i++) {
        		if (i==4) {
            		Material mat = materials[i];
                    // bind le texture
                    glUniform1i(fsObjTextureRef, 0);
                    checkGlError("glUniform1i");
                    glBindTexture(GL_TEXTURE_2D, mat.textureReference);
                    checkGlError("glBindTexture");

                	glDrawElements(GL_TRIANGLES, mat.length*3, GL_UNSIGNED_SHORT, m->getFaces() + (mat.startIndex * 3));
                	checkGlError("glDrawElemnts");
        		}


        	}




        } else {
        	//draw normals
        	for (int i = 0; i < m->getNumberOfVNT(); i++) {
        		if (i%9 == 0) {

        			float scale = 4.0f;

					float xPos = m->getVNT()[i];
					float yPos = m->getVNT()[i + 1];
					float zPos = m->getVNT()[i + 2];

					float xNorm = m->getVNT()[i + 3] * scale;
					float yNorm = m->getVNT()[i + 4] * scale;
					float zNorm = m->getVNT()[i + 5] * scale;

					GLfloat line[] = {xPos, yPos, zPos,//0.0f, 0.0f, 1.0f,
							xPos + xNorm, yPos + yNorm, zPos + zNorm
								   };
					GLfloat normals[] = {0.0f, 0.0f, 1.0f,
										0.0f, 0.0f, 1.0f
								   };
					glVertexAttribPointer(vsObjPosRef, 3, GL_FLOAT, GL_FALSE, 0, line);
					checkGlError("glVertexAttribPointer normal position");

					GLushort index[] = {0,1};
					glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, index);
					checkGlError("glDrawElements normal");

      			}
        	}

        }

    }

    /*
    GLfloat xAxis[] = {
    	0.0f, 0.0f, 10.0f,
    	100.0f, 0.0f, 10.0f
    };
    GLfloat yAxis[] = {
    	0.0f, 0.0f, 10.0f,
    	0.0f, 100.0f, 10.0f
    };
    GLfloat zAxis[] = {
    	0.0f, 0.0f, 10.0f,
    	0.0f, 0.0f, 100.0f
    };

    glm::mat4 MVP = envData->getProjectionMatrix() * envData->getViewMatrix() * glm::mat4();

    glUniform4f(vsColorRef,  1.0f,  0.0f,  0.0f,  1.0f);
    glUniformMatrix4fv(vsModelViewProjectionRef, 1, GL_FALSE, glm::value_ptr(MVP));
    glVertexAttribPointer(vsPositionRef, 3, GL_FLOAT, GL_FALSE, 0, xAxis);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(vsPositionRef);
    checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_LINES, 0, 2);
    checkGlError("glDrawArrays");

    glUniform4f(vsColorRef,  0.0f,  1.0f,  0.0f,  1.0f);
    glUniformMatrix4fv(vsModelViewProjectionRef, 1, GL_FALSE, glm::value_ptr(MVP));
    glVertexAttribPointer(vsPositionRef, 3, GL_FLOAT, GL_FALSE, 0, yAxis);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(vsPositionRef);
    checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_LINES, 0, 2);
    checkGlError("glDrawArrays");

    glUniform4f(vsColorRef,  0.0f,  0.0f,  1.0f,  1.0f);
    glUniformMatrix4fv(vsModelViewProjectionRef, 1, GL_FALSE, glm::value_ptr(MVP));
    glVertexAttribPointer(vsPositionRef, 3, GL_FLOAT, GL_FALSE, 0, zAxis);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(vsPositionRef);
    checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_LINES, 0, 2);
    checkGlError("glDrawArrays");
	*/
	/*
	//calculate FPS
	 struct timespec res;
	 clock_gettime(CLOCK_MONOTONIC, &res);

	 double currentTime = 1000.0*res.tv_sec + (double)res.tv_nsec/1e6;
	 double timeDelta = currentTime-lastTime;
	 lastTime = currentTime;
	 //LOGI("FPS: %f",timeDelta);
	  */
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
