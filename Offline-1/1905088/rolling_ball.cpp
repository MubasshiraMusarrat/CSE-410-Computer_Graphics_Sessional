#include <bits/stdc++.h>
#include <windows.h>
#include <GL/glut.h>
#include <random>

#include "camera.h"

using namespace std;

double coords[3] = {19.29, 19.29, 20};
double look[3] = {-1 / sqrt(2), -1 / sqrt(2), 0};
double up[3] = {0, 0, 1};
Camera camera(coords, look, up);

int row = 64;
int col = 64;
int width = 5;

double wallCord = 15;

double radius = 1.5;
int sectorCount = 8;
int stackCount = 16;

double direction = rand() % 360; // with respect to y-axis
double speed = 0.5;

bool arrowPaused = false;

bool mode = true;

double BallX = 0;
double BallY = 0;

vector3D rotatingAxis = {0, 0, 0};
double rotatingAngle = 0;

double minCollisionTime = 0;

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

void drawCheckerBoard()
{
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            if ((i + j) % 2 == 0)
            {
                glColor3f(1.0, 1.0, 1.0);
            }
            else
            {
                glColor3f(0.0, 0.0, 0.0);
            }
            glBegin(GL_QUADS);
            {
                glVertex3f(width * i, width * j, 0);
                glVertex3f(width * (i + 1), width * j, 0);
                glVertex3f(width * (i + 1), width * (j + 1), 0);
                glVertex3f(width * i, width * (j + 1), 0);
            }
            glEnd();
        }
    }
}

void drawWalls()
{
    glColor3f(1.0, 0.0, 0.0);
    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        {
            glRotated(90 * i, 0, 0, 1);
            glBegin(GL_QUADS);
            {
                glVertex3f(-wallCord, -wallCord, 0);
                glVertex3f(-wallCord, -wallCord, 2);
                glVertex3f(wallCord, -wallCord, 2);
                glVertex3f(wallCord, -wallCord, 0);
            }
            glEnd();
        }
        glPopMatrix();
    }
}

void drawSphere()
{
    vector<double> vertices;
    vector<double> normals;
    vector<double> textures;

    double x, y, z, xy;
    double nx, ny, nz, lengthInv = 1.0 / radius;
    double s, t;

    int k1, k2;

    double sectorStep = 2 * M_PI / sectorCount;
    double stackStep = M_PI / stackCount;
    double sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cos(stackAngle);
        z = radius * sin(stackAngle);

        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;

            x = xy * cos(sectorAngle);
            y = xy * sin(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            s = (double)j / sectorCount;
            t = (double)i / stackCount;

            textures.push_back(s);
            textures.push_back(t);
        }
    }

    for (int i = 0; i <= stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if (i <= stackCount / 2)
            {
                if (j % 2 == 0)
                    glColor3f(1.0, 0.0, 0.0);
                else
                    glColor3f(0.0, 1.0, 0.0);
            }
            else
            {
                if (j % 2 == 0)
                    glColor3f(0.0, 1.0, 0.0);
                else
                    glColor3f(1.0, 0.0, 0.0);
            }
            if (i != 0)
            {
                glBegin(GL_TRIANGLES);
                {
                    glNormal3d(normals[3 * k1], normals[3 * k1 + 1], normals[3 * k1 + 2]);
                    glVertex3d(vertices[3 * k1], vertices[3 * k1 + 1], vertices[3 * k1 + 2]);
                    glTexCoord2d(textures[2 * k1], textures[2 * k1 + 1]);

                    glNormal3d(normals[3 * k2], normals[3 * k2 + 1], normals[3 * k2 + 2]);
                    glVertex3d(vertices[3 * k2], vertices[3 * k2 + 1], vertices[3 * k2 + 2]);
                    glTexCoord2d(textures[2 * k2], textures[2 * k2 + 1]);

                    glNormal3d(normals[3 * (k1 + 1)], normals[3 * (k1 + 1) + 1], normals[3 * (k1 + 1) + 2]);
                    glVertex3d(vertices[3 * (k1 + 1)], vertices[3 * (k1 + 1) + 1], vertices[3 * (k1 + 1) + 2]);
                    glTexCoord2d(textures[2 * (k1 + 1)], textures[2 * (k1 + 1) + 1]);
                }
                glEnd();
            }

            if (i != (stackCount - 1))
            {
                glBegin(GL_TRIANGLES);
                {
                    glNormal3d(normals[3 * (k1 + 1)], normals[3 * (k1 + 1) + 1], normals[3 * (k1 + 1) + 2]);
                    glVertex3d(vertices[3 * (k1 + 1)], vertices[3 * (k1 + 1) + 1], vertices[3 * (k1 + 1) + 2]);
                    glTexCoord2d(textures[2 * (k1 + 1)], textures[2 * (k1 + 1) + 1]);

                    glNormal3d(normals[3 * k2], normals[3 * k2 + 1], normals[3 * k2 + 2]);
                    glVertex3d(vertices[3 * k2], vertices[3 * k2 + 1], vertices[3 * k2 + 2]);
                    glTexCoord2d(textures[2 * k2], textures[2 * k2 + 1]);

                    glNormal3d(normals[3 * (k2 + 1)], normals[3 * (k2 + 1) + 1], normals[3 * (k2 + 1) + 2]);
                    glVertex3d(vertices[3 * (k2 + 1)], vertices[3 * (k2 + 1) + 1], vertices[3 * (k2 + 1) + 2]);
                    glTexCoord2d(textures[2 * (k2 + 1)], textures[2 * (k2 + 1) + 1]);
                }
                glEnd();
            }
        }
    }
}

void drawArrorw()
{
    glColor3f(0.0, 0.0, 1.0);
    GLUquadric *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluCylinder(quadric, 0.1, 0.1, 1, 20, 20);

    glTranslatef(0, 0, 1);
    glutSolidCone(0.2, 1, 20, 20);

    gluDeleteQuadric(quadric);
}

void drawZArrow()
{
    glColor3f(0.44, 0.85, 0.576);
    GLUquadric *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluCylinder(quadric, 0.05, 0.05, 7, 20, 20);

    glTranslatef(0, 0, 7);
    glutSolidCone(0.15, 2.5, 20, 20);

    gluDeleteQuadric(quadric);
}

void drawSphereAndArrow()
{
    glPushMatrix();
    {
        glTranslated(0, 0, radius);
        glRotated(rotatingAngle, rotatingAxis.x, rotatingAxis.y, 0);
        drawSphere();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(direction, 0, 0, 1);
        glPushMatrix();
        {
            glTranslated(0, 0, radius);
            glRotated(90, -1, 0, 0);
            glTranslated(0, 0, radius);
            glScaled(1.5, 1.5, 1.5);
            drawArrorw();
        }
        glPopMatrix();
    }
    glPopMatrix();

    if (!arrowPaused)
    {
        glPushMatrix();
        {
            glTranslated(0, 0, 2 * radius);
            drawZArrow();
        }
        glPopMatrix();
    }
}

void rotateSphere(double dir)
{
    vector3D directionVector = {-sin(dir * M_PI / 180), cos(dir * M_PI / 180), 0};
    vector3D ZAxis = {0, 0, 1};
    rotatingAxis.x = directionVector.y * ZAxis.z - directionVector.z * ZAxis.y;
    rotatingAxis.y = directionVector.z * ZAxis.x - directionVector.x * ZAxis.z;
    rotatingAngle -= 30;
    rotatingAngle = fmod(rotatingAngle, 360);
}

bool checkCollision()
{
    if ((BallX - speed * sin(direction * M_PI / 180) + radius) >= wallCord ||
        (BallX - speed * sin(direction * M_PI / 180) - radius) <= -wallCord ||
        (BallY + speed * cos(direction * M_PI / 180) + radius) >= wallCord ||
        (BallY + speed * cos(direction * M_PI / 180) - radius) <= -wallCord)
    {
        return true;
    }
    return false;
}

void goForwardManual()
{
    if (checkCollision())
    {
        direction += 180;
        direction = fmod(direction, 360);
    }
    rotateSphere(direction);
    BallX -= speed * sin(direction * M_PI / 180); // minus because the direction vector points to negative y-axis
    BallY += speed * cos(direction * M_PI / 180); // plus because the direction vector points to positive x-axis
}

void goBackwardManual()
{
    if (checkCollision())
    {
        direction += 180;
        direction = fmod(direction, 360);
    }
    rotateSphere(direction + 180); // backward = 180+theta
    BallX += speed * sin(direction * M_PI / 180);
    BallY -= speed * cos(direction * M_PI / 180);
}

void CollisionTimer(int value)
{
    if (checkCollision())
    {
        vector3D directionVector = {-sin(direction * M_PI / 180), cos(direction * M_PI / 180), 0};
        if ((BallX - speed * sin(direction * M_PI / 180) + radius) >= wallCord ||
            (BallX - speed * sin(direction * M_PI / 180) - radius) <= -wallCord)
        {
            directionVector.x *= -1;
        }
        if ((BallY + speed * cos(direction * M_PI / 180) + radius) >= wallCord ||
            (BallY + speed * cos(direction * M_PI / 180) - radius) <= -wallCord)
        {
            directionVector.y *= -1;
        }
        direction = atan2(directionVector.y, -directionVector.x) * 180 / M_PI;
        direction = fmod(direction, 360);
        minCollisionTime = 0;
    }
    else
    {
        double d = 0;
        d = min(abs(wallCord - (BallX + radius)), abs(-wallCord - (BallX - radius)));
        d = min(d, abs(wallCord - (BallY + radius)));
        d = min(d, abs(-wallCord - (BallY - radius)));
        minCollisionTime = d / speed;
        minCollisionTime *= 0.1;
    }
}

void simulationMode()
{
    rotateSphere(direction);
    BallX -= speed * sin(direction * M_PI / 180); // minus because the direction vector points to negative y-axis
    BallY += speed * cos(direction * M_PI / 180); // plus because the direction vector points to positive x-axis
    glutTimerFunc(minCollisionTime * 1000, CollisionTimer, 0);
}

void display()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera.View();

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // axes();

    glPushMatrix();
    {
        glTranslated(-row * width / 2, -col * width / 2, 0);
        drawCheckerBoard();
    }
    glPopMatrix();

    glPushMatrix();
    {
        drawWalls();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslated(BallX, BallY, 0);
        drawSphereAndArrow();
    }
    glPopMatrix();

    if (!mode)
    {
        simulationMode();
    }

    glutSwapBuffers();
}

void DisplayTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(100, DisplayTimer, 0);
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
    case ' ':
        minCollisionTime = 0;
        mode = !mode;
        break;
    case 'j':
        direction += 5;
        direction = fmod(direction, 360);
        break;
    case 'l':
        direction -= 5;
        direction = fmod(direction, 360);
        break;
    case 'i':
        if (mode)
        {
            goForwardManual();
        }
        break;
    case 'k':
        if (mode)
        {
            goBackwardManual();
        }
        break;
    default:
        cout << "Invalid key" << endl;
        break;
    }
}

void Timer(int value)
{
    arrowPaused = !arrowPaused;
    glutPostRedisplay();
    glutTimerFunc(10000, Timer, 0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(300, 70);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("task1");

    init();

    glutDisplayFunc(display);
    glutSpecialFunc(specialKeyListener);
    glutKeyboardFunc(keyboardListener);
    glutTimerFunc(0, DisplayTimer, 0);
    glutTimerFunc(0, Timer, 0);
    glutMainLoop();
    return 0;
}