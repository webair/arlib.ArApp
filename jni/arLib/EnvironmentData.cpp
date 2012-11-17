#include "arLib/EnvironmentData.h"
#include "util/Logger.h"
#include <glm/gtx/euler_angles.hpp>

EnvironmentData::EnvironmentData()
{

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

void EnvironmentData::setVerticalAngle(float angle) {
	frustumAngle = angle;
	// TODO set this on other place
	frustumNear = 1.0f;
	frustumFar = 100.0f;
	frustumDistanceRatio = tan((frustumAngle*3.141592f/180.0f) * 0.5f);
	this->createBaseProjection();
}

float EnvironmentData::getVerticalAngle()
{
	return frustumAngle;
}

void EnvironmentData::createBaseProjection()
{
	frustumNear = 1.0f;
	frustumFar = 100.0f;
	frustumDistanceRatio = tan((frustumAngle*3.141592f/180.0f) * 0.5f);

    LOGI("setup graphics:");
    LOGI("frustum near: %f", frustumNear);
    LOGI("frustum far: %f", frustumFar);
    LOGI("frustum angle: %f", frustumAngle);
    LOGI("frustum distance ratio: %f", frustumDistanceRatio);

    mat4 projection = perspective(this->getVerticalAngle(), this->getImageRatio(), frustumNear, frustumFar);
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

void EnvironmentData::setRotationMatrix(float *rotationMatrix) {
	// REMINDER: glm is column major, so access to value: mat4[column][row]
	mat4 rotMat = transpose(make_mat4(rotationMatrix));
	//LOGI("glm row1: %f, %f, %f, %f", rotMat[0][0], rotMat[1][0], rotMat[2][0], rotMat[3][0]);
	//LOGI("glm row2: %f, %f, %f, %f", rotMat[0][1], rotMat[1][1], rotMat[2][1], rotMat[3][1]);
	//LOGI("glm row3: %f, %f, %f, %f", rotMat[0][2], rotMat[1][2], rotMat[2][2], rotMat[3][2]);
	this->rotationMatrix = new mat4(rotMat);
}

glm::mat4 EnvironmentData::getProjectionMatrix()
{
	return *this->baseProjection;
	//return proj;
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
