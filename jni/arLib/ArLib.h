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
	ArLib(Renderer* r,
			int viewportWidth,
			int viewportHeight,
			int imageWidth,
			int imageHeight,
			float verticalAngle);

	~ArLib();

	//camera image
	void setVerticalAngle(float angle);


	//actions
	void processImage(unsigned char *imageData);

	//device states
	void setAzimuth(float azimuth);
	void setPitch(float pitch);
	void setRoll(float roll);

	//getters
	vector<Model*>* getModels();
	glm::mat4 getProjection();

private:
	// open gl stuff
	Renderer *renderer;
	EnvironmentData* envData;

	glm::mat4* baseProjection;

	unsigned char* rgbaRaw;



	void createViewport(float availableWidth, float availableHeight);

};
#endif
