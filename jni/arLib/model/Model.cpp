
#include "Model.h"

#include "util/Logger.h"

using namespace std;

Model::Model(int modelId, GLfloat* vnt, int numberOfVNT,
		  GLushort* faces,int numberOfFaces,
		  GLfloat* centerOfGravity, GLfloat* boundingBox,
		  GLfloat northAngle, Location location
		  )
{
	this->location = location;
	LOGI("lat, lon: %f, %f", this->getLocation().latitude, this->getLocation().longitude);
	this->vnt = (GLfloat *) new GLfloat[numberOfVNT];

	this->numberOfVNT = numberOfVNT;
	for (int i = 0; i < numberOfVNT; i++) {

		if (i%9==0) {
			this->vnt[i] = vnt[i];
			this->vnt[i+1] = vnt[i+1];
			this->vnt[i+2] = vnt[i+2];

			this->vnt[i+3] = vnt[i+3];
			this->vnt[i+4] = vnt[i+4];
			this->vnt[i+5] = vnt[i+5];

			this->vnt[i+6] = vnt[i+6];
			this->vnt[i+7] = vnt[i+7];
			this->vnt[i+8] = vnt[i+8];
		}


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

	LOGI("cog: %f, %f, %f", this->centerOfGravity[0], this->centerOfGravity[1], this->centerOfGravity[2]);


	mat4 rotateX = rotate(mat4(), -90.0f, vec3(1.0f, 0.0f, 0.0f));
	mat4 rotateY = rotate(mat4(), northAngle, vec3(0.0f, 1.0f, 0.0f));
	mat4 objectMatrix = translate(mat4(), vec3(-this->centerOfGravity[0],
			-this->centerOfGravity[1],
			-this->centerOfGravity[2]));
	mat4 totRot = rotateY * rotateX;
	objectMatrix = totRot * objectMatrix;
	this->objectMatrix = new mat4(objectMatrix);
}

Model::~Model() {
	delete [] vnt;  // When done, free memory pointed to by a
	vnt = NULL;
}

GLuint Model::getNumberOfVNT() {
	return this->numberOfVNT;
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

Location Model::getLocation() {
	return this->location;
}
void Model::setNumberOfMaterials(int nr)
{
	this->numberOfMaterials = nr;
}
GLuint Model::getNumberOfMaterials()
{
	return this->numberOfMaterials;
}
void Model::setMaterials(Material* mats)
{
	this->materials = mats;
}
Material* Model::getMaterials()
{
	return this->materials;
}
