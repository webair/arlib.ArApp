#ifndef AR_DEFINITIONS
#define AR_DEFINITIONS

/*
 * Definition of structures to group the data
 */

#include <GLES2/gl2.h>

struct Point {
	float x;
	float y;
};

struct Dimension {
	float width;
	float height;
};

struct Orientation {
	float azimuth;
	float pitch;
	float roll;
};

struct Rect {
	Dimension dimension;
	Point origin;
};

struct Location {
	float longitude;
	float latitude;
};

struct TextureData {
	GLubyte* byteData;
	unsigned int length;
	unsigned int width;
	unsigned int height;
};

struct Material {
	int startIndex;
	int length;
	GLuint textureReference;
};
#endif
