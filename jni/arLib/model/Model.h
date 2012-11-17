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

	// raw data
	GLuint getNumberOfVertices();
	GLfloat* getVertices();
	GLfloat* getNormals();
	GLuint getNumberOfFaces();
	GLushort* getFaces();
	GLfloat* getCenterOfGravity();

	// object modeling
	void setModelMatrix(mat4 matrix);
	mat4 getModelMatrix();


protected:
	GLuint numberOfVertices;
	GLfloat* vertices;
	GLfloat* normals;
	GLuint numberOfFaces;
	GLushort *faces;
	GLfloat centerOfGravity[3];

	mat4* modelMatrix;
	mat4* objectMatrix;


};

class Cube : public Model {

};
#endif
