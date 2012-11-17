
#include "Model.h"

#include "util/Logger.h"

using namespace std;

Model::Model(GLfloat* vnt, int numberOfVNT,
		  GLushort* faces,int numberOfFaces,
		  GLfloat* centerOfGravity, GLfloat* boundingBox,
		  GLfloat northAngle)
{
	this->vnt = (GLfloat *) new GLfloat[numberOfVNT];

	for (int i = 0; i < numberOfVNT; i++) {
		this->vnt[i] = vnt[i];
	}

	this->numberOfFaces = numberOfFaces;
	this->faces = (GLushort *) new GLushort[numberOfFaces];

	for (int i = 0; i < numberOfFaces; i++) {
		this->faces[i] = faces[i];
	}

	for (int i = 0; i < 24; i++) {
		this->boundingBox[i] = boundingBox[i];
	}

	for (int i = 0; i < 3; i++) {
		this->centerOfGravity[i] = centerOfGravity[i];
	}
	mat4 objectMatrix = translate(mat4(), vec3(-this->centerOfGravity[0],
			-this->centerOfGravity[1],
			-this->centerOfGravity[2]));
	objectMatrix = rotate(objectMatrix, northAngle, vec3(0.0f, 1.0f, 0.0f));

	this->objectMatrix = new mat4(objectMatrix);

}

Model::~Model() {
	delete [] vnt;  // When done, free memory pointed to by a
	vnt = NULL;
}

GLfloat* Model::getVNT() {
	return this->vnt;
}

GLuint Model::getNumberOfFaces() {
	return this->numberOfFaces;

}
GLushort* Model::getFaces() {
	return this->faces;
}

void Model::setWorldMatrix(mat4 matrix) {
	this->modelMatrix = new mat4(matrix);
}

glm::mat4 Model::getModelMatrix() {
	return (*this->modelMatrix) * (*this->objectMatrix);
}
