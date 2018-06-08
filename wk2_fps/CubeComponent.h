#pragma once

#include "DrawComponent.h"
#include <vector>
#include <GL/freeglut.h>

using namespace std;

struct Vertex
{
	float x, y, z;
	float nx, ny, nz;
	float r, g, b, a;
	float tx, ty;
};

class CubeComponent : public DrawComponent
{
	float size;
	bool DrawOutline = false;
	vector<Vertex> vertex;
	GLuint terrainTextureId;

public:
	CubeComponent(float size, vector<Vertex> vertex, GLuint terrainTextureId);
	~CubeComponent();

	virtual void draw() override;
	void drawOutline();
};

