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
	frustumFar = 1000.0f;
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

glm::mat4 EnvironmentData::getProjectionMatrix()
{
	return *this->baseProjection;
	//return proj;
}

glm::mat4 EnvironmentData::getViewMatrix()
{

	//glm::vec3 lookAt =  vec3(0.0f,0.0f,1.0f) * glm::rotate( mat4(1.0f), -deviceOrientation.azimuth, vec3(0.0f,1.0f,0.0f));
	/*
	glm::mat4 view = glm::lookAt(
	    glm::vec3(0.0f, 100.0f, 0.0f), // camera position (in our case 0,0,0)
	    glm::vec3(0.0f, 0.0f, 0.0f), // look at position
	    glm::vec3(0.0f, 1.0f, 0.0f)  // Head is up (set to 0,-1,0 to look upside-down)
	);
*/
	//mat4 view = glm::yawPitchRoll(-deviceOrientation.azimuth, -deviceOrientation.pitch, -deviceOrientation.roll);
	mat4 view = mat4(1.0f);
	view = rotate(view, deviceOrientation.azimuth, glm::vec3(0.0f, 1.0f, 0.0f));
	view = rotate(view, deviceOrientation.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	view = rotate(view, deviceOrientation.roll, glm::vec3(0.0f, 0.0f, 1.0f));


	//mat4 view = translate(mat4(1.0f),vec3(-5.0f, 0.0f, 0.0f));
	return view;
}
