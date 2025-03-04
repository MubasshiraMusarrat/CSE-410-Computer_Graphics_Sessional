#ifndef CAMERA_H
#define CAMERA_H

#include <bits/stdc++.h>
#include <windows.h>
#include <GL/glut.h>

using namespace std;

struct vector3D{
    double x, y, z;
};

class Camera{
    vector3D pos, l, u, r;
public:
    Camera(double a[], double b[], double c[]){
    pos.x = a[0];
    pos.y = a[1];
    pos.z = a[2];
    l.x = b[0];
    l.y = b[1];
    l.z = b[2];
    u.x = c[0];
    u.y = c[1];
    u.z = c[2];
    r.x = u.y * l.z - u.z * l.y;
    r.y = u.z * l.x - u.x * l.z;
    r.z = u.x * l.y - u.y * l.x;
    }

    void View(){
        gluLookAt(pos.x, pos.y, pos.z, 
            pos.x+l.x, pos.y+l.y, pos.y+l.z, 
            u.x, u.y, u.z);
    }

    void MoveForward(){
        pos.x += l.x;
        pos.y += l.y;
        pos.z += l.z;
    }

    void MoveBackward(){
        pos.x -= l.x;
        pos.y -= l.y;
        pos.z -= l.z;
    }

    void MoveRight(){
        pos.x -= r.x;
        pos.y -= r.y;
        pos.z -= r.z;
    }

    void MoveLeft(){
        pos.x += r.x;
        pos.y += r.y;
        pos.z += r.z;
    }

    void MoveUp(){
        pos.x += u.x;
        pos.y += u.y;
        pos.z += u.z;
    }

    void MoveDown(){
        pos.x -= u.x;
        pos.y -= u.y;
        pos.z -= u.z;
    }

    void lookLeft(){
        r.x = r.x*cos(0.1) + l.x*sin(0.1);
        r.y = r.y*cos(0.1) + l.y*sin(0.1);
        r.z = r.z*cos(0.1) + l.z*sin(0.1);

        l.x = l.x*cos(0.1) - r.x*sin(0.1);
        l.y = l.y*cos(0.1) - r.y*sin(0.1);
        l.z = l.z*cos(0.1) - r.z*sin(0.1);
    }

    void lookRight(){
        r.x = r.x*cos(0.1) - l.x*sin(0.1);
        r.y = r.y*cos(0.1) - l.y*sin(0.1);
        r.z = r.z*cos(0.1) - l.z*sin(0.1);

        l.x = l.x*cos(0.1) + r.x*sin(0.1);
        l.y = l.y*cos(0.1) + r.y*sin(0.1);
        l.z = l.z*cos(0.1) + r.z*sin(0.1);
    }

    void lookUp(){
        l.x = l.x*cos(0.1) + u.x*sin(0.1);
        l.y = l.y*cos(0.1) + u.y*sin(0.1);
        l.z = l.z*cos(0.1) + u.z*sin(0.1);

        u.x = u.x*cos(0.1) - l.x*sin(0.1);
        u.y = u.y*cos(0.1) - l.y*sin(0.1);
        u.z = u.z*cos(0.1) - l.z*sin(0.1);
    }

    void lookDown(){
        l.x = l.x*cos(0.1) - u.x*sin(0.1);
        l.y = l.y*cos(0.1) - u.y*sin(0.1);
        l.z = l.z*cos(0.1) - u.z*sin(0.1);

        u.x = u.x*cos(0.1) + l.x*sin(0.1);
        u.y = u.y*cos(0.1) + l.y*sin(0.1);
        u.z = u.z*cos(0.1) + l.z*sin(0.1);
    }

    void tiltCounterClockwise(){
        u.x = u.x*cos(0.1) + r.x*sin(0.1);
        u.y = u.y*cos(0.1) + r.y*sin(0.1);
        u.z = u.z*cos(0.1) + r.z*sin(0.1);

        r.x = r.x*cos(0.1) - u.x*sin(0.1);
        r.y = r.y*cos(0.1) - u.y*sin(0.1);
        r.z = r.z*cos(0.1) - u.z*sin(0.1);
    }

    void tiltClockwise(){
        u.x = u.x*cos(0.1) - r.x*sin(0.1);
        u.y = u.y*cos(0.1) - r.y*sin(0.1);
        u.z = u.z*cos(0.1) - r.z*sin(0.1);

        r.x = r.x*cos(0.1) + u.x*sin(0.1);
        r.y = r.y*cos(0.1) + u.y*sin(0.1);
        r.z = r.z*cos(0.1) + u.z*sin(0.1);
    }

    void MUWCRP(){
        vector3D center;

        center.x = pos.x + l.x;
        center.y = pos.y + l.y;
        center.z = pos.z + l.z;

        pos.x += u.x;
        pos.y += u.y;
        pos.z += u.z;

        l.x = center.x - pos.x;
        l.y = center.y - pos.y;
        l.z = center.z - pos.z;
    }

    void MDWCRP(){
        vector3D center;

        center.x = pos.x + l.x;
        center.y = pos.y + l.y;
        center.z = pos.z + l.z;

        pos.x -= u.x;
        pos.y -= u.y;
        pos.z -= u.z;

        l.x = center.x - pos.x;
        l.y = center.y - pos.y;
        l.z = center.z - pos.z;
    }

};

#endif // CAMERA_H