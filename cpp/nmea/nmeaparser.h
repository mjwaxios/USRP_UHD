#ifndef NMEA_H
#define NMEA_H

#include <iostream>
#include <vector>
#include <string>

#define KNOTS_TO_MPS 0.514444444

class NMEA_PARSER
{
public:
    NMEA_PARSER();
    NMEA_PARSER(const std::string GGASentence,const std::string RMCSentence);
    NMEA_PARSER(const std::string GGAorRMC);
    int     UTC;
    double  latitude;
    double  longitude;
    double  altitude;
    double  speed;
    double  heading;
    int     numberSatellites;
    int     date;

private:
    // Set values of each sentence type

    // GGA sentences
    bool isValidGGA(const std::string GGASentence);
    void setValuesGGA(const std::string GGASentence);

    // RMC sentence
    bool isValidRMC(const std::string RMCSentence);
    void setValuesRMC(const std::string RMCSentence);

    // Auxualiary functions
    std::vector<std::string> splitStringByComma(const std::string);
    double stringToDouble(const std::string);
    double getCoordinates(std::string);
    std::string pad_zero(std::string, int);

};

double degreesToDecimal(const int Degrees,const double Minutes,const int seconds = 0);

#endif // NMEA_H
