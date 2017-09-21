#include "nmeaparser.h"

#include <assert.h>
#include <sstream>
#include <stdlib.h>
#include <iomanip>

NMEA_PARSER::NMEA_PARSER()
{
    UTC                 = 0;
    latitude            = 0;
    longitude           = 0;
    altitude            = 0;
    numberSatellites    = 0;
    speed               = 0;
    heading             = 0;
    date                = 0;
}

// Constructor using GGA and RMC sentences only. Assigns all data.
NMEA_PARSER::NMEA_PARSER(std::string GGASentence,std::string RMCSentence){
    if (isValidGGA(GGASentence)) setValuesGGA(GGASentence);
    if (isValidRMC(RMCSentence)) setValuesRMC(RMCSentence);
}

NMEA_PARSER::NMEA_PARSER(std::string GGAorRMC){
    if(isValidGGA(GGAorRMC))
    	setValuesGGA(GGAorRMC);
    else if(isValidRMC(GGAorRMC))
    	setValuesRMC(GGAorRMC);
}

// Set values from GGA sentence

// Checks if GGA sentence is valid.
bool NMEA_PARSER::isValidGGA(const std::string GGASentence){
    bool returnBool = true;
    std::vector<std::string> elementVector = splitStringByComma(GGASentence);

    if (elementVector[0] != "$GPGGA")               returnBool = false;
    if (elementVector.size() != 15)                 returnBool = false;
    if (atoi(elementVector[6].c_str()) == 0)        returnBool = false;
    if (atoi(elementVector[7].c_str()) == 0)        returnBool = false;

    return returnBool;
}

// Input: GGA NMEA sentence
// Output: set values in class.
void NMEA_PARSER::setValuesGGA(std::string GGA){
	/*
	.                                                     11
	        1         2      3 4        5 6 7  8   9   10 |  12 13  14   15
	        |         |      | |        | | |  |   |   |  |  |  |   |    |
	$--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
	 1) Time (UTC)
	 2) Latitude
	 3) N or S (North or South)
	 4) Longitude
	 5) E or W (East or West)
	 6) GPS Quality Indicator,
	 0 - fix not available,
	 1 - GPS fix,
	 2 - Differential GPS fix
	 7) Number of satellites in view, 00 - 12
	 8) Horizontal Dilution of precision
	 9) Antenna Altitude above/below mean-sea-level (geoid)
	10) Units of antenna altitude, meters
	11) Geoidal separation, the difference between the WGS-84 earth ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid
	12) Units of geoidal separation, meters
	13) Age of differential GPS data, time in seconds since last SC104 type 1 or 9 update, null field when DGPS is not used
	14) Differential reference station ID, 0000-1023
	15) Checksum
	*/
	std::vector<std::string> elementVector = splitStringByComma(GGA);
    // Assert we have a GGA sentence
    assert(elementVector[0] == "$GPGGA");

    this->UTC                 = atoi(elementVector[1].c_str());
    this->latitude            = getCoordinates(elementVector[2]);
    if (elementVector[3] == "S") this->latitude  = -this->latitude;
    this->longitude           = getCoordinates(elementVector[4]);
    if (elementVector[5] == "W") this->longitude  = -this->longitude;
    this->altitude            = stringToDouble(elementVector[9]);
    this->numberSatellites    = atoi(elementVector[7].c_str());
}


// RMC sentence

// Check if RMC sentence is valid with NMEA standard.
bool NMEA_PARSER::isValidRMC(const std::string RMCSentence){
    bool returnBool = true;
    std::vector<std::string> elementVector = splitStringByComma(RMCSentence);

    if (elementVector[0] != "$GPRMC")               returnBool = false;
    if (elementVector.size() != 12)                 returnBool = false;
    if (elementVector[2] != "A")                    returnBool = false;

    return returnBool;
}

void NMEA_PARSER::setValuesRMC(const std::string RMCSentence){
    /*
    .      1         2 3       4  5       6 7   8   9   10  11 12
           |         | |       |  |       | |   |   |    |   | |
    $--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxx,x.x,a*hh
     1) Time (UTC)
     2) Status, V = Navigation receiver warning
     3) Latitude
     4) N or S
     5) Longitude
     6) E or W
     7) Speed over ground, knots
     8) Track made good, degrees true
     9) Date, ddmmyy
    10) Magnetic Variation, degrees
    11) E or W
    12) Checksum

    */

	std::vector<std::string> elementVector = splitStringByComma(RMCSentence);
    // Assert we have an RMC  sentence
    assert(elementVector[0] == "$GPRMC");

    this->UTC               = atoi(elementVector[1].c_str());
    this->latitude          = getCoordinates(elementVector[3]);
    if (elementVector[4] == "S") this->latitude  = -this->latitude;
    this->longitude         = getCoordinates(elementVector[5]);
    if (elementVector[6] == "W") this->longitude  = -this->longitude;
    this->speed             = stringToDouble(elementVector[7])* KNOTS_TO_MPS;
    this->heading           = stringToDouble(elementVector[8]);
    this->date              = atoi(elementVector[9].c_str());


}

/*-----Auxiliary functions-----*/

// Input: coma separated string
// Output: Vector with all the elements in input.
std::vector<std::string> NMEA_PARSER::splitStringByComma(std::string input){

	std::vector<std::string>  returnVector;
	std::stringstream    ss(input);
    std::string          element;

    while(std::getline(ss, element, ',')) {
        returnVector.push_back(element);
    }


    return returnVector;
}
double degreesToDecimal(int degrees, double minutes, int seconds )
{
    double returnDouble = 0;
    returnDouble = degrees + minutes/60 + seconds/3600.0f;
    return returnDouble;
}
double NMEA_PARSER::stringToDouble(std::string inputString){
    //If string empty, return 0.
    double returnValue = 0;
    std::istringstream istr(inputString);
    istr >> returnValue;
    return (returnValue);

}
std::string NMEA_PARSER::pad_zero(std::string input, int len){
    std::ostringstream oss;
    oss << std::setw(len) << std::setfill('0') << input;
    return oss.str();
}
double NMEA_PARSER::getCoordinates(std::string array){
    double decimalDegrees = 0;
    // Convert input array into two sub arrays containing the degrees and the minutes
    // Check for correct array length
    array = pad_zero(array, 10);
    std::string degree = array.substr(0, 3);
    std::string minute = array.substr(3, 10);
    int degrees = atoi(degree.c_str());
    double minutes = (double)atof(minute.c_str());
    decimalDegrees = degreesToDecimal(degrees, minutes);
    return decimalDegrees;

}

