#ifndef AR_MODEL
#define AR_MODEL

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Model {
public:
	Model();
	~Model();

	GLfloat* vertices;
	GLuint verticesSize;
	GLfloat centerPoint[3];

	mat4 getModelMatrix();
	mat4* modelView;
};

class Cube : public Model {

};
#endif
