#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "ar-jni.h"
#include "util/Logger.h"
#include "arLib/gles2/Renderer.h"
#include "arLib/ArLib.h"
#include "arLib/EnvironmentData.h"
#include "arLib/Definitions.h"




using namespace cv;
using namespace std;
using namespace glm;

ArLib *arLib;

int imageDataSize = 0;
int rImageWidth = 0;
int rImageHeight = 0;

extern "C" {

JNIEXPORT void JNICALL Java_ch_bfh_bachelor_ar_ArLib_initArLib
  (JNIEnv *env, jclass obj, jint screenWidth, jint screenHeight, jint imageWidth, jint imageHeight, jfloat cameraAngle)
	{
		//test ArLib
		Renderer *r = new Renderer();

		arLib = new ArLib(r,
				(int) screenWidth,
				(int) screenHeight,
				(int) imageWidth,
				(int) imageHeight,
				(float) cameraAngle
		);

		imageDataSize = imageWidth * imageHeight * 4;

		rImageWidth = (int)imageWidth;
		rImageHeight = (int)imageHeight;

		//test adding models
		Model *myModel = new Cube;
	    glm::mat4 View = mat4(1.0f);

	    //north
	    View = glm::translate(View, glm::vec3(0.0f, 0.0f, 10.0f));

	    myModel->setModelMatrix(View);
		r->addModel(myModel);

		myModel = new Cube;
	    View = mat4(1.0f);

	    //east
	    View = rotate(View, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	    View = glm::translate(View, glm::vec3(0.0f, 0.0f, 10.0f));

	    myModel->setModelMatrix(View);
		r->addModel(myModel);

		myModel = new Cube;
	    View = mat4(1.0f);

	    //south
	    View = glm::translate(View, glm::vec3(0.0f, 0.0f, -10.0f));

	    myModel->setModelMatrix(View);
		r->addModel(myModel);

		myModel = new Cube;
	    View = mat4(1.0f);

	    //west
	    View = rotate(View, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	    View = glm::translate(View, glm::vec3(0.0f, 0.0f, 10.0f));

	    myModel->setModelMatrix(View);
		r->addModel(myModel);

		myModel = new Cube;
	    View = mat4(1.0f);

	    //down
	    View = glm::translate(View, glm::vec3(0.0f, -10.0f, 0.0f));

	    myModel->setModelMatrix(View);
		r->addModel(myModel);

		myModel = new Cube;
	    View = mat4(1.0f);

	    //up
	    View = glm::translate(View, glm::vec3(0.0f, 10.0f, 0.0f));

	    myModel->setModelMatrix(View);
		r->addModel(myModel);

		//mat4 tranlation = translate(mat4(),vec3(2.0f,3.0f,4.0f));
		//LOGI("rot: %f, %f, %f, %f", tranlation[3][1], tranlation[3][2], tranlation[2][0], tranlation[3][0]);
		/*
		float testMatrix[16] = {
				1.1f, 2.1f, 3.1f, 4.1f,
				1.2f, 2.2f, 3.2f, 4.2f,
				1.3f, 2.3f, 3.3f, 4.3f,
				1.4f, 2.4f, 3.4f, 4.4f,
		};
		mat4 testMat = transpose(make_mat4(testMatrix));
		*/
	}

JNIEXPORT void JNICALL Java_ch_bfh_bachelor_ar_ArLib_precessImage
  (JNIEnv *env, jclass obj, jbyteArray yuvImageArray, jfloat azimuth,
		  jfloat pitch, jfloat roll, jfloatArray rotationMatrixArray)
	{
		jbyte* yuvRaw  = env->GetByteArrayElements(yuvImageArray, 0);

		Orientation orientation;
		orientation.azimuth = (float) azimuth;
		//orientation.roll = 0.0f;
		orientation.roll = (float) roll;
		orientation.pitch = (float) pitch;
		/*
		orientation.azimuth = 0.0f;
		orientation.roll = 0.0f;
		orientation.pitch = 0.0f;
		*/
		//LOGI("azimuth: %f, pitch: %f, roll: %f", orientation.azimuth, orientation.pitch, orientation.roll);

		float *rotationMatrixRaw = env->GetFloatArrayElements(rotationMatrixArray, 0);
		arLib->envData->setRotationMatrix(rotationMatrixRaw);
		arLib->setDeviceOrientation(orientation);
		arLib->processImage((unsigned char *)yuvRaw);
	    env->ReleaseByteArrayElements(yuvImageArray, yuvRaw, 0);
	    env->ReleaseFloatArrayElements(rotationMatrixArray, rotationMatrixRaw, 0);
	}
}
