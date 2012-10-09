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



static const char verticeShader[] =
		"attribute vec4 a_position;"
		"attribute vec2 a_texCoord;"
		"uniform mat4 u_projection;"
		"varying vec2 v_texCoord;"
		"varying vec4 v_color;"

		"void main()"
		"{"
		"gl_Position = u_projection * a_position;"
		"v_color = vec4(1.0,0.0,0.0,1.0);"
		"v_texCoord = a_texCoord;"
		"}";

static const char fragmentShader[] =
		"precision mediump float;"
		"uniform sampler2D s_texture;"
		"varying vec4 v_color;"
		"varying vec2 v_texCoord;"
		"void main()\n"
		"{"
		"gl_FragColor = texture2D( s_texture, v_texCoord );"
		"}";

Renderer::Renderer() {
	setupGraphics();
}

Renderer::~Renderer(){
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

void Renderer::setImageSize(int w, int h) {
	this->imageWidth = w;
	this->imageHeight = h;
}

void Renderer::setScreenSize(int w, int h) {
	LOGI("set screen size w:%d h:%d",w,h);
	width=(float)w;
	height=(float)h;
	glViewport(0, 0, w, h);

	float projection[16];
	for (int i=0; i<16; i++) {
		projection[i] = 0.0f;
	}
	float frustumScale = 1.0f;
	float fzNear = 0.5f; float fzFar = 3.0f;

	projection[0] = 0.5f;
	projection[5] = 0.5f;
	projection[10] = 1.0f;
	projection[11] = 1.0f;
	projection[12] = 0.5f;


	glUseProgram(programHandle);
	glUniformMatrix4fv(vsProjectionHandle, 1, GL_FALSE, projection);
	checkGlError("glUniformMatrix4fv");
	glUseProgram(0);


}


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

void Renderer::setupGraphics() {
	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);

	GLuint vShaderRef;
	GLuint fShaderRef;
	GLint linked;

	// Load the vertex/fragment shaders
	vShaderRef = loadShader(GL_VERTEX_SHADER, verticeShader);
	fShaderRef = loadShader(GL_FRAGMENT_SHADER, fragmentShader);

	// Create the program object
	programHandle = glCreateProgram();
	if (programHandle == 0) {
		LOGE("setup graphics failed !!!");
		return;
	}

	glAttachShader(programHandle, vShaderRef);
	glAttachShader(programHandle, fShaderRef);

	// Link the program
	glLinkProgram(programHandle);
	// Check the link status
	glGetProgramiv(programHandle, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint infoLen = 0;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1) {
			LOGE("Error linking program",LOG_TAG);
		}
		glDeleteProgram(programHandle);
		return;
	}
	glUseProgram(programHandle);
	vsPositionHandle = glGetAttribLocation(programHandle, "a_position");
	vsTexCoordHandle = glGetAttribLocation(programHandle, "a_texCoord");
	vsProjectionHandle = glGetUniformLocation(programHandle, "u_projection");
	fsTextureHandle = glGetUniformLocation(programHandle, "s_texture");
	LOGI("projection handle id: %d", vsProjectionHandle);
	// Use tightly packed data
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Generate a texture object
    glGenTextures(1, &textureHandle);
    glGenTextures(1, &textureHandle2);

    LOGI("texture %d, %d loaded", textureHandle, textureHandle2);
    LOGI("texture handle: %d", fsTextureHandle);


}

void Renderer::loadTexture(GLubyte* pixels) {


    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    checkGlError("glActiveTexture");

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    checkGlError("glBindTexture");

     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA,
             GL_UNSIGNED_BYTE, pixels);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

     glBindTexture(GL_TEXTURE_2D, textureHandle2);
     checkGlError("glBindTexture");
     GLubyte testpixels[4 * 4] = { 255, 0, 0, 255, // Red
                     0, 255, 0, 255, // Green
                     0, 0, 255, 255,// Blue
                     255, 255, 0, 255 // Yellow
                     };

      // Load the texture
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA,
                      GL_UNSIGNED_BYTE, testpixels);
      checkGlError("glTexImage2D");

      // Set the filtering mode
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      checkGlError("glGetAttribLocation");

}

//cube
const GLfloat gFrontFace[] = {
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f
};

const GLfloat gBackFace[] = {
		-1.0f, 1.0f, 2.0f,
		-1.0f, -1.0f, 2.0f,
        1.0f, -1.0f, 2.0f,
        1.0f, -1.0f, 2.0f,
        1.0f, 1.0f, 2.0f,
        -1.0f, 1.0f, 2.0f
};


const GLfloat gLeftFace[] = {
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 2.0f,
        -1.0f, 1.0f, 2.0f,
        -1.0f, -1.0f, 2.0f,
        -1.0f, -1.0f, 1.0f
};

const GLfloat gRightFace[] = {
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 2.0f,
        1.0f, 1.0f, 2.0f,
        1.0f, -1.0f, 2.0f,
        1.0f, -1.0f, 1.0f
};
/*
const GLfloat gTexCoords[] = {
		-0.5f, 0.5f,
		-0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f
};
*/
const GLfloat gTexCoords[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
};

void Renderer::renderFrame() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(programHandle);
    checkGlError("glUseProgram");

    glVertexAttribPointer(vsPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, gFrontFace);
    checkGlError("glVertexAttribPointer");
    glVertexAttribPointer(vsTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, gTexCoords);
    checkGlError("glVertexAttribPointer");

    glEnableVertexAttribArray(vsPositionHandle);
    checkGlError("glEnableVertexAttribArray");

    glEnableVertexAttribArray(vsTexCoordHandle);
    checkGlError("glEnableVertexAttribArray");

    // Bind the texture
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, textureHandle);

    // Set the sampler texture unit to 0
    glUniform1i(fsTextureHandle, 0);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    checkGlError("glDrawArrays");

    glVertexAttribPointer(vsPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, gLeftFace);
    checkGlError("glVertexAttribPointer");
    glBindTexture(GL_TEXTURE_2D, textureHandle2);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    checkGlError("glDrawArrays");

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
