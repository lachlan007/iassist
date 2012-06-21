//---------------------------------------------------------------------------
//  iAssist - Rapid deployment of Maxim iButton(c) miniature logger devices
//  Copyright (C) 2012 ETH Zurich, Computer Engineering and Networks
//      Laboratory, Matthias Keller <kellmatt at tik.ee.ethz.ch>
//  Copyright (C) 2009 Guido Hungerbuehler
//  Copyright (C) 2009 Oliver Knecht
//  Copyright (C) 2009 Suhel Sheikh
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------
#include "Area.h"

Area::Area() {
	AreaID="";
	Altitude=-9999;
	Longitude=-9999;
	Latitude=-9999;
	HAE_DEM=-9999;
	East_DEM=-9999;
	North_DEM=-9999;
	Slope_DEM=-9999;
	Aspect_DEM=-9999;
	SolDy_DEM=-9999;
	SolYr_DEM=-9999;
	StartLongitude=-9999;
	StartLatitude=-9999;
	EndLongitude1=-9999;
	EndLatitude1=-9999;
	EndLongitude2=-9999;
	EndLatitude2=-9999;
}

Area::~Area() {

}

void Area::clearArea() {
	AreaID="";
	Altitude=-9999;
	Longitude=-9999;
	Latitude=-9999;
	HAE_DEM=-9999;
	East_DEM=-9999;
	North_DEM=-9999;
	Slope_DEM=-9999;
	Aspect_DEM=-9999;
	SolDy_DEM=-9999;
	SolYr_DEM=-9999;
	StartLongitude=-9999;
	StartLatitude=-9999;
	EndLongitude1=-9999;
	EndLatitude1=-9999;
	EndLongitude2=-9999;
	EndLatitude2=-9999;
}
