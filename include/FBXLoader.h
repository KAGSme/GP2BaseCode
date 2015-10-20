#pragma once
#ifndef FBXLOADER_H
#define FBXLoader

#include "Common.h"
#include "Mesh.h"
#include "Vertices.h"
#include <fbxsdk.h>

bool loadFBXFromFile(const string& filename, MeshData *meshData);

void processNode(FbxNode *node, MeshData *meshData);
void processAttribute(FbxNodeAttribute *attribute, MeshData *meshData);
void processMesh(FbxMesh *mesh, MeshData *meshData);
void processMeshTextureCoords(FbxMesh *mesh, Vertex *verts, int numVerts);

#endif