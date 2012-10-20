#include "arLib/EnvironmentData.h"
#include "util/Logger.h"

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

glm::mat4 EnvironmentData::getProjection()
{
	return mat4(1.0);
}
