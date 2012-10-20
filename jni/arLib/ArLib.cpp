#include "ArLib.h"
#include "util/Logger.h"



ArLib::ArLib(Renderer* r,
		int viewportWidth,
		int viewportHeight,
		int imageWidth,
		int imageHeight,
		float verticalAngle)
{
	this->envData = new EnvironmentData();
	renderer = r;
	Dimension dimension;
	dimension.width = (float) imageWidth;
	dimension.height = (float) imageHeight;
	envData->setImageDimension(dimension);
	envData->setVerticalAngle(verticalAngle);

	createViewport((float) viewportWidth, (float) viewportHeight);

	// create camera texture
	envData->cameraTextrueRef = r->generateTexture();

	LOGI("generate camera texture: ref=%d", envData->cameraTextrueRef);
}

void ArLib::createViewport(float availableWidth, float availableHeight)
{
	float viewPortWidth = availableWidth;
	float viewPortHeight = availableHeight;
	Dimension imageDimension = envData->getImageDimension();

	//convert height propotion, so that the image does not get
	if (viewPortHeight > imageDimension.width) {
		viewPortHeight = imageDimension.height;
	}
	viewPortWidth = envData->getImageRatio() * viewPortHeight;
	if (viewPortWidth > imageDimension.width) {
		viewPortWidth = imageDimension.width;
		viewPortHeight = (imageDimension.height / imageDimension.width) * viewPortWidth;
	}

	LOGI("image width: %f", imageDimension.width);
	LOGI("image width: %f", imageDimension.height);
	LOGI("image ratio: %f", envData->getImageRatio());
	LOGI("image RGBA size: %d", envData->getImageRGBASize());

	renderer->setViewport(
			(availableWidth-viewPortWidth)/2,
			(availableHeight-viewPortHeight)/2,
			viewPortWidth,
			viewPortHeight
	);
}

void ArLib::processImage(unsigned char *imageData)
{
	//TODO optimize
	Dimension imageDimension = envData->getImageDimension();

	int imageWidth = (int) imageDimension.width;
	int imageHeight = (int) imageDimension.height;

	Mat yuv(imageHeight + imageHeight/2, imageWidth, CV_8UC1, (unsigned char *)imageData);
	unsigned char* rgbaRaw = (unsigned char *)malloc(envData->getImageRGBASize() * sizeof(unsigned char));
	Mat rgba(imageHeight, imageWidth, CV_8UC4, rgbaRaw);
	cvtColor(yuv, rgba, CV_YUV420sp2RGBA);

	renderer->loadTexture(envData->cameraTextrueRef,rgbaRaw, imageDimension.width, imageDimension.height);
	renderer->renderFrame(envData);
	free(rgbaRaw);
}
