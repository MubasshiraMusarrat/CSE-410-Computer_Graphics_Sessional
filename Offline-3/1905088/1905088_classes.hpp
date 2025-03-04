#ifndef CLASSES_HPP
#define CLASSES_HPP

#include <GL/glut.h>
#include <cmath>
#include <vector>

#define PI 3.1416
#define epsilon 1e-6

using namespace std;

class vector3D
{
public:
    double x, y, z;
    vector3D(double a, double b, double c) : x(a), y(b), z(c) {}
    vector3D() : x(0), y(0), z(0) {}

    inline vector3D operator+(const vector3D &v) const
    {
        return vector3D(x + v.x, y + v.y, z + v.z);
    }
    inline vector3D operator-(const vector3D &v) const
    {
        return vector3D(x - v.x, y - v.y, z - v.z);
    }
    inline vector3D operator*(double c) const
    {
        return vector3D(x * c, y * c, z * c);
    }
    inline double operator*(const vector3D &v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }
    inline vector3D X(const vector3D &v) const
    {
        return vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    vector3D operator=(const vector3D &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    inline double dist() const
    {
        return sqrt(x * x + y * y + z * z);

    }
    vector3D normalize()
    {
        double d = dist();
        x /= d;
        y /= d;
        z /= d;
        return *this;
    }
};

class camera{
    double rotationAngle;
public:
    vector3D pos, l, u, r;
    camera(){
        pos = vector3D(0, 0, 0);
        l = vector3D(0, 0, -1);
        u = vector3D(0, 1, 0);
        r = vector3D(1, 0, 0);
        rotationAngle = 1*PI/180;
    }
    
    void setCoords(vector3D a, vector3D b, vector3D c, double ang=1*PI/180){
        pos = a;
        l = b.normalize();
        u = c.normalize();
        r = l.X(u).normalize();
        rotationAngle = ang;
    }

    inline void moveForward(){
        pos = pos + l;
    }

    inline void moveBackward(){
        pos = pos - l;
    }

    inline void moveRight(){
        pos = pos + r;
    }

    inline void moveLeft(){
        pos = pos - r;
    }

    inline void moveUp(){
        pos = pos + u;
    }

    inline void moveDown(){
        pos = pos - u;
    }

    void lookLeft(){
        l = l * cos(rotationAngle) - r * sin(rotationAngle);
        l = l.normalize();
        r = l.X(u).normalize();
    }

    void lookRight(){
        l = l * cos(rotationAngle) + r * sin(rotationAngle);
        l = l.normalize();
        r = l.X(u).normalize();
    }

    void lookUp(){
        l = l * cos(rotationAngle) + u * sin(rotationAngle);
        l = l.normalize();
        u = r.X(l).normalize();
    }

    void lookDown(){
        l = l * cos(rotationAngle) - u * sin(rotationAngle);
        l = l.normalize();
        u = r.X(l).normalize();
    }

    void tiltClockwise(){
        u = u * cos(rotationAngle) - r * sin(rotationAngle);
        u = u.normalize();
        r = l.X(u).normalize();
    }

    void tiltCounterClockwise(){
        u = u * cos(rotationAngle) + r * sin(rotationAngle);
        u = u.normalize();
        r = l.X(u).normalize();
    }
};

class Ray{
public:
    vector3D start, dir;
    Ray(){}
    Ray(vector3D s, vector3D d){
        start = s;
        dir = d;
    }
    void normalize(){
        dir.normalize();
    }
};

class pointLight{
public:
    vector3D lightPos;
    double color[3];
    pointLight(){}
    pointLight(vector3D pos, double c[3]){
        lightPos = pos;
        for(int i=0; i<3; i++){
            color[i] = c[i];
        }
    }
    void draw(){
        glPushMatrix();
            glTranslatef(lightPos.x, lightPos.y, lightPos.z);
            glColor3f(color[0], color[1], color[2]);
            glutSolidSphere(0.5, 100, 100);
        glPopMatrix();
    }
};

class spotLight {
public:
    pointLight p;
    vector3D direction;
    double angle;
    spotLight(){}
    spotLight(vector3D lightPos, double color[3], vector3D dir, double a){
        p = pointLight(lightPos, color);
        direction = dir;
        angle = a;
    }
    void draw(){
        p.draw();
    }
};

class object;

int recurrsionLevel = 0;

extern vector<object*> objects;
extern vector<pointLight> pointLights;
extern vector<spotLight> spotLights;

class object{
public:
    vector3D refrencePoint;
    double param[3];
    double color[3];
    double light[4];
    int shine;
    object(){}
    void setColor(double c[3]){
        for(int i=0; i<3; i++){
            color[i] = c[i];
        }
    }
    void setLight(double l[4]){
        for(int i=0; i<4; i++){
            light[i] = l[i];
        }
    }
    void setShine(int s){
        shine = s;
    }
    virtual void draw() = 0;

    virtual double findTmin(Ray r){
        return -1;
    }

    virtual double* getColorAtPoint(vector3D p){
        return color;
    }

    virtual vector3D getNormalAtPoint(Ray r,vector3D p) = 0;

    double* fixColor(double *c){
        for(int i=0; i<3; i++){
            if(c[i]>1){
                c[i] = 1;
            }
            if(c[i]<0){
                c[i] = 0;
            }
        }
        return c;
    }

    virtual double intersect(Ray r, double *c, int level){
        double tmin = findTmin(r);
        if(tmin <0 || level == 0){
            return tmin;
        }
        vector3D intersectionPoint = r.start + r.dir * tmin;
        double *intersectionPointColor = getColorAtPoint(intersectionPoint);
        for(int i=0; i<3; i++){
            c[i] = intersectionPointColor[i] * light[0];
        }
        c = fixColor(c);
        vector3D normal = getNormalAtPoint(r, intersectionPoint);
        for(int i=0; i<pointLights.size(); i++){
          Ray lightRay(pointLights[i].lightPos, pointLights[i].lightPos-intersectionPoint);
          lightRay.normalize();
            double t = INT_MAX;
            int nearest;   
            for (int k = 0; k < objects.size(); k++)
            {
                double *tempColor = new double[3]{0, 0, 0};
                double tempT = objects[k]->intersect(lightRay, tempColor, 0);
                if(tempT >= 0 && tempT < t){
                    t = tempT;
                    nearest = k;
                }
            }
            if(tmin<t){
                double lambert = normal*lightRay.dir;
                lambert = max(lambert, 0.0);
                for(int j=0; j<3; j++){
                    c[j] += pointLights[i].color[j] * light[1] * lambert * intersectionPointColor[j];
                }
                fixColor(c);

                Ray reflectionRay(intersectionPoint, lightRay.dir - normal * 2 * (lightRay.dir*normal));
                reflectionRay.normalize();
                double phong = pow((reflectionRay.dir*r.dir), shine);
                phong = max(phong, 0.0);
                for(int j=0; j<3; j++){
                    c[j] += pointLights[i].color[j] * light[2] * phong * intersectionPointColor[j];
                }   
                fixColor(c);
            }
        }

        for(int i=0; i<spotLights.size(); i++){
            Ray lightRay(spotLights[i].p.lightPos, spotLights[i].p.lightPos - intersectionPoint);
            lightRay.normalize();

            double dot = lightRay.dir*spotLights[i].direction;
            dot/=(lightRay.dir.dist()*spotLights[i].direction.dist());
            double theta = acos(abs(dot));
            if(theta > spotLights[i].angle * PI / 180.0){
                continue;
            }

            double t = INT_MAX;
            int nearest;   
            for (int k = 0; k < objects.size(); k++)
            {
                double *tempColor = new double[3]{0, 0, 0};
                double tempT = objects[k]->intersect(lightRay, tempColor, 0);
                if(tempT >= 0 && tempT < t){
                    t = tempT;
                    nearest = k;
                }
            }
            if(tmin<t){
                double lambert = normal*lightRay.dir;
                lambert = max(lambert, 0.0);
                for(int j=0; j<3; j++){
                    c[j] += spotLights[i].p.color[j] * light[1] * lambert * intersectionPointColor[j];
                }
                fixColor(c);

                Ray reflectionRay(intersectionPoint, lightRay.dir - normal * 2 * (lightRay.dir*normal));
                reflectionRay.normalize();
                double phong = pow((reflectionRay.dir*r.dir), shine);
                phong = max(phong, 0.0);
                for(int j=0; j<3; j++){
                    c[j] += spotLights[i].p.color[j] * light[2] * phong * intersectionPointColor[j];
                }   
                fixColor(c);
            }
        }

        if(level >= recurrsionLevel){
            return tmin;
        }
    
        Ray reflectionRay(intersectionPoint, r.dir - normal * 2 * (r.dir*normal));
        reflectionRay.start = reflectionRay.start + reflectionRay.dir * epsilon;
        reflectionRay.normalize();
        
        double *reflectionColor = new double[3]{0, 0, 0};
        double t = INT_MAX;
        int nearest;
        for (int k = 0; k < objects.size(); k++)
        {
            double *tempColor = new double[3]{0, 0, 0};
            double tempT = objects[k]->intersect(reflectionRay, tempColor, 0);
            if(tempT >= 0 && tempT < t){
                t = tempT;
                nearest = k;
            }
        }
        if(t<INT_MAX){
            objects[nearest]->intersect(reflectionRay, reflectionColor, level+1);
            for(int i=0; i<3; i++){
                c[i] += reflectionColor[i] * light[3];
            }
            fixColor(c);
        }
        return tmin;
    }
};

class sphere: public object{
    vector3D center;
    double radius;
public:
    sphere(vector3D c, double r){
        this->refrencePoint = center = c;
        this->param[0] = radius = r;
        this->param[1] = 0;
        this->param[2] = 0;
    }

    void draw(){
        glPushMatrix();
            glTranslatef(center.x, center.y, center.z);
            glColor3f(color[0], color[1], color[2]);
            glutSolidSphere(radius, 100, 100);
        glPopMatrix();
    }

    double findTmin(Ray r){
        vector3D oc = r.start - center;
        double a = r.dir*r.dir;
        double b = 2*(oc*r.dir);
        double c = oc*oc - radius*radius;
        double discriminant = b*b - 4*a*c;
        if (discriminant < 0) {
            return -1;
        }
        else{
            double t = (-b - sqrt(discriminant))/(2*a);
            if(t<0){
                t = (-b + sqrt(discriminant))/(2*a);
            }
            return t;
        }
    }

    vector3D getNormalAtPoint(Ray r, vector3D p){
        vector3D n = p - center;
        n.normalize();
        if(r.dir*n > 0){
            n = n*-1;
        }
        return n;
    }
};

class triangle: public object{
    vector3D a, b, c; 
public:
    triangle(vector3D A, vector3D B, vector3D C){
        a = A;
        b = B;
        c = C;
    }
    void draw(){
        glPushMatrix();
        glBegin(GL_TRIANGLES);{
            glColor3f(color[0], color[1], color[2]);
            glVertex3f(a.x, a.y, a.z);
            glVertex3f(b.x, b.y, b.z);
            glVertex3f(c.x, c.y, c.z);
        }glEnd();
        glPopMatrix();
    }

    double getDeterminant(vector3D p , vector3D q, vector3D r){
        return p.x*(q.y*r.z - r.y*q.z) - q.x*(p.y*r.z - r.y*p.z) + r.x*(p.y*q.z - q.y*p.z);
    }

    double findTmin(Ray r){
        double detA = getDeterminant(a-b, a-c, r.dir);
        if (abs(detA) < 0) {
            return -1;
        }
        double detT = getDeterminant(a-b, a-c, a-r.start);
        double t = detT/detA;
        if(t<0){
            return -1;
        }
        double detBeta = getDeterminant(a-r.start, a-c, r.dir);
        double beta = detBeta/detA;
        if(beta<0 || beta>1){
            return -1;
        }
        double detGamma = getDeterminant(a-b, a-r.start, r.dir);
        double gamma = detGamma/detA;
        if(gamma<0 || gamma>1 || (beta+gamma)>1){
            return -1;
        }
        return t;
    }

    vector3D getNormalAtPoint(Ray r, vector3D p){
        vector3D normal = (b-a).X(c-a);
        normal.normalize();
        if(r.dir*normal > 0){
            normal = normal*-1;
        }
        return normal;
    }
};

struct quadricCoefficients{
    double A, B, C, D, E, F, G, H, I, J;
    
};

class general: public object{
    quadricCoefficients q;
    vector3D cubeRefPoint;
public:
    general(quadricCoefficients Q, vector3D refPoint, double param[3]){
        q = Q;
        cubeRefPoint = refPoint;
        for(int i=0; i<3; i++){
            this->param[i] = param[i];
        }
    }

    void draw(){}

    bool isInside(vector3D p){
        if(param[2]>0 && (p.z  < cubeRefPoint.z || p.z > cubeRefPoint.z+param[2])){
            return false;
        }
        if(param[1]>0 && (p.y < cubeRefPoint.y || p.y > cubeRefPoint.y+param[1])){
            return false;
        }
        if(param[0]>0 && (p.x < cubeRefPoint.x || p.x > cubeRefPoint.x+param[0])){
            return false;
        }
        return true;
    }

    double findTmin(Ray r){
        double A = q.A*r.dir.x*r.dir.x + q.B*r.dir.y*r.dir.y + q.C*r.dir.z*r.dir.z + q.D*r.dir.x*r.dir.y + q.E*r.dir.x*r.dir.z + q.F*r.dir.y*r.dir.z;
        double B = 2 * (q.A*r.dir.x*r.start.x + q.B*r.dir.y*r.start.y + q.C*r.dir.z*r.start.z)+ q.D*(r.start.y*r.dir.x + r.start.x*r.dir.y) + q.E*(r.start.x*r.dir.z + r.start.z*r.dir.x) + q.F*(r.start.y*r.dir.z + r.start.z*r.dir.y) + q.G*r.dir.x + q.H*r.dir.y + q.I*r.dir.z;
        double C = q.A*r.start.x*r.start.x + q.B*r.start.y*r.start.y + q.C*r.start.z*r.start.z + q.D*r.start.x*r.start.y + q.E*r.start.x*r.start.z + q.F*r.start.y*r.start.z + q.G*r.start.x + q.H*r.start.y + q.I*r.start.z + q.J;
        double t = -1;
        if(A == 0){
            if(B == 0){
                return -1;
            }
            t = -C/B;
            vector3D intersection = r.start + r.dir*t;
            if(!isInside(intersection)){
                return -1;
            } 
        }
        else{
            double det = B*B - 4*A*C;
            if(det < 0){
                return -1;
            }
            double t1 = (-B - sqrt(det))/(2*A);
            double t2 = (-B + sqrt(det))/(2*A);
            if(t1 > 0){
                t = t1;
                vector3D intersection = r.start + r.dir*t;
                if(!isInside(intersection)){
                    t = t1 = -1;
                } 
            }
            if(t1 < 0){
                if(t2 > 0){
                    t = t2;
                    vector3D intersection = r.start + r.dir*t;
                    if(!isInside(intersection)){
                        t = t2 = -1;
                    } 
                }
            }  
        }
        return t; 
    }

    vector3D getNormalAtPoint(Ray r, vector3D p){
        vector3D n;
        n.x = 2*q.A*p.x + q.D*p.y + q.E*p.z + q.G;
        n.y = 2*q.B*p.y + q.D*p.x + q.F*p.z + q.H;
        n.z = 2*q.C*p.z + q.E*p.x + q.F*p.y + q.I;
        n.normalize();
        if(r.dir*n > 0){
            n = n*-1;
        }
        return n;
    }
};

class Floor: public object{
    double floorWidth, tileWidth;
public:
    Floor(double floorWidth, double tileWidth){
        this->floorWidth = floorWidth;
        this->tileWidth = tileWidth;
        this->refrencePoint = vector3D(-floorWidth/2,-floorWidth/2,0);
        this->param[0] = tileWidth;
        this->param[1] = 0;
        this->param[2] = 0;
    }

    void draw(){
        glPushMatrix();
        for(int i=-floorWidth/2; i<=floorWidth/2; i+=tileWidth){
            for(int j=-floorWidth/2; j<=floorWidth/2; j+=tileWidth){
                if(static_cast<int>((i+j)/tileWidth)%2 == 0){
                    glColor3f(1, 1, 1);
                }
                else{
                    glColor3f(0, 0, 0);
                }
                glBegin(GL_QUADS);{
                    glVertex3f(i, j, 0);
                    glVertex3f(i+tileWidth, j, 0);
                    glVertex3f(i+tileWidth, j+tileWidth, 0);
                    glVertex3f(i, j+tileWidth, 0);
                }glEnd();
            }
        }
        glPopMatrix();
    }

     double findTmin(Ray r){
        vector3D n = vector3D(0, 0, 1);
        if(r.dir*n == 0){
            return -1;
        }
        double t = -(r.start*n) / (r.dir*n); // distance from origin D = 0
        vector3D intersection = r.start + r.dir*t;
        if(intersection.x < refrencePoint.x || intersection.x > refrencePoint.x+floorWidth || intersection.y < refrencePoint.y || intersection.y > refrencePoint.y+floorWidth){
            return -1;
        }
        return t;
     }

    double* getColorAtPoint(vector3D p){
        int i = (p.x - refrencePoint.x)/tileWidth;
        int j = (p.y - refrencePoint.y)/tileWidth;
        if((i+j)%2 == 0){
            color[0] = 1;
            color[1] = 1;
            color[2] = 1;
        }
        else{
            color[0] = 0;
            color[1] = 0;
            color[2] = 0;  
        }
        return color;
    }

    vector3D getNormalAtPoint(Ray r, vector3D p){
        if(r.dir*vector3D(0, 0, 1) < 0){
            return vector3D(0, 0, 1);
        }
        else{
            return vector3D(0, 0, -1);
        }
    }
};

#endif