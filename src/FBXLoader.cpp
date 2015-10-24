#include "FBXLoader.h"

int level = 0;

void PrintTabs() {
	for (int i = 0; i < level; i++) {
		printf("\t");
	}
}
//Return a string-based representation based on the attribute type.
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

bool loadFBXFromFile(const string& filename, MeshData *meshData) {
	level = 0;
	//Initialize the SDK manager. This object handles memory management.
	FbxManager* ISdkManager = FbxManager::Create();

	//Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(ISdkManager, IOSROOT);
	ISdkManager->SetIOSettings(ios);

	//Create an Importer using the SDKmanager.
	FbxImporter* IImporter = FbxImporter::Create(ISdkManager, "");

	//Call the initialise method which will load the contents of the FBX File
	if (!IImporter->Initialize(filename.c_str(), -1, ISdkManager->GetIOSettings())) {
		return false;
	}

	//Create a new scene so that it can be populated by the imported file.
	FbxScene* IScene = FbxScene::Create(ISdkManager, "myScene");
	//Import the contents of the file into the scene.
	IImporter->Import(IScene);

	//Process Nodes
	FbxNode* IRootNode = IScene->GetRootNode();
	if (IRootNode) {
		cout << "Root Node " << IRootNode->GetName() << endl;
		for (int i = 0; i < IRootNode->GetChildCount(); i++) {
			processNode(IRootNode->GetChild(i), meshData);
		}
	}
	IImporter->Destroy();
	return true;
}

void processNode(FbxNode *node, MeshData *meshData) {

}
void processAttribute(FbxNodeAttribute *attribute, MeshData *meshData) {

}
void processMesh(FbxMesh *mesh, MeshData *meshData) {

}
void processMeshTextureCoords(FbxMesh *mesh, Vertex *verts, int numVerts) {

}