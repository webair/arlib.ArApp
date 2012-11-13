
#include "Model.h"

#include "util/Logger.h"

using namespace std;

Model::Model() {
	numberOfVertices = 8;
	vertices = (GLfloat *) new GLfloat[numberOfVertices * 3];

	numberOfFaces = 36;
	faces = (GLushort *) new GLushort[numberOfFaces];

	centerOfGravity[0] = 0.0f;
	centerOfGravity[1] = 0.0f;
	centerOfGravity[2] = 0.0f;

	GLfloat values[] = {-1.0f, 1.0f, 1.0f, -1.0f};
	GLfloat vertice_value[3];

	for (int i= 0; i<4; i++) {
		int x = i*3;
		int y = x+1;
		int z = y+1;
		vertices[x] = values[i];
		vertices[y] = values[(i+1)%4];
		vertices[z] = 1.0f;
	}

	for (int i= 12; i<24; i++) {
		int x = i*3;
		int y = x+1;
		int z = y+1;
		vertices[x] = values[i];
		vertices[y] = values[(i+1)%4];
		vertices[z] = -1.0f;
	}
	//front
	faces[0] = 0;
	faces[1] = 1;
	faces[2] = 2;
	faces[3] = 2;
	faces[4] = 3;
	faces[5] = 0;

	//left
	faces[6] = 0;
	faces[7] = 4;
	faces[8] = 7;
	faces[9] = 7;
	faces[10] = 3;
	faces[11] = 0;

	//back
	faces[12] = 7;
	faces[13] = 6;
	faces[14] = 5;
	faces[15] = 5;
	faces[16] = 4;
	faces[17] = 7;

	//right
	faces[18] = 2;
	faces[19] = 6;
	faces[20] = 5;
	faces[21] = 5;
	faces[22] = 1;
	faces[23] = 2;

	//top
	faces[24] = 3;
	faces[25] = 2;
	faces[26] = 7;
	faces[27] = 7;
	faces[28] = 6;
	faces[29] = 2;

	//bottom
	faces[30] = 0;
	faces[31] = 1;
	faces[32] = 4;
	faces[33] = 4;
	faces[34] = 5;
	faces[35] = 1;



}

Model::~Model() {
	delete [] vertices;  // When done, free memory pointed to by a
	vertices = NULL;
}

void Model::setModelMatrix(mat4 matrix) {
	this->modelMatrix = new mat4(matrix);

}

glm::mat4 Model::getModelMatrix() {
	return *this->modelMatrix;
}

GLuint Model::getNumberOfVertices() {
	return this->numberOfVertices;
}
GLfloat* Model::getVertices() {
	return this->vertices;

}

GLuint Model::getNumberOfFaces() {
	return this->numberOfFaces;

}
GLushort* Model::getFaces() {
	return this->faces;

}
GLfloat* getCenterOfGravity();
