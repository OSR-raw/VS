#pragma once
//Incapsulates all necessary objects for kinect interaction + visual representation
#include "main.h"

class TriangleMesh;
class KinectReader;
class GridModel;
class VAO;

class KinectTool
{
public:
	KinectTool( float half_x, float half_y, float start_z, float end_z );
	~KinectTool();
	void DoToolUpdate();
	int InteractModel( GridModel* model, glm::quat quat );
	TriangleMesh* GetToolMesh();

private:
	KinectTool();
	float _start_z, _end_z;

	TriangleMesh* _msh;
	KinectReader* _reader;
};

