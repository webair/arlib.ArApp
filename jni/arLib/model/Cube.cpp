
#include "Model.h"

#include "util/Logger.h"

using namespace std;

GLushort a_indices[]  = { 0, 1, 2,   2, 3, 0,      // front
                       4, 5, 6,   6, 7, 4,      // right
                       8, 9,10,  10,11, 8,      // top
                      12,13,14,  14,15,12,      // left
                      16,17,18,  18,19,16,      // bottom
                      20,21,22,  22,23,20 };    // back

GLfloat a_vertices[] = {
						1, 1, 1,   0, 0, 1,   1, 1, 1,              // v0 (front)
                       -1, 1, 1,   0, 0, 1,   1, 1, 0,              // v1
                       -1,-1, 1,   0, 0, 1,   1, 0, 0,              // v2
                        1,-1, 1,   0, 0, 1,   1, 0, 1,              // v3

                        1, 1, 1,   1, 0, 0,   1, 1, 1,              // v0 (right)
                        1,-1, 1,   1, 0, 0,   1, 0, 1,              // v3
                        1,-1,-1,   1, 0, 0,   0, 0, 1,              // v4
                        1, 1,-1,   1, 0, 0,   0, 1, 1,              // v5

                        1, 1, 1,   0, 1, 0,   1, 1, 1,              // v0 (top)
                        1, 1,-1,   0, 1, 0,   0, 1, 1,              // v5
                       -1, 1,-1,   0, 1, 0,   0, 1, 0,              // v6
                       -1, 1, 1,   0, 1, 0,   1, 1, 0,              // v1

                       -1, 1, 1,  -1, 0, 0,   1, 1, 0,              // v1 (left)
                       -1, 1,-1,  -1, 0, 0,   0, 1, 0,              // v6
                       -1,-1,-1,  -1, 0, 0,   0, 0, 0,              // v7
                       -1,-1, 1,  -1, 0, 0,   1, 0, 0,              // v2

                       -1,-1,-1,   0,-1, 0,   0, 0, 0,              // v7 (bottom)
                        1,-1,-1,   0,-1, 0,   0, 0, 1,              // v4
                        1,-1, 1,   0,-1, 0,   1, 0, 1,              // v3
                       -1,-1, 1,   0,-1, 0,   1, 0, 0,              // v2

                        1,-1,-1,   0, 0,-1,   0, 0, 1,              // v4 (back)
                       -1,-1,-1,   0, 0,-1,   0, 0, 0,              // v7
                       -1, 1,-1,   0, 0,-1,   0, 1, 0,              // v6
                        1, 1,-1,   0, 0,-1,   0, 1, 1 };            // v5

Model::Model() {
	numberOfVertices = 36;
	vertices = (GLfloat *) new GLfloat[216];

	for (int i = 0; i < 36*3*3; i++) {
		vertices[i] = a_vertices[i];
	}

	numberOfFaces = 36;
	faces = (GLushort *) new GLushort[numberOfFaces];

	for (int i = 0; i < 36; i++) {
		faces[i] = a_indices[i];
	}

	centerOfGravity[0] = 0.0f;
	centerOfGravity[1] = 0.0f;
	centerOfGravity[2] = 0.0f;



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
