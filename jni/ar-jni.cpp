#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "ar-jni.h"
#include "util/Logger.h"
#include "arLib/gles2/Renderer.h"
#include "arLib/ArLib.h"



using namespace cv;
using namespace std;

Renderer *r;

ArLib *arLib;

int imageDataSize = 0;
int rImageWidth = 0;
int rImageHeight = 0;

extern "C" {

JNIEXPORT void JNICALL Java_ch_bfh_bachelor_ar_ArLib_initArLib
  (JNIEnv *env, jclass obj, jint screenWidth, jint screenHeight, jint imageWidth, jint imageHeight)
	{
		//test ArLib
		r = new Renderer();

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

		r->setImageSize((int)imageWidth, (int)imageHeight);


	}

JNIEXPORT void JNICALL Java_ch_bfh_bachelor_ar_ArLib_precessImage
  (JNIEnv *env, jclass obj, jbyteArray yuvImageArray)
	{

		int grayImageSize = imageDataSize / 4;

		jbyte* yuvRaw  = env->GetByteArrayElements(yuvImageArray, 0);
		Mat yuv(rImageHeight + rImageHeight/2, rImageWidth, CV_8UC1, (unsigned char *)yuvRaw);

		// will be used for further image processing
		//unsigned char* grayRaw = (unsigned char *)malloc(grayImageSize * sizeof(unsigned char));
		//Mat gray(rImageHeight, rImageWidth, IPL_DEPTH_8U, grayRaw);

		unsigned char* rgbaRaw = (unsigned char *)malloc(imageDataSize * sizeof(unsigned char));
		Mat rgba(rImageHeight, rImageWidth, CV_8UC4, (unsigned char *)rgbaRaw);

		cvtColor(yuv, rgba, CV_YUV420sp2RGBA);
		//cv::Canny(gray, gray, 50.0f, 150.0f);
		//cvtColor(gray, rgba, CV_GRAY2RGBA);

		//vector<KeyPoint> v;
	    //FastFeatureDetector detector(50);
	    //detector.detect(yuv, v);
	    //for( size_t i = 0; i < v.size(); i++ )
	        //circle(rgba, Point(v[i].pt.x, v[i].pt.y), 10, Scalar(255,0,0,255));


		r->loadTexture((unsigned char *)rgbaRaw);
		r->renderFrame();

		free(rgbaRaw);
		//free(grayRaw);
	    env->ReleaseByteArrayElements(yuvImageArray, yuvRaw, 0);

	}
}
