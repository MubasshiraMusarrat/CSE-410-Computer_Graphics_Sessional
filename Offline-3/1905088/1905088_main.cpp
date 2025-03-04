// g++ 1905088_main.cpp -o main.exe -lfreeglut -lglew32 -lopengl32 -lglu32
#include <iostream>
#include <GL/glut.h>
#include <fstream>

#include "bitmap_image.hpp"
#include "1905088_classes.hpp"

using namespace std;

camera c;

vector<object*> objects;
vector<pointLight> pointLights;
vector<spotLight> spotLights;

double pixelWidth, pixelHeight;
double windowWidth = 500, windowHeight = 500;
double viewAngle = 80;
int noOfCapture = 1;

void init(){
    vector3D pos(115, 115, 50);
    vector3D u(0, 0, 1);
    vector3D l(-1/sqrt(2), -1/sqrt(2), 0);

    c.setCoords(pos, l, u);

    glClearColor(0.0 , 0.0 , 0.0 , 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(viewAngle, 1, 1, 1000.0);
}

void loadData(){
    ifstream fin("scene.txt");
    fin >> recurrsionLevel;
    fin >> pixelWidth;
    pixelHeight = pixelWidth;
    int numberOfObjects;
    fin >> numberOfObjects;
    object* o;

    for(int i=0; i<numberOfObjects; i++){
    
        string objectType;
        fin >> objectType;
        if(objectType == "sphere"){
            vector3D center;
            fin >> center.x >> center.y >> center.z;
            double radius;
            fin >> radius;
            o = new sphere(center, radius);
        }
        else if(objectType == "triangle"){
            vector3D a, b, c;
            fin >> a.x >> a.y >> a.z;
            fin >> b.x >> b.y >> b.z;
            fin >> c.x >> c.y >> c.z;
            o = new triangle(a, b, c);
        }
        else if(objectType == "general"){
            quadricCoefficients q;
            fin >> q.A >> q.B >> q.C >> q.D >> q.E >> q.F >> q.G >> q.H >> q.I >> q.J;
            vector3D refPoint;
            fin >> refPoint.x >> refPoint.y >> refPoint.z;
            double param[3];
            fin >> param[0] >> param[1] >> param[2];
            o = new general(q, refPoint, param);
        }
        else{
            cout << "Invalid object type" << endl;
        }

        double color[3];
        fin >> color[0] >> color[1] >> color[2];
        double light[4];
        fin >> light[0] >> light[1] >> light[2] >> light[3];
        int shininess;
        fin >> shininess;

        o->setColor(color);
        o->setLight(light);
        o->setShine(shininess);
        objects.push_back(o);
    }

    int numberOfPointLights;
    fin >> numberOfPointLights;
    for(int i=0; i<numberOfPointLights; i++){
        vector3D lightPos;
        fin >> lightPos.x >> lightPos.y >> lightPos.z;
        double color[3];
        fin >> color[0] >> color[1] >> color[2];
        pointLight p(lightPos, color);
        pointLights.push_back(p);
    }

    int numberOfSpotLights;
    fin >> numberOfSpotLights;
    for(int i=0; i<numberOfSpotLights; i++){
        vector3D lightPos;
        fin >> lightPos.x >> lightPos.y >> lightPos.z;
        double color[3];
        fin >> color[0] >> color[1] >> color[2];
        vector3D direction;
        fin >> direction.x >> direction.y >> direction.z;
        double cutoffAngle;
        fin >> cutoffAngle;
        spotLight s(lightPos, color, direction, cutoffAngle);
    }
    o = new Floor(1000,20);
    o->setLight(new double[4]{0.4,0.2,0.2,0.2});
    o->setShine(10);
    objects.push_back(o);
    fin.close();
}

void drawAxes(){
    glBegin(GL_LINES);{ 
        glColor3f(1, 0, 0);
        glVertex3f(100, 0, 0);
        glVertex3f(-100, 0, 0);
        glColor3f(0, 1, 0);
        glVertex3f(0, 100, 0);
        glVertex3f(0, -100, 0);
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, 100);
        glVertex3f(0, 0, -100);
    }glEnd();
}

void capture(){
    // cout << c.pos.x <<c.pos.y << c.pos.z <<endl;
    bitmap_image image(pixelWidth, pixelHeight);
    image.set_all_channels(0, 0, 0);
    double planeDistance = (windowHeight/2.0) / tan((viewAngle/2.0) * (PI/180.0));
    vector3D topleft = c.pos + c.l * planeDistance - c.r * (windowWidth/2.0) + c.u * (windowHeight/2.0);
    double du = windowWidth / pixelWidth;
    double dv = windowHeight / pixelHeight;
    topleft = topleft + c.r * (du/2.0) - c.u * (dv/2.0);

    for(int i=1; i<=pixelWidth; i++){
        for(int j=1; j<=pixelHeight; j++){
            vector3D curPixel = topleft + c.r * du * (i-1) - c.u * dv * (j-1);
            vector3D dir = curPixel - c.pos;
            Ray ray(c.pos, dir);
            ray.normalize();

            double *color = new double[3]{0, 0, 0};
            double tmin = INT_MAX;
            int nearest;
            for (int k = 0; k < objects.size(); k++)
            {
                double t = objects[k]->intersect(ray, color ,0);
                if (t >= 0 && t < tmin)
                {
                    tmin = t;
                    nearest = k;
                }
            }
            if (tmin < INT_MAX)
            {
                objects[nearest]->intersect(ray, color, 1);
                image.set_pixel(i, j, color[0] * 255, color[1] * 255, color[2] * 255);
            }
            delete[] color;
        }
    }
    image.save_image("Output_1"+to_string(noOfCapture)+".bmp");
    image.clear();
    noOfCapture++;
    cout << "Image captured" << endl;
}

void display(){
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(c.pos.x, c.pos.y, c.pos.z, c.pos.x+c.l.x, c.pos.y+c.l.y, c.pos.z+c.l.z, c.u.x, c.u.y, c.u.z);

    // drawAxes();
    for(int i=0; i<objects.size(); i++){
        objects[i]->draw();
    }
    for(int i=0; i<pointLights.size(); i++){
        pointLights[i].draw();
    }
    for(int i=0; i<spotLights.size(); i++){
        spotLights[i].draw();
    }

    glutSwapBuffers();
}

void specialKeyListener(int key, int x, int y){
    switch (key)
    {
    case GLUT_KEY_UP:
        c.moveForward();
        break;
    case GLUT_KEY_DOWN:
        c.moveBackward();
        break;
    case GLUT_KEY_LEFT:
        c.moveLeft();
        break;
    case GLUT_KEY_RIGHT:
        c.moveRight();
        break;
    case GLUT_KEY_PAGE_UP:
        c.moveUp();
        break;
    case GLUT_KEY_PAGE_DOWN:
        c.moveDown();
        break;
    default:
        cout << "Invalid special key" << endl;
        break;
    }
}

void keyboardListener(unsigned char key, int x, int y){
    switch (key)
    {
    case '0':
        capture();
        break;
    case '1':
        c.lookLeft();
        break;
    case '2':
        c.lookRight();
        break;
    case '3':
        c.lookUp();
        break;
    case '4':
        c.lookDown();
        break;
    case '5':
        c.tiltClockwise();
        break;
    case '6':
        c.tiltCounterClockwise();
        break;
    default:
        cout << "Invalid key" << endl;
        break;
    }
}

void idle(){
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(10, 10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("ray tracing");

    init();
    loadData();

    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutSpecialFunc(specialKeyListener);
    glutKeyboardFunc(keyboardListener);

    glutMainLoop();
    for(int i=0; i<objects.size(); i++){
        delete objects[i];
    }
    objects.clear();
    pointLights.clear();
    spotLights.clear();
    return 0;
}