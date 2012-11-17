#ifndef AR_ENVIROMENT_DATA
#define AR_ENVIROMENT_DATA

#include <opencv2/core/core.hpp>
#include <GLES2/gl2.h>
#include <vector>
#include "arLib/model/Model.h"
#include "arLib/Definitions.h"


using namespace std;
using namespace glm;
using namespace cv;

class EnvironmentData {

public:
	EnvironmentData();
	~EnvironmentData();

	//gles2 helpers
	GLuint cameraTextrueRef;

	void setVerticalAngle(float angle);
	float getVerticalAngle();

	void setImageDimension(Dimension size);
	Dimension getImageDimension();
	float getImageRatio();
	int getImageRGBASize();

	//device states
	void setDeviceOrientation(Orientation orientation);
	void setRotationMatrix(float *rotationMatrix);
	Orientation getDeviceOrientation();
	void setDeviceLocation(Location location);
	Location getDeviceLocation();

	void addModel(Model* m);
	vector<Model*>* getModels();

	mat4 getProjectionMatrix();
	mat4 getViewMatrix();



	float frustumDistanceRatio;
private:
	float frustumNear;
	float frustumFar;
	float frustumAngle;

	mat4* baseProjection;
	mat4* rotationMatrix;
	//image stuff
	Dimension imageDimension;
	float imageRatio;
	int imageRGBASize;

	Orientation deviceOrientation;
	Location deviceLocation;
	void createBaseProjection();

	//models
	vector<Model*>* models;

};
#endif
