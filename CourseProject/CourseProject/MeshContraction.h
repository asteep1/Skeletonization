#pragma once

#include "MeshLib_Core/Mesh.h"
#include "MeshLib_Core/Iterators.h"
#include <math.h>
#include <cmath>
#include "Data.h"

Mesh * contracted_mesh;
std::vector<double> L;
std::vector<double> LV;

void contract_mesh();
double cot(double angle);
void get_cotangent_weighting(Vertex * v);
void approx_inward_curve_flow();


void contract_mesh()
{
	*contracted_mesh = *pmesh;
	L.clear();
	L.resize(contracted_mesh->numEdges);
	for (MeshVertexIterator vit(contracted_mesh); !vit.end(); ++vit)
	{
		Vertex * v = *vit;
		get_cotangent_weighting(v);
		approx_inward_curve_flow();

	}

}

void get_cotangent_weighting(Vertex *v)
{
	for (VertexEdgeIterator veit(v); !veit.end(); ++veit)
	{
		Edge * e = *veit;
		//The two points of th edge
		double alpha_angle = e->he[0]->source()->index();
		double beta_angle = 180 - alpha_angle;
		double cot_alpha = cot(alpha_angle);
		double cot_beta = cot(beta_angle);
		if (e->he[0]->source()->boundary())
		{
			cot_alpha = 0.0f;
		}
		if (e->he[0]->target()->boundary())
		{
			cot_beta = 0.0f;
		}
		double wij = cot_alpha + cot_beta;
		if (isnan(wij))
		{
			wij = 0.0f;
		}
		L[e->index()] = wij;

	}
}

void approx_inward_curve_flow()
{
	LV.clear();
	LV.resize(contracted_mesh->numVertices());

}

double cot(double angle)
{
	return (1 / tan(angle));
}