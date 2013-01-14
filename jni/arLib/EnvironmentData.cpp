#include "arLib/EnvironmentData.h"
#include "util/Logger.h"
#include <glm/gtx/euler_angles.hpp>

EnvironmentData::EnvironmentData()
{
	models = new vector<Model*>;
}

EnvironmentData::~EnvironmentData()
{
	delete models;
}

void EnvironmentData::setCameraTextureReference(GLuint reference)
{
	this->cameraTextrueRef = reference;

}
GLuint EnvironmentData::getCameraTextureReference() {
	return this->cameraTextrueRef;
}

void EnvironmentData::setImageDimension(Dimension dimension)
{
	imageDimension = dimension;
	imageRatio = imageDimension.width / imageDimension.height;
	imageRGBASize = (int)dimension.width * (int)dimension.height * 4;
}

Dimension EnvironmentData::getImageDimension()
{
	return imageDimension;
}

float EnvironmentData::getImageRatio()
{
	return imageRatio;
}

int EnvironmentData::getImageRGBASize()
{
	return imageRGBASize;
}

void EnvironmentData::setVerticalAngle(float angle, float near, float far) {
	frustumAngle = angle;
	frustumNear = near;
	frustumFar = far;
	this->createBaseProjection();
}

float EnvironmentData::getVerticalAngle()
{
	return frustumAngle;
}

void EnvironmentData::createBaseProjection()
{
	frustumNear = 1.0f;
	frustumFar = 1000.0f;

    mat4 projection = perspective(frustumAngle, this->getImageRatio(), frustumNear, frustumFar);
    baseProjection = new mat4(projection);
}

void EnvironmentData::setDeviceOrientation(Orientation orientation)
{
	deviceOrientation = orientation;
}

Orientation EnvironmentData::getDeviceOrientation()
{
	return deviceOrientation;
}

glm::mat4 EnvironmentData::getProjectionMatrix()
{
	return *this->baseProjection;
}

glm::mat4 EnvironmentData::getViewMatrix()
{
	return rotate(rotate(rotate(mat4(), (deviceOrientation.roll * 180.0f) / 3.14159265f, vec3(0.0f, 0.0f, 1.0f)), (deviceOrientation.pitch * 180.0f) / 3.14159265f, vec3(1.0f, 0.0f, 0.0f)), (deviceOrientation.azimuth * 180.0f) / 3.14159265f, vec3(0.0f, 1.0f, 0.0f));
}

void EnvironmentData::setDeviceLocation(Location location) {
	this->deviceLocation = location;
}

Location EnvironmentData::getDeviceLocation() {
	return this->deviceLocation;
}

// adding models
void EnvironmentData::addModel(Model* m) {
	models->push_back(m);
}

vector<Model*>* EnvironmentData::getModels() {
	return models;
}
