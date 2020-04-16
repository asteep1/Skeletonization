#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "MeshLib_Core/Mesh.h"
#include "MeshLib_Core/Iterators.h"
#include "Data.h"
#include "GUI.h"
#include "Computation.h"


int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cout << "Error in command arguments";
		std::cout << "Usage: " << argv[0] << " input_mesh.obj\n";
		return -1;
	}
	pmesh = new Mesh();

	bool flag = pmesh->readOBJFile(argv[1]);

	if (!flag) {
		std::cerr << "Fail to read " << argv[1] << "\n";
		return -2;
	}

	// Computing geometric attributes and store them
	ComputeBoundingBox();
	ComputeAllFaceNormals();
	ComputeAllCornerAngles();
	ComputeAllVertNormals();
	
	//OpenGL Routines
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("EE 4730 Course Project Group 7 Skeletonization");
	MyGLInit();

	//Register Callback Functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(mykeyboard);
	glutMainLoop();

	delete pmesh;
	return 0;
}
