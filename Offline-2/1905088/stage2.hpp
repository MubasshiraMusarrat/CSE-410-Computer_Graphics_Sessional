#ifndef STAGE2_HPP
#define STAGE2_HPP

#include "stage1.hpp"

using namespace std;

extern double eye[3];
extern double look[3];
extern double up[3];
extern vector<vector3D> vertices;

void viewTransformation(){
    ofstream output2File("stage2.txt");

    vector3D eyeV(eye[0], eye[1], eye[2]);
    vector3D lookV(look[0], look[1], look[2]);
    vector3D upV(up[0], up[1], up[2]);
    vector3D l = (lookV - eyeV).normalize();
    vector3D r = (l.cross(upV)).normalize();
    vector3D u = (r.cross(l)).normalize();

    Matrix T = translate(-eye[0], -eye[1], -eye[2]);

    Matrix R = R.IMatrix(4, 4);
    for (int i = 0; i < 3; i++){
        R.setVal(0, i, r.getVal(i));
        R.setVal(1, i, u.getVal(i));
        R.setVal(2, i, -l.getVal(i));
    }

    Matrix V = R * T;

    for (int i = 0; i < vertices.size(); i++){
        vector3D v = vertices[i];
        Matrix point = vecToMat(v);
        Matrix result = V * point;
        result = scalingW(result);
        // cout << result.getVal(0, 0) << " " << result.getVal(1, 0) << " " << result.getVal(2, 0) << " " << result.getVal(3, 0) << endl;
        for(int j=0; j<3 ; j++){
            v.setVal(j, result.getVal(j, 0));
            output2File << fixed << setprecision(7) << v.getVal(j) << " ";
        }
        output2File << endl;
        if(i%3 == 2) output2File << endl;
        vertices[i] = v;
    }

    output2File.close();
}

#endif // STAGE2_HPP