#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "ar-jni.h"
#include "util/Logger.h"
#include "arLib/gles2/Renderer.h"
#include "arLib/ArLib.h"
#include "arLib/EnvironmentData.h"




using namespace cv;
using namespace std;

ArLib *arLib;

int imageDataSize = 0;
int rImageWidth = 0;
int rImageHeight = 0;

extern "C" {

JNIEXPORT void JNICALL Java_ch_bfh_bachelor_ar_ArLib_initArLib
  (JNIEnv *env, jclass obj, jint screenWidth, jint screenHeight, jint imageWidth, jint imageHeight)
	{
		//test ArLib
		Renderer *r = new Renderer();

		arLib = new ArLib(r,
				(int) screenWidth,
				(int) screenHeight,
				(int) imageWidth,
				(int) imageHeight,
				30.0f
		);

		imageDataSize = imageWidth * imageHeight * 4;

		rImageWidth = (int)imageWidth;
		rImageHeight = (int)imageHeight;

		//test adding models
		Model *myModel = new Cube;
	    glm::mat4 View = glm::mat4(1.0);
	    View = glm::translate(View, glm::vec3(0.0, 0.0, -25.0));
	    myModel->modelView = new glm::mat4(View);
		r->addModel(myModel);

		//test adding models
		Model *myModel2 = new Cube;
	    glm::mat4 View2 = glm::mat4(1.0);
	    View2 = glm::translate(View2, glm::vec3(0.0, 0.0, -25.0));
	    myModel2->modelView = new glm::mat4(View2);
		r->addModel(myModel2);
	}

JNIEXPORT void JNICALL Java_ch_bfh_bachelor_ar_ArLib_precessImage
  (JNIEnv *env, jclass obj, jbyteArray yuvImageArray, jfloat azimuth, jfloat pitch, jfloat roll)
	{
		jbyte* yuvRaw  = env->GetByteArrayElements(yuvImageArray, 0);
		arLib->processImage((unsigned char *)yuvRaw);
	    env->ReleaseByteArrayElements(yuvImageArray, yuvRaw, 0);
	}
}
