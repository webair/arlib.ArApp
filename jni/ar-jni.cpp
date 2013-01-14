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
		LOGI("initializing ArLib");
		//test ArLib
		Renderer *r = new Renderer();

		arLib = new ArLib(r,
				(int) screenWidth,
				(int) screenHeight,
				(int) imageWidth,
				(int) imageHeight,
				(float) cameraAngle
		);
	}

/*
	public static native void addModel(float[] vnt, short[] faces,
		float[] pointOfGravity, float[] boundingBox, float northAngle,
		float longitude, float latitude);
 */
JNIEXPORT void JNICALL Java_ch_bfh_bachelor_ar_ArLib_addModel
  (JNIEnv *env, jclass obj, jint modelId,
		  jfloatArray vntArray, jshortArray facesArray,
		  jfloatArray cogArray , jfloatArray bbArray, jfloat northAngle,
		  jfloat latitude, jfloat longitude, jobjectArray textureArrayArray, jintArray materialArray, jintArray imageDimensionArray) {
			//test adding models
			Location modelLocation;
			modelLocation.latitude = latitude;
			modelLocation.longitude = longitude;

			jfloat* vntRaw  = env->GetFloatArrayElements(vntArray, 0);
			jshort* facesRaw  = env->GetShortArrayElements(facesArray, 0);
			jfloat* cogRaw  = env->GetFloatArrayElements(cogArray, 0);
			jfloat* bbRaw  = env->GetFloatArrayElements(bbArray, 0);
			jint* dimensionRaw = env->GetIntArrayElements(imageDimensionArray, 0);

			TextureData* textureDataArray = new TextureData[env->GetArrayLength(textureArrayArray)];
			jint* materialRaw = env->GetIntArrayElements(materialArray,0);

			for(int i=0;i<env->GetArrayLength(textureArrayArray);i++)
			{
				TextureData texD;
				jbyteArray textureArray = (jbyteArray) env->GetObjectArrayElement(textureArrayArray,i);
				jbyte* textureRaw = env->GetByteArrayElements(textureArray, 0);

				texD.length = env->GetArrayLength(textureArray);
				if (texD.length==0) {
					texD.length = 4;
					texD.byteData = new GLubyte[4];
					texD.byteData[0],texD.byteData[3] = 255;
					texD.byteData[1], texD.byteData[2] = 0;
					texD.width = 1;
					texD.height = 1;
				} else {
					//copy image
					GLubyte *imgData = new GLubyte[texD.length];
					for (unsigned int j=0; j < texD.length; j++) {
						if (j % 4 == 0) {
							unsigned int targetPos = texD.length-j-4;
							imgData[targetPos] = textureRaw[j];
							imgData[targetPos+1] = textureRaw[j+1];
							imgData[targetPos+2] = textureRaw[j+2];
							imgData[targetPos+3] = textureRaw[j+3];

						}

					}
					texD.byteData = imgData;
					texD.width= dimensionRaw[i*2];
					texD.height = dimensionRaw[(i*2)+1];
					env->ReleaseByteArrayElements(textureArray, textureRaw,0);
				}
				textureDataArray[i] = texD;
			}

			Model *myModel = new Model(2, (GLfloat *)vntRaw, env->GetArrayLength(vntArray),
					(GLushort *)facesRaw, env->GetArrayLength(facesArray),
					(GLfloat *)cogRaw, (GLfloat *)bbRaw,
					(float) northAngle, modelLocation);

		    arLib->addModel(myModel, materialRaw, env->GetArrayLength(textureArrayArray), textureDataArray);

			env->ReleaseFloatArrayElements(vntArray, vntRaw, 0);
			env->ReleaseShortArrayElements(facesArray, facesRaw, 0);
		    env->ReleaseFloatArrayElements(cogArray, cogRaw, 0);
		    env->ReleaseFloatArrayElements(bbArray, bbRaw, 0);

			env->ReleaseIntArrayElements(materialArray, materialRaw, 0);

}

JNIEXPORT void JNICALL Java_ch_bfh_bachelor_ar_ArLib_precessImage
  (JNIEnv *env, jclass obj, jbyteArray yuvImageArray, jfloat azimuth,
		  jfloat pitch, jfloat roll, jfloat latitude, jfloat longitude)
	{
		jbyte* yuvRaw  = env->GetByteArrayElements(yuvImageArray, 0);
		Orientation orientation;
		orientation.azimuth = (float) azimuth;
		orientation.roll = (float) roll;
		orientation.pitch = (float) pitch;

		arLib->setDeviceOrientation(orientation);

	    Location location;
	    location.latitude = (float) latitude;
	    location.longitude = (float) longitude;

		arLib->setDeviceLocation(location);
		arLib->processImage((unsigned char *)yuvRaw);

		env->ReleaseByteArrayElements(yuvImageArray, yuvRaw, 0);
	}
}
