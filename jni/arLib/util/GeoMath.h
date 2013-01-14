#include "arLib/Definitions.h"

/*
 * Helper class to calculate with geo referenced data
 *
 */
class GeoMath {

public:

	/*
	 * calculates the angle beetween two lat/lng points from the north direction
	 */
	float static getAngle(Location fromPoint, Location toPoint);

	/*
	 * calculates the distance beetween two lat/lng points
	 */
	float static getDistance(Location fromPoint, Location toPoint);
};
