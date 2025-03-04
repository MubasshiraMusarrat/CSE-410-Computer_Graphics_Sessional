#ifndef STAGE3_HPP
#define STAGE3_HPP

#include "stage2.hpp"

using namespace std;

extern double fovY;
extern double aspectRatio;
extern double near;
extern double far;
extern vector<vector3D> vertices;

void projectionTransformation(){
    ofstream output3File("stage3.txt");

    double fovX = fovY * aspectRatio;
    double t = near * tan(fovY * M_PI / 360);
    double r = near * tan(fovX * M_PI / 360);

    Matrix P(4, 4);
    P.setVal(0, 0, near/r);
    P.setVal(1, 1, near/t);
    P.setVal(2, 2, -(far+near)/(far-near));
    P.setVal(2, 3, -(2*far*near)/(far-near));
    P.setVal(3, 2, -1);

    for (int i = 0; i < vertices.size(); i++){
        vector3D v = vertices[i];
        Matrix point = vecToMat(v);
        Matrix result = P * point;
        result = scalingW(result);

        // cout << result.getVal(0, 0) << " " << result.getVal(1, 0) << " " << result.getVal(2, 0) << " " << result.getVal(3, 0) << endl;
        for(int j=0; j<3 ; j++){
            v.setVal(j, result.getVal(j, 0));
            output3File << fixed << setprecision(7) << v.getVal(j) << " ";
        }
        output3File << endl;
        if(i%3 == 2) output3File << endl;
        vertices[i] = v;
    }

    output3File.close();
}

#endif // STAGE3_HPP