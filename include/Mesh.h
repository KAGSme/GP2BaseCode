#pragma once
#ifndef _MESH_H
#define _MESH_H

#include "Vertices.h"
#include "Common.h"

struct MeshData
{
	vector<Vertex> vertices;
	vector<int> indices;

	int getNumVerts() {
		return vertices.size();
	};

	int getNumIndicies() {
		return indices.size();
	};

    ~Mesh(){
		vertices.clear();
		indices.clear();
	}

};

#endif
