#include "ar-jni.h"
#include "util/Logger.h"
#include "gles2/Renderer.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace cv;
using namespace std;

Renderer *r;

int imageDataSize = 0;

extern "C" {
/*
 * from wikipedia:
size.total = size.width * size.height;
y = yuv[position.y * size.width + position.x];
u = yuv[(position.y / 2) * (size.width / 2) + (position.x / 2) + size.total];
v = yuv[(position.y / 2) * (size.width / 2) + (position.x / 2) + size.total + (size.total / 4)];
 */
int convertYUVtoRGB(int y, int u, int v) {
    int r,g,b;

    r = y + (int)1.402f*v;
    g = y - (int)(0.344f*u +0.714f*v);
    b = y + (int)1.772f*u;
    r = r>255? 255 : r<0 ? 0 : r;
    g = g>255? 255 : g<0 ? 0 : g;
    b = b>255? 255 : b<0 ? 0 : b;
    return 0xff000000 | (b<<16) | (g<<8) | r;
}

void decodeYuvImage (unsigned char* src, int* dst, int width, int height) {
    int size = width*height;
    int offset = size;
    int u, v, y1, y2, y3, y4;

    for(int i=0, k=0; i < size; i+=2, k+=2) {
    	int r,g,b;

        y1 = src[i  ]&0xff;
        y2 = src[i+1]&0xff;
        y3 = src[width+i  ]&0xff;
        y4 = src[width+i+1]&0xff;

        u = src[offset+k  ]&0xff;
        v = src[offset+k+1]&0xff;
        u = u-128;
        v = v-128;

        dst[i  ] = convertYUVtoRGB(y1, u, v);
        dst[i+1] = convertYUVtoRGB(y2, u, v);
        dst[width+i  ] = convertYUVtoRGB(y3, u, v);
        dst[width+i+1] = convertYUVtoRGB(y4, u, v);

        if (i!=0 && (i+2)%width==0)
            i+=width;
    }

}


JNIEXPORT void JNICALL Java_ch_bfh_bachelor_ar_ArLib_initArLib
  (JNIEnv *env, jclass obj, jint screenWidth, jint screenHeight, jint imageWidth, jint imageHeight)
	{
		LOGI("init screen: %dx%d", screenWidth, screenHeight);
		LOGI("init texture for image: %dx%d", imageWidth, imageHeight);
		r = new Renderer();
		r->setImageSize((int)imageWidth, (int)imageHeight);
		r->setScreenSize((int)screenWidth, (int)screenHeight);

		imageDataSize = imageWidth * imageHeight * 4;
	}

JNIEXPORT void JNICALL Java_ch_bfh_bachelor_ar_ArLib_precessImage
  (JNIEnv *env, jclass obj, jbyteArray yuvImageArray)
	{
		jbyte* yuvRaw  = env->GetByteArrayElements(yuvImageArray, 0);
		unsigned char* rgbaRaw = (unsigned char *)malloc(imageDataSize * sizeof(unsigned char));
		decodeYuvImage((unsigned char *)yuvRaw, (int *)rgbaRaw, r->imageWidth, r->imageHeight);
		//unsigned char* rgbaRaw = (unsigned char *) env->GetByteArrayElements(yuvImageArray, 0);

		//Mat yuv(r->imageHeight, r->imageWidth, CV_8UC1, (unsigned char *)yuvRaw);
		Mat rgba(r->imageHeight, r->imageWidth, CV_8UC4, (unsigned char *)rgbaRaw);

		//cvtColor(yuv, rgba, CV_YUV420sp2BGR, 4);

		vector<KeyPoint> v;

	    FastFeatureDetector detector(50);
	    detector.detect(rgba, v);
	    for( size_t i = 0; i < v.size(); i++ )
	        circle(rgba, Point(v[i].pt.x, v[i].pt.y), 10, Scalar(255,0,0,255));

		r->loadTexture((unsigned char *)rgbaRaw);
		r->renderFrame();
		free(rgbaRaw);
	}
}
