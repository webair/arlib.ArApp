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
	Model(GLfloat* vnt, int numberOfVNT,
		  GLushort* faces, int numberOfFaces,
		  GLfloat* centerOfGravity, GLfloat* boundingBox,
		  GLfloat northAngle);
	~Model();

	// raw data vertices, normals & textures
	GLfloat* getVNT();

	//faces
	GLuint getNumberOfFaces();
	GLushort* getFaces();

	GLfloat* getCenterOfGravity();

	GLfloat* getBoundingBox();

	//defines the translation relative to 0,0,0 coordinate
	void setWorldMatrix(mat4 modelMatrix);

	//returns all the translation and rotation needed to display object
	mat4 getModelMatrix();


protected:
	GLfloat* vnt;

	GLuint numberOfFaces;
	GLushort *faces;

	GLfloat centerOfGravity[3];
	GLfloat boundingBox[24];

	mat4* modelMatrix;
	//defines the matrxix to move the object to 0 coord and rotate relative to north angle
	mat4* objectMatrix;
};

#endif
