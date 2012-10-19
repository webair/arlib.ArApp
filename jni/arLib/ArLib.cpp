#include "ArLib.h"
#include "util/Logger.h"


ArLib::ArLib(Renderer* r,
		int viewportWidth,
		int viewportHeight,
		int imageWidth,
		int imageHeight,
		float verticalAngle)
{
	renderer = r;
	this->imageWidth = (float) imageWidth;
	this->imageHeight = (float) imageHeight;
	this->imageRatio = imageWidth / imageHeight;
	this->frustumAngle = verticalAngle;

	createViewport((float) viewportWidth, (float) viewportHeight);
	createBaseProjection();
}

void ArLib::createViewport(float availableWidth, float availableHeight)
{
	float viewPortWidth = availableWidth;
	float viewPortHeight = availableHeight;

	//convert height propotion, so that the image does not get
	if (viewPortHeight > imageHeight) {
		viewPortHeight = imageHeight;
	}
	viewPortWidth = (imageWidth / imageHeight) * viewPortHeight;
	if (viewPortWidth > imageWidth) {
		viewPortWidth = imageWidth;
		viewPortHeight = (imageHeight / imageWidth) * viewPortWidth;
	}

	LOGI("image width: %f", imageWidth);
	LOGI("image width: %f", imageHeight);
	LOGI("image ratio: %f", imageRatio);
	LOGI("viewport width: %f", viewPortWidth);
	LOGI("viewport height: %f", viewPortHeight);

	renderer->setViewport(
			(availableWidth-viewPortWidth)/2,
			(availableHeight-viewPortHeight)/2,
			viewPortWidth,
			viewPortHeight
	);
}

void ArLib::createBaseProjection()
{
	frustumNear = 1.0f;
	frustumFar = 100.0f;
	frustumDistanceRatio = tan((frustumAngle*3.141592f/180.0f) * 0.5f);

    LOGI("setup graphics:");
    LOGI("frustum near: %f", frustumNear);
    LOGI("frustum far: %f", frustumFar);
    LOGI("frustum angle: %f", frustumAngle);
    LOGI("frustum distance ratio: %f", frustumDistanceRatio);

    glm::mat4 projection = glm::perspective(frustumAngle, imageRatio, frustumNear, frustumFar);
    baseProjection = new glm::mat4(projection);
}
