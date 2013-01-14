#ifndef AR_ENVIROMENT_DATA
#define AR_ENVIROMENT_DATA

/*
 * EnviromentData class to hold all relevant data for the rendering process
 * as well as for the image processing
 *
 */

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

	/*
	 * getters and setters for the relevant Data
	 */

	void setCameraTextureReference(GLuint reference);
	GLuint getCameraTextureReference();

	void setVerticalAngle(float angle, float near, float far);
	float getVerticalAngle();

	void setImageDimension(Dimension size);
	Dimension getImageDimension();
	float getImageRatio();
	int getImageRGBASize();

	//device states
	void setDeviceOrientation(Orientation orientation);
	Orientation getDeviceOrientation();
	void setDeviceLocation(Location location);
	Location getDeviceLocation();

	void addModel(Model* m);
	vector<Model*>* getModels();

	mat4 getProjectionMatrix();
	mat4 getViewMatrix();

private:
	GLuint cameraTextrueRef;
	float frustumNear;
	float frustumFar;
	float frustumAngle;

	mat4* baseProjection;

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
