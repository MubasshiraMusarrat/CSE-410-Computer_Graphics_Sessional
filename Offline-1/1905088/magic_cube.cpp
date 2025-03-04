#include <bits/stdc++.h>
#include <windows.h>
#include <GL/glut.h>

#include "camera.h"

using namespace std;

double coords[3] = {2.2, 2.2, 2.2};
double look[3] = {-1 / sqrt(3.0), -1 / sqrt(3.0), -1 / sqrt(3.0)};
double up[3] = {-1 / sqrt(6.0), 2 / sqrt(6.0), -1 / sqrt(6.0)};
Camera camera(coords, look, up);

double a = 1;

double octaX = 0;
double octaY = 0;
double octaZ = 0;
double octaScale = 1;

double radius = 1 / sqrt(3);

double rotatingAngle = 0;

double sphereX = 1;
double sphereY = 0;
double sphereZ = 0;
double sphereScale = 0;

double cylinderMaxH = sqrt(2);
double cylinderMaxX = 1 / sqrt(2);

double cylinderH = cylinderMaxH;
double cylinderTheta = M_PI / 2;
double cylinderX = cylinderMaxX;
double cylinderScale = 0;

double steps = 16;

void init()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 500);
}

void axes()
{
    glBegin(GL_LINES);
    {
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(100, 0, 0);
        glVertex3f(-100, 0, 0);

        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0, -100, 0);
        glVertex3f(0, 100, 0);

        glColor3f(1.0, 0.0, 1.0);
        glVertex3f(0, 0, 100);
        glVertex3f(0, 0, -100);
    }
    glEnd();
}

void drawFace()
{
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(a, 0, 0);
        glVertex3f(0, a, 0);
        glVertex3f(0, 0, a);
    }
    glEnd();
}

void drawOctahedronHead()
{
    for (int i = 0; i < 4; i++)
    {
        if (i % 2 == 0)
            glColor3f(1.0, 0.0, 1.0);
        else
            glColor3f(0.0, 1.0, 1.0);
        glRotatef(90, 0, 1, 0);
        glPushMatrix();
        {
            glTranslated(octaX, octaY, octaZ);
            glScaled(octaScale, octaScale, octaScale);
            drawFace();
        }
        glPopMatrix();
    }
}

void drawOctahedron()
{
    for (int i = 0; i < 2; i++)
    {
        glPushMatrix();
        {
            for (int i = 0; i < 4; i++)
            {
                if (i % 2 == 0)
                    glColor3f(1.0, 0.0, 1.0);
                else
                    glColor3f(0.0, 1.0, 1.0);
                glRotatef(90, 0, 1, 0);
                glPushMatrix();
                { 
                    glTranslated(octaX, octaY, octaZ);
                    glScaled(octaScale, octaScale, octaScale);
                    drawFace();
                }
                glPopMatrix();
            }
        }
        glPopMatrix();
        glRotated(180, 0, 0, 1);
    }
}

void drawSphereSlice(int slices)
{
    vector3D n1, n2, d;
    double theta, phi;

    int n = pow(2, slices) + 1;
    vector<vector<vector3D>> vertices(n, vector<vector3D>(n, vector3D()));

    for (int i = 0; i < n; i++)
    {
        phi = M_PI / 4 - M_PI / 2 * i / (n - 1);
        n2.x = -sin(phi);
        n2.y = cos(phi);
        n2.z = 0;

        for (int j = 0; j < n; j++)
        {
            theta = -M_PI / 4 + M_PI / 2 * j / (n - 1);
            n1.x = -sin(theta);
            n1.y = 0;
            n1.z = -cos(theta);

            d.x = n1.y * n2.z - n1.z * n2.y;
            d.y = n1.z * n2.x - n1.x * n2.z;
            d.z = n1.x * n2.y - n1.y * n2.x;

            double norm = sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
            d.x /= norm;
            d.x *= radius;
            d.y /= norm;
            d.y *= radius;
            d.z /= norm;
            d.z *= radius;

            vertices[i][j].x = d.x;
            vertices[i][j].y = d.y;
            vertices[i][j].z = d.z;
        }
    }

    glBegin(GL_QUADS);
    {
        for (int i = 0; i < n - 1; i++)
        {
            for (int j = 0; j < n - 1; j++)
            {
                glVertex3f(vertices[i][j].x, vertices[i][j].y, vertices[i][j].z);
                glVertex3f(vertices[i][j + 1].x, vertices[i][j + 1].y, vertices[i][j + 1].z);
                glVertex3f(vertices[i + 1][j + 1].x, vertices[i + 1][j + 1].y, vertices[i + 1][j + 1].z);
                glVertex3f(vertices[i + 1][j].x, vertices[i + 1][j].y, vertices[i + 1][j].z);
            }
        }
    }
    glEnd();
}

void drawSphere()
{
    glPushMatrix();
    {
        for (int i = 0; i < 4; i++)
        {
            if (i % 2 == 0)
                glColor3f(0.0, 0.0, 1.0);
            else
                glColor3f(0.0, 1.0, 0.0);
            glPushMatrix();
            {
                glTranslated(sphereX, 0, 0);
                glScaled(sphereScale, sphereScale, sphereScale);
                drawSphereSlice(6);
            }
            glPopMatrix();
            glRotated(90, 0, 1, 0);
        }
    }
    glPopMatrix();
    glPushMatrix();
    {
        for (int i = 1; i <= 2; i++)
        {
            glColor3f(1.0, 0.0, 0.0);
            glRotated(90 * i, 0, 0, 1);
            glPushMatrix();
            {
                glTranslated(sphereX, 0, 0);
                glScaled(sphereScale, sphereScale, sphereScale);
                drawSphereSlice(6);
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
}

void drawCylinder(int slices)
{
    // double dihedralAngle = acos(-1 / 3);
    double prevX = 0;
    double prevY = 0;
    double currX, currY;
    double theta = -cylinderTheta / 2;
    double dTheta = cylinderTheta / slices;

    glPushMatrix();
    {
        for (int i = 1; i <= slices; i++)
        {
            currX = radius * cos(theta);
            currY = radius * sin(theta);
            glBegin(GL_QUADS);
            {
                glColor3f(1.0, 1.0, 0.0);
                glVertex3f(currX, currY, cylinderH / 2);
                glVertex3f(currX, currY, -cylinderH / 2);
                glVertex3f(prevX, prevY, -cylinderH / 2);
                glVertex3f(prevX, prevY, cylinderH / 2);
            }
            glEnd();
            prevX = currX;
            prevY = currY;
            theta += dTheta;
        }
    }
    glPopMatrix();
}

void drawCylinderFrame()
{
    glPushMatrix();
    {
        for (int i = 0; i < 4; i++)
        {
            // drawCylinderFace();
            glPushMatrix();
            {
                glRotated(45, 0, 1, 0);
                glTranslated(cylinderX, 0, 0);
                glScaled(sphereScale, sphereScale, 1);
                drawCylinder(100);
            }
            glPopMatrix();
            glRotated(90, 0, 1, 0);
        }
    }
    glPopMatrix();
}

void sphereToOcta()
{
    double baseCenterX = 1 / 3;
    double baseCenterY = 1 / 3;
    double baseCenterZ = 1 / 3;
    if (sphereScale > 0.0 && octaScale < 1.0)
    {
        sphereScale -= 1 / steps;
        sphereX += 1 / steps;
        cylinderH += cylinderMaxH / steps;
        cylinderX += cylinderMaxX / steps;
        octaScale += 1 / steps;
        octaX -= baseCenterX / steps;
        octaY -= baseCenterY / steps;
        octaZ -= baseCenterZ / steps;
    }
}

void octaToSphere()
{
    double baseCenterX = 1 / 3;
    double baseCenterY = 1 / 3;
    double baseCenterZ = 1 / 3;
    if (octaScale > 0.0 && sphereScale < 1.0)
    {
        octaScale -= 1 / steps;
        octaX += baseCenterX / steps;
        octaY += baseCenterY / steps;
        octaZ += baseCenterZ / steps;
        sphereScale += 1 / steps;
        sphereX -= 1 / steps;
        cylinderH -= cylinderMaxH / steps;
        cylinderX -= cylinderMaxX / steps;
    }
}

void display()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera.View();

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // axes();

    // glScaled(1.5, 1.5, 1.5);
    glRotated(rotatingAngle, 0, 1, 0);

    glPushMatrix();
    {
        drawSphere();
    }
    glPopMatrix();

    glPushMatrix();
    {
        drawCylinderFrame();
        glRotatef(90, 1, 0, 0);
        drawCylinderFrame();
        glRotatef(90, 0, 0, 1);
        drawCylinderFrame();
    }
    glPopMatrix();

    glPushMatrix();
    {
        drawOctahedron();
    }
    glPopMatrix();

    glutSwapBuffers();
}

void idle()
{
    glutPostRedisplay();
}

void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        camera.MoveForward();
        break;
    case GLUT_KEY_DOWN:
        camera.MoveBackward();
        break;
    case GLUT_KEY_RIGHT:
        camera.MoveRight();
        break;
    case GLUT_KEY_LEFT:
        camera.MoveLeft();
        break;
    case GLUT_KEY_PAGE_UP:
        camera.MoveUp();
        break;
    case GLUT_KEY_PAGE_DOWN:
        camera.MoveDown();
        break;
    default:
        cout << "Invalid special key" << endl;
        break;
    }
}

void keyboardListener(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '1':
        camera.lookLeft();
        break;
    case '2':
        camera.lookRight();
        break;
    case '3':
        camera.lookUp();
        break;
    case '4':
        camera.lookDown();
        break;
    case '5':
        camera.tiltCounterClockwise();
        break;
    case '6':
        camera.tiltClockwise();
        break;
    case 'w':
        camera.MUWCRP();
        break;
    case 's':
        camera.MDWCRP();
        break;
    case 'a':
        rotatingAngle += 2;
        break;
    case 'd':
        rotatingAngle -= 2;
        break;
    case '.':
        sphereToOcta();
        break;
    case ',':
        octaToSphere();
        break;
    default:
        cout << "Invalid key" << endl;
        break;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(300, 70);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("task2");

    init();

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutSpecialFunc(specialKeyListener);
    glutKeyboardFunc(keyboardListener);
    glutMainLoop();
    return 0;
}