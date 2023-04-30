#ifndef DISTANCE_MATRIX_HPP
#define DISTANCE_MATRIX_HPP

#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>

#include "datastructures.hpp"

#define BASE_URL "https://dev.virtualearth.net/REST/v1/Routes/DistanceMatrix"
#define TRAVEL_MODE "&travelMode=driving"
#define API_KEY "Akm_NVgib10xxw5_Kj5q6unyAY-drITfWrBMHL76QjnkV3gvqCJRbaezqHaqrd-F"
#define PI 3.14159265359

enum DistanceType {
    MANHATTAN = 0,
    EUCLIDEAN = 1,
    HAVERSINE = 2,
    REAL = 3
};

inline double euclidean_dist(const Coordinate& p1, const Coordinate& p2) {
    return sqrt((p1.latitude - p2.latitude) * (p1.latitude - p2.latitude) +
                (p1.longitude - p2.longitude) * (p1.longitude - p2.longitude));
}

inline double manhattan_dist(const Coordinate& p1, const Coordinate& p2) {
    return abs(p1.latitude - p2.longitude) +
           abs(p1.latitude - p2.longitude);
}
inline double haversine_dist(const Coordinate& c1, const Coordinate& c2) {
    double lat_1_deg = c1.latitude;
            double lon_1_deg = c1.longitude;
            double lat_2_deg = c2.latitude;
            double lon_2_deg = c2.longitude;
            // double PI = 3.14159265359;
            double lat_1_rad, lon_1_rad, lat_2_rad, lon_2_rad;
            lat_1_rad = lat_1_deg * (PI / 180);
            lon_1_rad = lon_1_deg * (PI / 180);
            lat_2_rad = lat_2_deg * (PI / 180);
            lon_2_rad = lon_2_deg * (PI / 180);
            double delta_lat, delta_lon;
            delta_lat = lat_1_rad - lat_2_rad;
            delta_lon = lon_1_rad - lon_2_rad;

            // Calculate sin^2 (delta / 2) for both lat and long
            double sdlat = pow(sin(delta_lat / 2), 2);
            double sdlon = pow(sin(delta_lon / 2), 2);

            // Radius of the Earth (approximate)
            const double radius_earth_miles = 3963;
            const double radius_earth_km = 6378;

            // http://en.wikipedia/org/wiki/Haversine_formula
            double t = sqrt(sdlat + (cos(lat_1_rad) * cos(lat_2_rad) * sdlon));
            double distance_miles = 2 * radius_earth_miles * asin(t);
            double distance_km = 2 * radius_earth_km * asin(t);
            return distance_km;
}

std::vector<std::vector<double>> compute_matrix(const std::vector<Coordinate> &locations,
                                                DistanceType disType, bool getTime);
inline double getDist(struct Coordinate c1,struct Coordinate c2,DistanceType method){
            if(method == EUCLIDEAN)
                return euclidean_dist(c1,c2);
            else if(method == HAVERSINE) 
                return haversine_dist(c1,c2);
            else if(method == MANHATTAN)
                return manhattan_dist(c1,c2);
}
// static string getCURLResponse(string curl_url);
// void parseResponse(string response, vector<vector<double>> &matrix,bool getTime);
// static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

#endif