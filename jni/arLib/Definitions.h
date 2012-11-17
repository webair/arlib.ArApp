#ifndef AR_DEFINITIONS
#define AR_DEFINITIONS

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
#endif
