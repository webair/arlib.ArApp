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
		"varying vec2 v_texCoord;"
		"void main()"
		"{"
			"gl_Position = a_position;"
			"v_texCoord = a_texCoord;"
		"}";

static const char cameraFragmentShader[] =
		"precision mediump float;"
		"uniform sampler2D s_texture;"
		"varying vec2 v_texCoord;"
		"void main()\n"
		"{"
			"gl_FragColor = texture2D( s_texture, v_texCoord );"
		"}";



static const char objectVerticeShader[] =

		"attribute vec4 a_position;"
		"attribute vec2 a_texCoord;"
		"attribute vec4 a_normal;"
		"uniform mat4 u_projectionView;"
		"uniform mat4 u_modelView;"
		"uniform vec4 u_color;"
		"varying vec2 v_texCoord;"
		"varying vec4 v_color;"
		"varying vec4 diffuse;"
		"varying float depth;"

		"void main()"
		"{"
			"vec4 newNormal = u_projectionView * u_modelView * a_normal;"
			"vec4 newPosition = u_projectionView * u_modelView * a_position;"

			//diffuse loight calculation
			"vec3 s = normalize(newPosition.xyz-vec3(0.0, 200.0, -300.0));"
			"v_color = u_color * max(0.0, dot(normalize(newNormal.xyz), s));"

			"gl_Position = newPosition;"
			//"lightintensity = 1.0;"
			//"v_color = u_color;"

			/*
			"gl_Position = u_modelViewProjection * a_position;"
			"vec4 l_normal = u_modelViewProjection * a_normal;"
			"vec3 l_lightSource = vec3(0.0, 0.0, 0.0);"
			"vec3 lightVector = normalize(l_lightSource - gl_Position.xyz);"
			"float distance = length(lightVector);"
			"float fCosine = dot(l_normal.xyz, normalize(lightVector));"
			"float fLamber = max(fCosine,0.0);"
			"v_color = normalize(vec4(u_color.xyz * fLamber * (1.0 / (1.0 + (0.3 * distance * distance))),1.0));"
			"v_texCoord = a_texCoord;"
			//"v_normal = l_normal.xyz;"
			*/
		"}"
		;

		/*
		"precision highp float;"

		// Matrices
		//"uniform mat4 u_modelViewProjection;"
		"uniform mat4 u_modelViewProjection;"

		// Attributes
		"attribute vec4 a_position;"
		"attribute vec3 a_normal;"

		// Varyings
		"varying vec3 v_normal;"

		"void main() "
		"{"

		    // Define position and normal in model coordinates
		    "vec4 mcPosition = a_position;"
		    "vec3 mcNormal = a_normal;"

		    // Calculate and normalize eye space normal
		    "vec3 ecNormal = vec3(u_modelViewProjection * vec4(mcNormal, 0.0));"
		    "ecNormal = ecNormal / length(ecNormal);"
		    "v_normal = a_normal;"

		    "gl_Position = u_modelViewProjection * mcPosition;"
		"}"
		;
		*/

static const char objectFragmentShader[] =

		"precision mediump float;"
		"varying vec4 v_color;"
		"varying float depth;"
		"void main()\n"
		"{"
		"gl_FragColor = vec4(v_color.rgb,1.0);"
		//"gl_FragColor = vec3((v_color*lightintensity*0.1).rgb);"
			//"vec4 ambient = gl_FrontMaterial.ambient;"
			//"gl_FragColor = v_color;"
		"}"
		;






		/*
		"precision highp float;"

		"struct DirectionalLight "
		"{"
		    "vec3 direction;"
		    "vec3 halfplane;"
		    "vec4 ambientColor;"
		    "vec4 diffuseColor;"
		    "vec4 specularColor;"
		"}"
		";"

		"struct Material"
		"{"
		    "vec4 ambientFactor;"
		    "vec4 diffuseFactor;"
		    "vec4 specularFactor;"
		    "float shininess;"
		"};"

		// Light
		"uniform DirectionalLight u_directionalLight;"

		// Material
		"uniform Material u_material;"

		"varying vec3 v_normal;"

		"void main() "
		"{"

		    // Normalize v_normal
		    "vec3 ecNormal = v_normal / length(v_normal);"

		    "float ecNormalDotLightDirection = max(0.0, dot(ecNormal, u_directionalLight.direction));"
		    "float ecNormalDotLightHalfplane = max(0.0, dot(ecNormal, u_directionalLight.halfplane));"

		    // Calculate ambient light
		    "vec4 ambientLight = u_directionalLight.ambientColor * u_material.ambientFactor;"

		    // Calculate diffuse light
		    "vec4 diffuseLight = ecNormalDotLightDirection * u_directionalLight.diffuseColor * u_material.diffuseFactor;"

		    // Calculate specular light
		    "vec4 specularLight = vec4(0.0);"
		    "if (ecNormalDotLightHalfplane > 0.0) "
		    "{"
		       "specularLight = pow(ecNormalDotLightHalfplane, u_material.shininess) * u_directionalLight.specularColor * u_material.specularFactor;"
		    "}"

		    "vec4 light = ambientLight + diffuseLight + specularLight;"

		    "gl_FragColor = light;"
		"}"
		;
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
	// 10 should be enough for the moment...
	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);

	GLuint cameraVerticeShaderRef;
	GLuint cameraFragmentShaderRef;

	GLuint objectVerticeShaderRef;
	GLuint objectFragmentShaderRef;

	// Load the vertex/fragment shaders
	cameraVerticeShaderRef = loadShader(GL_VERTEX_SHADER, cameraVerticeShader);
	cameraFragmentShaderRef = loadShader(GL_FRAGMENT_SHADER, cameraFragmentShader);
	objectVerticeShaderRef = loadShader(GL_VERTEX_SHADER, objectVerticeShader);
	objectFragmentShaderRef = loadShader(GL_FRAGMENT_SHADER, objectFragmentShader);



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


	vsCameraPositionRef = glGetAttribLocation(cameraProgramRef, "a_position");
	vsPositionRef = glGetAttribLocation(objectProgramRef, "a_position");
	vsNormalRef = glGetAttribLocation(objectProgramRef, "a_normal");
	LOGI("refpost: %d, normalred: %d, cameraPosRef: %d", vsPositionRef, vsNormalRef, vsCameraPositionRef);
	vsTexCoordHandle = glGetAttribLocation(cameraProgramRef, "a_texCoord");
    fsCameraTextureRef = glGetUniformLocation(cameraProgramRef, "s_texture");
    vsModelViewRef = glGetUniformLocation(objectProgramRef, "u_modelView");
    vsProjectionViewRef = glGetUniformLocation(objectProgramRef, "u_projectionView");
    vsColorRef = glGetUniformLocation(objectProgramRef, "u_color");

    glUseProgram(objectProgramRef);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glDepthMask( true );

    //glClearDepthf(1.0f);

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

GLfloat testQuad[] = {
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
};

GLushort test[] = {0,1,2};
void Renderer::renderFrame(EnvironmentData *envData) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(cameraProgramRef);
    checkGlError("glUseProgram");


    glVertexAttribPointer(vsCameraPositionRef, 3, GL_FLOAT, GL_FALSE, 0, gCameraQuad);
    checkGlError("glVertexAttribPointer");
    glVertexAttribPointer(vsTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, gCameraTexCoord);
    checkGlError("glVertexAttribPointer");

    glEnableVertexAttribArray(vsCameraPositionRef);
    checkGlError("glEnableVertexAttribArray");

    glEnableVertexAttribArray(vsTexCoordHandle);
    checkGlError("glEnableVertexAttribArray");

    // Set the sampler texture unit to 0
    glUniform1i(fsCameraTextureRef, 0);
    checkGlError("fsCameraTextureRef");
    glBindTexture(GL_TEXTURE_2D, envData->cameraTextrueRef);
    checkGlError("glBindTexture");
    glDrawArrays(GL_TRIANGLES, 0, 6);
    checkGlError("glDrawArrays");

    //glClear( GL_DEPTH_BUFFER_BIT);
    glUseProgram(objectProgramRef);
    glClear( GL_DEPTH_BUFFER_BIT);

    vector<Model*> *models = envData->getModels();

    bool showNormals = false;
    glm::mat4 PM = envData->getProjectionMatrix() * envData->getViewMatrix();
    glUniformMatrix4fv(vsProjectionViewRef, 1, GL_FALSE, glm::value_ptr(PM));

    for(vector<Model*>::size_type i = 0; i != models->size(); i++) {
    	Model* m = models->at(i);
    	glUniformMatrix4fv(vsModelViewRef, 1, GL_FALSE, glm::value_ptr(m->getModelMatrix()));

        glUniform4f(vsColorRef,  0.0f,  1.0f,  1.0f,  1.0f);


    	//GLfloat *blubi = m->getVNT();
    	//LOGI("%f %f %f", blubi[0], blubi[1], blubi[2]);


        if (!showNormals) {
			glVertexAttribPointer(vsPositionRef, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, m->getVNT());
			checkGlError("glVertexAttribPointer");

			//glVertexAttribPointer(vsNormalRef, 3, GL_FLOAT, GL_FALSE, 0, m->getNormals());
			glVertexAttribPointer(vsNormalRef, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, m->getVNT()+3);
			checkGlError("glVertexAttribPointer");

			glEnableVertexAttribArray(vsPositionRef);
			checkGlError("glEnableVertexAttribArray");
			glDrawElements(GL_TRIANGLES, m->getNumberOfFaces(), GL_UNSIGNED_SHORT, m->getFaces());
			//glDrawArrays(GL_TRIANGLES, 0, 3);
			checkGlError("glDrawElemnts");
        } else {
        	//draw normals
        	/*
    		float line[] = {-1.0f, -1.0f, 1.0f,//0.0f, 0.0f, 1.0f,
    						1.0f, 0.0f, 0.0f,
    					   };
    		float normals[] = {0.0f, 0.0f, 1.0f,
    							0.0f, 0.0f, 1.0f,
    					   };

        	glVertexAttribPointer(vsPositionRef, 3, GL_FLOAT, GL_FALSE, 0, line);
			checkGlError("glVertexAttribPointer normal position");
        	glVertexAttribPointer(vsNormalRef, 3, GL_FLOAT, GL_FALSE, 0, normals);
        	checkGlError("glVertexAttribPointer normal normals");

        	GLushort index[] = {0,1};
			glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, index);
			checkGlError("glDrawElements normal");
			*/


        	for (int i = 0; i < m->getNumberOfVNT(); i++) {
        		if (i%9 == 0) {

        			float scale = 4.0f;

					float xPos = m->getVNT()[i];
					float yPos = m->getVNT()[i + 1];
					float zPos = m->getVNT()[i + 2];

					float xNorm = m->getVNT()[i + 3] * scale;
					float yNorm = m->getVNT()[i + 4] * scale;
					float zNorm = m->getVNT()[i + 5] * scale;

					//glm::vec3 normale = glm::normalize(vec3(xNorm, yNorm, zNorm));
					//LOGI("xPos & xNorm : %f & %f",xPos, xNorm * 30.0f);

					GLfloat line[] = {xPos, yPos, zPos,//0.0f, 0.0f, 1.0f,
							xPos + xNorm, yPos + yNorm, zPos + zNorm
								   };
					GLfloat normals[] = {0.0f, 0.0f, 1.0f,
										0.0f, 0.0f, 1.0f
								   };
					glVertexAttribPointer(vsPositionRef, 3, GL_FLOAT, GL_FALSE, 0, line);
					checkGlError("glVertexAttribPointer normal position");
					//glVertexAttribPointer(vsNormalRef, 3, GL_FLOAT, GL_FALSE, 0, normals);
					//checkGlError("glVertexAttribPointer normal normals");

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

void Renderer::printGLString(const char *name, GLenum s) {
	const char *v = (const char *) glGetString(s);
	LOGI("GL %s = %s\n", name, v);

}

void Renderer::checkGlError(const char* op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		LOGI("after %s() glError (0x%x)\n", op, error);
	}
}
