#ifndef VECTOR3D_HPP
#define VECTOR3D_HPP

#include <iostream>
#include <math.h>

class vector3D
{
    double x, y, z;
public:
    vector3D( double x, double y, double z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    double getVal(int i){
        if(i == 0) return x;
        if(i == 1) return y;
        else return z;
    }

    void setVal(int i, double val){
        if(i == 0) x = val;
        if(i == 1) y = val;
        else z = val;
    }

    vector3D normalize(){
        double length = sqrt(x*x + y*y + z*z);
        return vector3D(x/length, y/length, z/length);
    }

    double dot(vector3D v){
        return x*v.x + y*v.y + z*v.z;
    }

    vector3D cross(vector3D v){
        return vector3D(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }

    vector3D operator*(double d){
        return vector3D(x*d, y*d, z*d);
    }

    vector3D operator+(vector3D v){
        return vector3D(x+v.x, y+v.y, z+v.z);
    }

    vector3D operator-(vector3D v){
        return vector3D(x-v.x, y-v.y, z-v.z);
    }

};


#endif // VECTOR3D_HPP