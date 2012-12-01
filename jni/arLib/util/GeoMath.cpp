#include "GeoMath.h"
#include "util/Logger.h"
#include <cmath>

using namespace std;



float GeoMath::getAngle(Location fromPoint, Location toPoint) {
	float toRad = (float)M_PI / 180.0f;
	float toDeg = 180.0f / (float)M_PI;

	float lat1 = fromPoint.latitude * toRad;
	float lon1 = fromPoint.longitude * toRad;
	float lat2 = toPoint.latitude * toRad;
	float lon2 = toPoint.longitude * toRad;

	float dLat = lat2-lat1;
	float dLon = lon2-lon1;

	float y = sin(dLon) * cos(lat2);
	float x = cos(lat1) * sin(lat2) -
	        sin(lat1)*cos(lat2)*cos(dLon);
	float brng = atan2(y, x);
	return brng * toDeg;
}

float GeoMath::getDistance(Location fromPoint, Location toPoint) {
	float toRad = (float)M_PI / 180.0f;

	float lat1 = fromPoint.latitude * toRad;
	float lon1 = fromPoint.longitude * toRad;
	float lat2 = toPoint.latitude * toRad;
	float lon2 = toPoint.longitude * toRad;

	float R = 6371000.0f; // m
	float dLat = lat2-lat1;
	float dLon = lon2-lon1;
	float a = sin(dLat/2.0f) * sin(dLat/2.0f) +
	        cos(lat1) * cos(lat2) *
	        sin(dLon/2.0f) * sin(dLon/2.0f);
	float c = 2.0f * atan2(sqrt(a), sqrt(1.0f-a));
	float d = R * c;

	return d;

}
