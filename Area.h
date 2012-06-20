#ifndef AREA_H_
#define AREA_H_

#include <QString>

/**
 * Area holds all the Data which a Footprint/Area contains
 */
class Area {
public:
	/**
	 * Default constructor
	 */
	Area();
	/**
	 * Desconstructor
	 */
	virtual ~Area();

	/**
	 * Clears all the data
	 */
	void clearArea();

	//! AreaID of the the area
	QString AreaID;
	//! Altitude of the Area
	double Altitude;
	//! Longitude of the Area
	double Longitude;
	//! Latitude of the Area
	double Latitude;
	//! HAE_DEM, Altitude of model
	double HAE_DEM;
	//! Easting_DEM
	double East_DEM;
	//! Northing_DEM
	double North_DEM;
	//! Slope_DEM
	double Slope_DEM;
	//! Aspect_DEM
	double Aspect_DEM;
	//! SolDy_DEM
	double SolDy_DEM;
	//! SolDy_DEM
	double SolYr_DEM;
	//! StartLongitude
	double StartLongitude;
	//! StartLatitude
	double StartLatitude;
	//! EndLongitude 1
	double EndLongitude1;
	//! EndLatitude 1
	double EndLatitude1;
	//! EndLongitude 2
	double EndLongitude2;
	//! EndLatitude 2
	double EndLatitude2;
};

#endif /* AREA_H_ */
