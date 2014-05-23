/*
 *  Adafruit_Trellis_XY.cpp
 *  
 *  Copyright (C) 2014  George Kuetemeyer (gk@pobox.com)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  ---
 *
 *  Version 1.2
 */

#include <Adafruit_Trellis_XY.h>

// this array maps trellis IDs for a single Tile to X/Y coordinates
static byte TrellisToXY[16][2] = {
	{ 0, 3 },
	{ 1, 3 },
	{ 2, 3 },
	{ 3, 3 },
	{ 0, 2 },
	{ 1, 2 },
	{ 2, 2 },
	{ 3, 2 },
	{ 0, 1 },
	{ 1, 1 },
	{ 2, 1 },
	{ 3, 1 },
	{ 0, 0 },
	{ 1, 0 },
	{ 2, 0 },
	{ 3, 0 }
};

static byte XyToTrellis[4][4] = {
	{ 12,  8, 4, 0 },
	{ 13,  9, 5, 1 },
	{ 14, 10, 6, 2 },
	{ 15, 11, 7, 3 },
};

//---------------------------------------------------------------
Adafruit_Trellis_XY::Adafruit_Trellis_XY(void) {

}

//---------------------------------------------------------------
Adafruit_Trellis_XY::~Adafruit_Trellis_XY(void) {
 	
}

//---------------------------------------------------------------
// initialize object with total number of Trellis keys
void Adafruit_Trellis_XY::begin(byte i_numKeys) {
	// initialize number of keys	
	numKeys = i_numKeys;
}

//---------------------------------------------------------------
// set tile offsets
void Adafruit_Trellis_XY::setOffsets(byte tile_number, byte x_offset, byte y_offset) {
	offsets[tile_number][0] = x_offset;
	offsets[tile_number][1] = y_offset;	
}

//---------------------------------------------------------------
// get Trellis X coordinate
byte Adafruit_Trellis_XY::getTrellisX(byte trellisID) {
	return getCoordinate(trellisID, 0);
}

//---------------------------------------------------------------
// get Trellis Y coordinate
byte Adafruit_Trellis_XY::getTrellisY(byte trellisID) {
	return getCoordinate(trellisID, 1);
}

//---------------------------------------------------------------
// Get Trellis button/LED ID from X/Y values
byte Adafruit_Trellis_XY::getTrellisId(byte xValue, byte yValue) {
	byte xOffset;
	byte yOffset;
	byte tileNumber = 0;

	// get offset values
	xOffset = getOffset(xValue);
	yOffset = getOffset(yValue);

	// get tile number
	for(byte t = 0; t < 8; t++) {
		if(offsets[t][0] == xOffset && offsets[t][1] == yOffset) {
			tileNumber = t;
			// get values for last tile
			xValue = xValue - offsets[t][0];
			yValue = yValue - offsets[t][1];
			break;
		}
	}
	return 16 * tileNumber + XyToTrellis[xValue][yValue];
}

//---------------------------------------------------------------
byte Adafruit_Trellis_XY::getOffset(byte coordinate) {
	if(coordinate == 0) {
		return 0;
	}
	else if(coordinate > 3  && coordinate < 8) {
		return 4;
	}
	else if(coordinate > 7  && coordinate < 12) {
		return 8;
	}
	else if(coordinate > 11 && coordinate < 16) {
		return 12;
	}
	else if(coordinate > 15 && coordinate < 20) {
		return 16;
	}
	else if(coordinate > 7  && coordinate < 24) {
		return 20;
	}
	else if(coordinate > 11 && coordinate < 28) {
		return 24;
	}
	else if(coordinate > 15 && coordinate < 32) {
		return 28;
	}
	else {
		return 0;
	}
}

//---------------------------------------------------------------
// map Trellis button/led ids to X or Y coordinate 
byte Adafruit_Trellis_XY::getCoordinate(byte trellisID, byte coordinateType) {
	// get trellis matrix offsets
	if(trellisID < 16) {
		offset    = offsets[0][coordinateType];
	}
	else if(trellisID > 15 && trellisID < 32) {
		trellisID = trellisID - 16;
		offset    = offsets[1][coordinateType];
	}
	else if(trellisID > 31 && trellisID < 48) {
		trellisID = trellisID - 32;
		offset    = offsets[2][coordinateType];
	}  
	else if(trellisID > 47 && trellisID < 64) {
		trellisID = trellisID - 48;
		offset    = offsets[3][coordinateType];
	}
	else if(trellisID > 63 && trellisID < 80) {
		trellisID = trellisID - 64;
		offset    = offsets[4][coordinateType];
	}
	else if(trellisID > 79 && trellisID < 96) {
		trellisID = trellisID - 80;
		offset    = offsets[5][coordinateType];
	}  
	else if(trellisID > 95 && trellisID < 112) {
		trellisID = trellisID - 96;
		offset    = offsets[6][coordinateType];
	}
	else if(trellisID > 111 && trellisID < 128) {
		trellisID = trellisID - 112;
		offset    = offsets[7][coordinateType];
	}	
	else {
		offset    = offsets[0][coordinateType];
	}
	// add in offsets and return X or Y value
	return TrellisToXY[trellisID][coordinateType] + offset;
}
	
	