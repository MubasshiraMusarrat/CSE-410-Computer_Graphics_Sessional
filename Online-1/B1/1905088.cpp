//  g++ 1905088.cpp -o 1905088.exe -lfreeglut -lglew32 -lopengl32 -lglu32
//  .\1905088.exe

#include <bits/stdc++.h>
#include <windows.h>
#include <GL/glut.h>

#define PI 3.14159265358979323846

using namespace std;

float radius = 2;
float i = 0;
float j = 0;

void axes(){
    glScalef(10,10,10);
    glBegin(GL_LINES);
    {   
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(-1, 0, 0);
        glVertex3f(1, 0, 0);
        glVertex3f(0, -1, 0);
        glVertex3f(0, 1, 0);
        glVertex3f(0, 0, -1);
        glVertex3f(0, 0, 1);
    }glEnd();
}

void DrawCircle(float r, int num_segments)
{
    glBegin(GL_LINE_LOOP);
    for(int ii = 0; ii < num_segments; ii++)
    {
        float theta = 2.0f * PI * float(ii) / float(num_segments);//get the current angle

        float x = r * cosf(theta);//calculate the x component
        float y = r * sinf(theta);//calculate the y component

        glVertex2f(x, y);//output vertex

    }
    glEnd();
}

void display()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    // axes();

    glRotatef(i, 0, 0, 1);
    glPushMatrix();
        glTranslatef(radius, 0, 0);
        glColor3f(1.0, 0.0, 0.0);
        DrawCircle(radius/3, 100);

        glRotatef(j, 0, 0, 1);
        glPushMatrix();
            glTranslatef(radius/3, 0, 0);
            glColor3f(1.0, 1.0, 0.0);
            DrawCircle(radius/8, 100);
        glPopMatrix();

        glBegin(GL_LINES);
        {
            glColor3f(1.0, 0.0, 0.0);
            glVertex3f(0, 0, 0);
            glVertex3f(radius/3, 0, 0);
        }glEnd();
    glPopMatrix();

    glColor3f(0.0, 0.0, 1.0);
    DrawCircle(radius, 100);
        

    glBegin(GL_LINES);
    {
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0, 0, 0);
        glVertex3f(radius, 0, 0);
    }glEnd();

    glFlush();
}

void init(){
    // glClearColor(0.0f , 1.0f , 0.0f , 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 1000);
    // glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void Timer(int value){
    i++;
    j+=2;
    glutPostRedisplay();
    glutTimerFunc(30, Timer, 0);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(800, 500);
    glutInitWindowPosition(150, 50);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutCreateWindow("1905088");

    init();

    glutDisplayFunc(display);
    glutTimerFunc(0, Timer, 0);
    glutMainLoop();
    return 0;
}