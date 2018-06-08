#include "CubeComponent.h"
#include <GL/freeglut.h>


CubeComponent::CubeComponent(float size, vector<Vertex> vertex, GLuint terrainTextureId)
{
	this->size = size;
	this->vertex = vertex;
	this->terrainTextureId = terrainTextureId;
}


CubeComponent::~CubeComponent()
{
}

void CubeComponent::draw()
{
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);

	/*glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);*/

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, terrainTextureId);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), ((float*)vertex.data()) + 0);
	glNormalPointer(GL_FLOAT, sizeof(Vertex), ((float*)vertex.data()) + 3);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), ((float*)vertex.data()) + 10);
	glColorPointer(4, GL_FLOAT, sizeof(Vertex), ((float*)vertex.data()) + 6);
	glDrawArrays(GL_QUADS, 0, vertex.size());

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY_EXT);

	if (DrawOutline) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(255, 255, 255);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), ((float*)vertex.data()) + 0);
		glDrawArrays(GL_QUADS, 0, vertex.size());
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisableClientState(GL_VERTEX_ARRAY);
		DrawOutline = false;
	}
}

void CubeComponent::drawOutline() {
	DrawOutline = true;
}
