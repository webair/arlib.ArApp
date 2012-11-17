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

GLushort a_indices[]  = { 0, 1, 2,   2, 3, 0,      // front
                       4, 5, 6,   6, 7, 4,      // right
                       8, 9,10,  10,11, 8,      // top
                      12,13,14,  14,15,12,      // left
                      16,17,18,  18,19,16,      // bottom
                      20,21,22,  22,23,20 };    // back

GLfloat a_vertices[] = {
						1, 1, 1,   0, 0, 1,   1, 1, 1,              // v0 (front)
                       -1, 1, 1,   0, 0, 1,   1, 1, 0,              // v1
                       -1,-1, 1,   0, 0, 1,   1, 0, 0,              // v2
                        1,-1, 1,   0, 0, 1,   1, 0, 1,              // v3

                        1, 1, 1,   1, 0, 0,   1, 1, 1,              // v0 (right)
                        1,-1, 1,   1, 0, 0,   1, 0, 1,              // v3
                        1,-1,-1,   1, 0, 0,   0, 0, 1,              // v4
                        1, 1,-1,   1, 0, 0,   0, 1, 1,              // v5

                        1, 1, 1,   0, 1, 0,   1, 1, 1,              // v0 (top)
                        1, 1,-1,   0, 1, 0,   0, 1, 1,              // v5
                       -1, 1,-1,   0, 1, 0,   0, 1, 0,              // v6
                       -1, 1, 1,   0, 1, 0,   1, 1, 0,              // v1

                       -1, 1, 1,  -1, 0, 0,   1, 1, 0,              // v1 (left)
                       -1, 1,-1,  -1, 0, 0,   0, 1, 0,              // v6
                       -1,-1,-1,  -1, 0, 0,   0, 0, 0,              // v7
                       -1,-1, 1,  -1, 0, 0,   1, 0, 0,              // v2

                       -1,-1,-1,   0,-1, 0,   0, 0, 0,              // v7 (bottom)
                        1,-1,-1,   0,-1, 0,   0, 0, 1,              // v4
                        1,-1, 1,   0,-1, 0,   1, 0, 1,              // v3
                       -1,-1, 1,   0,-1, 0,   1, 0, 0,              // v2

                        1,-1,-1,   0, 0,-1,   0, 0, 1,              // v4 (back)
                       -1,-1,-1,   0, 0,-1,   0, 0, 0,              // v7
                       -1, 1,-1,   0, 0,-1,   0, 1, 0,              // v6
                        1, 1,-1,   0, 0,-1,   0, 1, 1 };            // v5

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

	    //east
		float centerOfGravity[] = {0.0f, 0.0f, 0.0f};
		float boundingBox[36];
		Model *myModel = new Model(a_vertices, 9*4*6,
				 a_indices, 6*6,
				 centerOfGravity, boundingBox,
				  45.0f);

	    mat4 View = glm::translate(mat4(), glm::vec3(0.0f, 0.0f, 10.0f));

	    myModel->setWorldMatrix(View);
	    arLib->addModel(myModel);
	}

/*
	public static native void addModel(float[] vnt, short[] faces,
		float[] pointOfGravity, float[] boundingBox, float northAngle,
		float longitude, float latitude);
 */
JNIEXPORT void JNICALL Java_ch_bfh_bachelor_ar_ArLib_addModel
  (JNIEnv *env, jclass obj,
		  jfloatArray vntArray, jshortArray facesArray,
		  jfloatArray cogArray , jfloatArray bbArray, jfloat northAngle,
		  jfloat latitude, jfloat longitude) {

	jfloat* vntRaw  = env->GetFloatArrayElements(vntArray, 0);
	jshort* facesRaw  = env->GetShortArrayElements(facesArray, 0);
	jfloat* cogRaw  = env->GetFloatArrayElements(cogArray, 0);
	jfloat* bbRaw  = env->GetFloatArrayElements(bbArray, 0);

	Model *myModel = new Model((GLfloat *)vntRaw, env->GetArrayLength(vntArray),
			(GLushort *)facesRaw, env->GetArrayLength(facesArray),
			(GLfloat *)cogRaw, (GLfloat *)bbRaw,
			(float) northAngle);

    mat4 View = glm::translate(mat4(), glm::vec3(0.0f, 0.0f, 10.0f));

    myModel->setWorldMatrix(View);
    arLib->addModel(myModel);

	env->ReleaseFloatArrayElements(vntArray, vntRaw, 0);
	env->ReleaseShortArrayElements(facesArray, facesRaw, 0);
    env->ReleaseFloatArrayElements(cogArray, cogRaw, 0);
    env->ReleaseFloatArrayElements(bbArray, bbRaw, 0);


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

		//ca. Rathausgasse 57
	    Location location;
	    location.latitude = 46.948592;
	    location.longitude = 7.449328;

		arLib->setDeviceLocation(location);
		arLib->processImage((unsigned char *)yuvRaw);


		env->ReleaseByteArrayElements(yuvImageArray, yuvRaw, 0);
	    env->ReleaseFloatArrayElements(rotationMatrixArray, rotationMatrixRaw, 0);
	}
}
