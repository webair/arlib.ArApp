#include "ArLib.h"
#include "util/Logger.h"
#include "arLib/util/GeoMath.h"
#include "arLib/Constants.h"

using namespace glm;

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
	envData->setVerticalAngle(verticalAngle, FRUSTUM_NEAR, FRUSTUM_FAR);

	createViewport((float) viewportWidth, (float) viewportHeight);

	// create camera texture
	envData->setCameraTextureReference(r->generateTexture());
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

	LOGI("screen width: %f", availableWidth);
	LOGI("screen height: %f", availableHeight);
	LOGI("viewport width: %f", viewPortWidth);
	LOGI("viewport height: %f", viewPortHeight);

	renderer->setViewport(
			(availableWidth-viewPortWidth)/2,
			(availableHeight-viewPortHeight)/2,
			viewPortWidth,
			viewPortHeight
	);
}

void ArLib::setDeviceOrientation(Orientation orientation) {
	envData->setDeviceOrientation(orientation);
}

void ArLib::setDeviceLocation(Location location) {
	envData->setDeviceLocation(location);
}

void ArLib::processImage(unsigned char *imageData)
{
	//convert camera image
	Dimension imageDimension = envData->getImageDimension();
	int imageWidth = (int) imageDimension.width;
	int imageHeight = (int) imageDimension.height;
	Mat yuv(imageHeight + imageHeight/2, imageWidth, CV_8UC1, (unsigned char *)imageData);
	unsigned char* rgbaRaw = (unsigned char *)malloc(envData->getImageRGBASize() * sizeof(unsigned char));
	Mat rgba(imageHeight, imageWidth, CV_8UC4, rgbaRaw);
	cvtColor(yuv, rgba, CV_YUV420sp2RGBA);

	// display camera image
	renderer->loadTexture(envData->getCameraTextureReference(),rgbaRaw, imageDimension.width, imageDimension.height);

	Location rathuusgass57;
	rathuusgass57.latitude = 46.94847f;
	rathuusgass57.longitude = 7.45024f;

	Location vorDemBundeshaus;
	vorDemBundeshaus.latitude = 46.947337f;
	vorDemBundeshaus.longitude = 7.443957f;


	// position models
	vector<Model*> *models = envData->getModels();
    for(vector<Model*>::size_type i = 0; i != models->size(); i++) {
    	Model* m = models->at(i);
    	float d = GeoMath::getDistance(envData->getDeviceLocation(),m->getLocation());
    	float a = GeoMath::getAngle(envData->getDeviceLocation(), m->getLocation());

    	mat4 view = rotate(mat4(), -a, vec3(0.0f, 1.0f, 0.0f));
    	view = translate(view, vec3(0.0f,0.0f, -d));
    	view = rotate(view, a, vec3(0.0f, 1.0f, 0.0f));
    	m->setWorldMatrix(view);
    }


    GLubyte *searchPattern = renderer->createSearchPattern(envData);
	free(searchPattern);

	renderer->renderFrame(envData);
	free(rgbaRaw);
}

void ArLib::addModel(Model* m,int materials[], int numberOfMaterials, TextureData* textureData) {
	Material *mats = new Material[numberOfMaterials];
	for (int i=0; i < numberOfMaterials; i++) {
		Material material;
		int startIndex =  materials[i*2];
		int length = materials[(i*2)+1] - startIndex;
		LOGI("Mat Nr, start, end, length: %d, %d, %d, %d", i, startIndex, materials[(i*2)+1], length);
		TextureData texData = textureData[i];
		GLuint texRef = renderer->generateTexture();

		renderer->loadTexture(texRef, texData.byteData, texData.width, texData.height);
		material.textureReference = texRef;
		material.startIndex = startIndex;
		material.length = length;
		LOGI("loaded material: texRef=%d, startIndex=%d, length=%d", material.textureReference, material.startIndex, material.length);
		mats[i] = material;
	}
	m->setMaterials(mats);
	m->setNumberOfMaterials(numberOfMaterials);
	envData->addModel(m);
}
