#pragma once

#include <GL/glut.h>
#include "Data.h"


float whratio;
int win_height, win_width;
GLUquadricObj* obj;

/* Some variables to measure mouse movement	*/
int mousePositionX0 = 0, mousePositionY0 = 0;
int mouseButton = 0;


/* Some variables to describe the object dimension and camera placement	*/
float cameraPosition[3] = { 0, 0, 2 };	//default camera position
float objCenter[3] = { 0, 0, 0 };			//default object center
float boxMin[3] = { 0, 0, 0 };
float boxMax[3] = { 0, 0, 0 };
float axislen = 1.414;

/* Some variables to control interactive transformations	*/
float my_Rotation_x = 0, my_Rotation_y = 0;
float my_Translation[3] = { 0, 0, 0 };

bool show_edges;


//rendering functions
void MyGLInit(void);
void SetCamera(void);
void Render_Mesh(void);
void Render_Edge(void);

//glut functions
void mouseMove(int x, int y);
void mouseClick(int button, int state, int x, int y);
void reshape(int w, int h);


void MyGLInit()
{
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT, GL_FILL);

	obj = gluNewQuadric();	//only for drawing spheres and cones		

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);

	// Create light components
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat	diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { cameraPosition[0], cameraPosition[1], cameraPosition[2], 1.0f }; // the light is on the camera position

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	show_edges = false;
}


void display(void)
{
	SetCamera();
	Render_Mesh();
	if (show_edges)
		Render_Edge();
	glutSwapBuffers();
}


void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	win_height = h;
	win_width = w;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	whratio = (double)w / (double)h; 	//A Commonly Suggested Setting: set ratio in gluPerspective to the aspect ratio of the associated viewport
	gluPerspective(60, whratio, axislen * 0.01, axislen * 5);
	glMatrixMode(GL_MODELVIEW);	//change back to modelview
	glutPostRedisplay();
}


void mykeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'e':
		show_edges = !show_edges;
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}


void SetCamera(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2], objCenter[0], objCenter[1], objCenter[2], 0, 1, 0);

	glTranslatef(my_Translation[0], my_Translation[1], my_Translation[2]);

	glTranslatef(objCenter[0], objCenter[1], objCenter[2]);	//before doing rotation to the object, move the object center to the origin

	glRotatef(my_Rotation_y, 0.0, 1.0, 0.0);
	glRotatef(my_Rotation_x, 1.0, 0.0, 0.0);

	glTranslatef(-objCenter[0], -objCenter[1], -objCenter[2]);
}


void mouseMove(int x, int y)
{
	double movingScale = axislen / win_height;  // just a scaling factor to make the mouse moving not too sensitive
	/* rotation*/
	if (mouseButton == GLUT_LEFT_BUTTON)
	{
		////////////do something////////////////
		my_Rotation_y += x - mousePositionX0;
		my_Rotation_x += y - mousePositionY0;
	}

	/*xy translation */
	if (mouseButton == GLUT_MIDDLE_BUTTON)
	{
		////////////do something ////////////////
		my_Translation[0] += movingScale * (x - mousePositionX0);
		my_Translation[1] -= movingScale * (y - mousePositionY0);
	}

	/* zoom in and out */
	if (mouseButton == GLUT_RIGHT_BUTTON)
	{ // suppose we want to make moving up as zooming out
		my_Translation[2] += movingScale * (y - mousePositionY0);
	}
	mousePositionX0 = x;
	mousePositionY0 = y;
	glutPostRedisplay();
}


void mouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		mouseButton = GLUT_LEFT_BUTTON;
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
		mouseButton = GLUT_MIDDLE_BUTTON;
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		mouseButton = GLUT_RIGHT_BUTTON;

	mousePositionX0 = x;
	mousePositionY0 = y;
	return;
}


//Rendering
void Render_Edge(void) {
	glColor3f(0, 0, 0.7);
	glLineWidth(2);
	glBegin(GL_LINES);
	for (MeshEdgeIterator eit(pmesh); !eit.end(); ++eit) {
		Edge* e = *eit;
		Point& p1 = e->he(0)->source()->point();
		Point& p2 = e->he(0)->target()->point();
		glVertex3f(p1[0], p1[1], p1[2]);
		glVertex3f(p2[0], p2[1], p2[2]);
	}
	glEnd();
	glLineWidth(1);
}

void Render_Mesh() {
	glColor3f(0.7f, 0.7f, 0.7f);
	//traverse all the face and draw them 
	glBegin(GL_TRIANGLES);
	for (MeshFaceIterator fit(pmesh); !fit.end(); ++fit) {
		Face* f = *fit;
		//Point & fn = fNormals[f->index()];
		for (FaceVertexIterator vit(f); !vit.end(); ++vit) {
			Vertex* v = *vit;
			Point& pt = v->point();
			Point& vn = vNormals[v->index()];
			glNormal3f(vn[0], vn[1], vn[2]);
			glVertex3f(pt[0], pt[1], pt[2]);
			glColor3f(0.7, 0.7, 0.7);
		}
	}
	glEnd();
}