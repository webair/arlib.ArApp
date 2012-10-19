#ifndef AR_ARLIB
#define AR_ARLIB

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "arLib/model/Model.h"
#include "arLib/gles2/Renderer.h"
#include <vector>

using namespace std;

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
	void processImage(unsigned char* imageData);

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
	float frustumNear;
	float frustumFar;
	float frustumAngle;
	float frustumDistanceRatio;

	glm::mat4* baseProjection;

	//image stuff
	float imageWidth;
	float imageHeight;
	float imageRatio;
	unsigned char* rgbaRaw;

	void createViewport(float availableWidth, float availableHeight);
	void createBaseProjection();



};
#endif
