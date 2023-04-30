#ifndef __DATASTRUCTURES__HEADER__
#define __DATASTRUCTURES__HEADER__

#include <bits/stdc++.h>

#include <fstream>
#include <iostream>

using namespace std;
// #define int long long

struct Dimension_ {
    float length, width, height;
    Dimension_() {}
    Dimension_(float l, float w, float h) {
        length = l;
        width = w;
        height = h;
    }
};

struct Position {
    float x, y, z;
};

struct Coordinate {
    float longitude, latitude;
    static constexpr double scale = 1e4;
    Coordinate() {}
    Coordinate(float lat, float lng) {
        longitude = lng;
        latitude = lat;
    }
};

inline bool operator<(const Coordinate& a1, const Coordinate& a2) {
    if (a1.longitude != a2.longitude) return a1.longitude < a2.longitude;
    if (a1.latitude != a2.latitude) return a1.latitude < a2.latitude;
    return false;
}

struct PolarCoordinate {
    float radius, angle;
};

class item {
    public:
    int id;
    Position position;
    Dimension_ size;
    Coordinate coordinate;
    PolarCoordinate polarCoordinate;
    float weight;
    float volume;
    int time;
    int orig_rank;
    item() {
        orig_rank = -1;
        position.x = -1;
        position.y = -1;
        position.z = -1;
    }
    item(float w, float h, float d, float lat, float lng) {
        size.width = w;
        size.height = h;
        size.length = d;
        coordinate.latitude = lat;
        coordinate.longitude = lng;
        weight = 0;
        volume = w * h * d;
        orig_rank = -1;
        position.x = -1;
        position.y = -1;
        position.z = -1;
        time = 540;
    }
    item(float w, float h, float d, float lat, float lng, double providedEdd)
        {
            size.width = w;
            size.height = h;
            size.length = d;
            coordinate.latitude = lat;
            coordinate.longitude = lng;
            weight = 0;
            volume = w*h*d;
            time = providedEdd;
        }
    void print() {
        cout << coordinate.latitude << " " << coordinate.longitude << "\n";
        // cout<<"( Latitude - "<<coordinate.latitude<<" Longitude - "<<coordinate.longitude<<" Size - ("<<size.width<<", "<<size.length<<", "<<size.height<<") )";
        return;
    }
    void printToFile(std::ofstream& out) {
        out << coordinate.latitude << " " << coordinate.longitude << "\n";
        // out<<"( Latitude - "<<coordinate.latitude<<" Longitude - "<<coordinate.longitude<<" Size - ("<<size.width<<", "<<size.length<<", "<<size.height<<") )";
        return;
    }
    float getVolume() {
            return size.width * size.length * size.height;
    }
};

inline bool operator<(const item& a, const item& b){
    return a.coordinate.latitude < b.coordinate.latitude;
}

class Bin{
    public:
    Dimension_ size;
    float capacity;
    Bin(){}
    Bin(float w,float l, float h){
        size.width = w;
        size.length = l;
        size.height = h;
        capacity = 1000;
    }

    Bin(float w, float l, float h, float c){
        size.width = w;
        size.length = l;
        size.height = h;
        capacity = c;
    }

    float getCapacity(){
        return capacity;
    }

    float getVolume(){
        return size.width * size.length * size.height * 0.85;
    }

};

#endif

