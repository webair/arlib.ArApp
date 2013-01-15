#ifndef AR_ARLIB
#define AR_ARLIB

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>


// ArLib includes
#include "arLib/model/Model.h"
#include "arLib/gles2/Renderer.h"
#include "arLib/EnvironmentData.h"
#include "arLib/Definitions.h"

using namespace std;
using namespace cv;

class ArLib {

public:
	/*
	 * constrctor to instantiate the ArLib. will calculate
	 * viewport for later rendering
	 */
	ArLib(Renderer* r,
			int viewportWidth,
			int viewportHeight,
			int imageWidth,
			int imageHeight,
			float verticalAngle);

	~ArLib();

	/*
	 * sets the camera angle which will be needed to calulate the correct
	 * projection
	 */
	void setVerticalAngle(float angle);

	/*
	 * proecesses the image start the rendering process.
	 */
	void processImage(unsigned char *imageData);

	/*
	 * sets the current device orienation (azimuth, pitch and roll)
	 */
	void setDeviceOrientation(Orientation orientation);

	/*
	 * sets the current device location (latitude, longitude)
	 */
	void setDeviceLocation(Location location);

	/*
	 * method to add a model. will create the needed textures and materials
	 */
	void addModel(Model* m,int materials[], int numberOfMaterials, TextureData* textureData);

	/*
	 * returns the curren environment data object
	 */

	EnvironmentData* getEnvironmentData();

private:
	Renderer *renderer;
	glm::mat4* baseProjection;
	unsigned char* rgbaRaw;
	EnvironmentData* envData;

	void createViewport(float availableWidth, float availableHeight);

};
#endif
