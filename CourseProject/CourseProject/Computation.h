#pragma once

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

void ComputeAllCornerAngles() {
	heAngles.clear();
	heAngles.resize(pmesh->numEdges() * 2);
	for (MeshFaceIterator fit(pmesh); !fit.end(); ++fit) {
		Face * f = *fit;
		Halfedge * he[3];
		he[0] = f->he();
		he[1] = he[0]->next(); he[2] = he[1]->next();
		Point pt[3];
		for (int i = 0; i < 3; ++i) {
			pt[i] = he[i]->next()->target()->point();
		}
		double len[3];
		for (int i = 0; i < 3; ++i) {
			len[i] = (he[i]->target()->point()
				- he[i]->source()->point()).norm();
		}
		double alpha[3];
		for (int i = 0; i < 3; ++i) {
			double l_a = len[(i + 1) % 3];
			double l_b = len[(i + 2) % 3];
			double nominator = l_a * l_a + l_b * l_b - len[i] * len[i];
			double denominator = 2 * l_a * l_b;
			alpha[i] = acos(nominator / denominator);
			heAngles[he[i]->next()->index()] = alpha[i];
		}
	}
}

Point ComputeFaceNormal(Face *f) {
	Halfedge * he1 = f->he();
	Halfedge * he2 = he1->next();
	Point p1 = he1->source()->point();
	Point p2 = he1->target()->point();
	Point p3 = he2->target()->point();
	Point p12 = p2 - p1;
	Point p23 = p3 - p2;
	Point p = p12 ^ p23;
	p = p / p.norm();
	return p;
}

void ComputeAllFaceNormals() {
	fNormals.clear();
	fNormals.resize(pmesh->numFaces());
	for (MeshFaceIterator fit(pmesh); !fit.end(); ++fit) {
		Face * f = *fit;
		Point fn = ComputeFaceNormal(f);
		fNormals[f->index()] = fn;
		//fNormals.push_back(fn);
	}
}

void ComputeAllVertNormals() {
	vNormals.clear();
	vNormals.resize(pmesh->numVertices());
	for (MeshVertexIterator vit(pmesh); !vit.end(); ++vit) {
		Vertex * v = *vit;
		Point vn(0, 0, 0);
		for (VertexInHalfedgeIterator heit(v); !heit.end(); ++heit) {
			//for (VertexFaceIterator vfit(v); !vfit.end(); ++vfit){
				//Face * f = *vfit;
			Halfedge * he = *heit;
			Face * f = he->face();
			Point & fn = fNormals[f->index()];
			double alpha = heAngles[he->index()];
			Point cfn = fn * alpha;
			vn += cfn;
		}
		vn /= vn.norm();
		vNormals[v->index()] = vn;
	}

}

void ComputeBoundingBox() {
	objCenter[0] = objCenter[1] = objCenter[2] = 0;
	boxMin[0] = boxMin[1] = boxMin[2] = 1e5;
	boxMax[0] = boxMax[1] = boxMax[2] = -1e5;
	int vNum = pmesh->numVertices();
	for (MeshVertexIterator it(pmesh); !it.end(); ++it) {
		Vertex * v = *it;
		Point & pt = v->point();
		for (int i = 0; i < 3; ++i) {
			objCenter[i] += pt[i];
			if (boxMin[i] > pt[i])
				boxMin[i] = pt[i];
			if (boxMax[i] < pt[i])
				boxMax[i] = pt[i];
		}
	}
	axislen = sqrt((boxMax[2] - boxMin[2])*(boxMax[2] - boxMin[2]) + (boxMax[1] - boxMin[1])*(boxMax[1] - boxMin[1]) + (boxMax[0] - boxMin[0])*(boxMax[0] - boxMin[0]));
	objCenter[0] /= vNum;
	objCenter[1] /= vNum;
	objCenter[2] /= vNum;

	cameraPosition[0] = objCenter[0];
	cameraPosition[1] = objCenter[1];
	cameraPosition[2] = objCenter[2] + axislen * 1.5;
}