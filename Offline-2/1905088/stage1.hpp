#ifndef STAGE1_HPP
#define STAGE1_HPP

#include <fstream>
#include <string>
#include <sstream>
#include <stack>
#include <vector>
#include <iomanip>

#include "vector3D.hpp"
#include "Matrix.hpp"

using namespace std;

double eye[3];
double look[3];
double up[3];
double fovY;
double aspectRatio;
double near;
double far;
vector<vector3D> vertices;

Matrix vecToMat(vector3D v){
    Matrix result(4, 1);
    for(int i = 0; i < 3; i++){
        result.setVal(i, 0, v.getVal(i));
    }
    result.setVal(3, 0, 1);
    return result;
}

Matrix translate(double tx, double ty, double tz){
    Matrix T = T.IMatrix(4, 4);
    T.setVal(0, 3, tx);
    T.setVal(1, 3, ty);
    T.setVal(2, 3, tz);
    return T;
}

Matrix scale(double sx, double sy, double sz){
    Matrix S = S.IMatrix(4, 4);
    S.setVal(0, 0, sx);
    S.setVal(1, 1, sy);
    S.setVal(2, 2, sz);
    return S;
}

vector3D R(vector3D x, vector3D a, double angle){
    vector3D result = x * cos(angle) + a * (a.dot(x)) * (1 - cos(angle)) + (a.cross(x)) * sin(angle);
    return result;
}

Matrix rotate(double angle, double ax, double ay, double az){
    vector3D axis(ax, ay, az);
    axis = axis.normalize();
    double rad = angle * M_PI / 180;

    vector3D c1 = R(vector3D(1, 0, 0), axis, rad);
    vector3D c2 = R(vector3D(0, 1, 0), axis, rad);
    vector3D c3 = R(vector3D(0, 0, 1), axis, rad);

    Matrix R = R.IMatrix(4, 4);
    for (int i = 0; i < 3; i++){
        R.setVal(i, 0, c1.getVal(i));
        R.setVal(i, 1, c2.getVal(i));
        R.setVal(i, 2, c3.getVal(i));
    }
    return R;
}

Matrix scalingW(Matrix point){
    Matrix result = point;
    if(point.getVal(3, 0) != 1){
        result.setVal(0, 0, point.getVal(0, 0) / point.getVal(3, 0));
        result.setVal(1, 0, point.getVal(1, 0) / point.getVal(3, 0));
        result.setVal(2, 0, point.getVal(2, 0) / point.getVal(3, 0));
        result.setVal(3, 0, 1);
    }
    return result;
}

void modelingTransformation(){
    ifstream sceneFile("scene.txt");
    ofstream output1File("stage1.txt");
    string line;
    stack<Matrix> transformationStack;
    Matrix M = M.IMatrix(4, 4);

    // first 4 lines
    sceneFile >> eye[0] >> eye[1] >> eye[2] 
    >> look[0] >> look[1] >> look[2] >> 
    up[0] >> up[1] >> up[2] >> 
    fovY >> aspectRatio >> near >> far;

    while(getline(sceneFile, line)){
        if(line == "triangle"){
            for (int i = 0; i < 3; i++){
                getline(sceneFile, line);
                stringstream ss(line);
                double x, y, z;
                ss >> x >> y >> z;

                vector3D v(x, y, z);
                Matrix point = vecToMat(v);
                point = M * point;
                point = scalingW(point);

                // cout << point.getVal(0, 0) << " " << point.getVal(1, 0) << " " << point.getVal(2, 0) << " " << point.getVal(3, 0) << endl;
                for(int j=0; j<3 ; j++){
                    v.setVal(j, point.getVal(j, 0));
                    output1File << fixed << setprecision(7) << v.getVal(j) << " ";
                }
                vertices.push_back(v);
                output1File << endl;
            }   
            output1File << endl;
        }else if(line == "translate"){
            getline(sceneFile, line);
            stringstream ss(line);
            double tx, ty, tz;
            ss >> tx >> ty >> tz;
            Matrix T = translate(tx, ty, tz);
            M = M * T;
        } else if(line == "scale"){
            getline(sceneFile, line);
            stringstream ss(line);
            double sx, sy, sz;
            ss >> sx >> sy >> sz;
            Matrix S = scale(sx, sy, sz);
            M = M * S;
        } else if(line == "rotate"){
            getline(sceneFile, line);
            stringstream ss(line);
            double angle, ax, ay, az;
            ss >> angle >> ax >> ay >> az;
            Matrix R = rotate(angle, ax, ay, az);
            M = M * R;
        } else if (line == "push"){
            transformationStack.push(M);
            // cout << transformationStack.size() << endl;
        } else if (line == "pop"){
            M = transformationStack.top();
            transformationStack.pop();
            // cout << transformationStack.size() << endl;
        } else if (line == "end"){
            break;
        }
    }

    sceneFile.close();
    output1File.close();
}

#endif // STAGE1_HPP