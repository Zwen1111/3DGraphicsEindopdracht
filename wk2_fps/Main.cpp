#include <GL/freeglut.h>
#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <vector>
#include <string>
#include "ModelComponent.h"
#include "GameObject.h"
#include "CubeComponent.h"
#include "PlayerComponent.h"
#include <math.h>
#include <Windows.h>
#include <Mmsystem.h>

using namespace std;

#define PI 3.14159265

float lastFrameTime = 0; 
float deltaTime = 0;

int width, height;

bool showMenu = true;

GLuint textures[3];
int heightmap[128][128][10];

std::list<GameObject*> objects;
GameObject* player;

struct Camera
{
	float posX = 0;
	float posY = -20;
	float rotX = 0;
	float rotY = 0;
	float posZ = -10;
} camera;

bool keys[255];

/*
Drawing text 2D screen.
*/
void drawText(string text) {
	glMatrixMode(GL_PROJECTION); // change the current matrix to PROJECTION
	double matrix[16]; // 16 doubles in stack memory
	glGetDoublev(GL_PROJECTION_MATRIX, matrix); // get the values from PROJECTION matrix to local variable
	glLoadIdentity(); // reset PROJECTION matrix to identity matrix
	glOrtho(0, 800, 0, 600, -5, 5); // orthographic perspective
	glMatrixMode(GL_MODELVIEW); // change current matrix to MODELVIEW matrix again
	glLoadIdentity(); // reset it to identity matrix
	glPushMatrix(); // push current state of MODELVIEW matrix to stack
	glLoadIdentity(); // reset it again. (may not be required, but it my convention)
	glRasterPos2i(0, height - 20); // raster position in 2D
	for (int i = 0; i<text.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)text[i]); // generation of characters in our text with 9 by 15 GLU font
	}
	glPopMatrix(); // get MODELVIEW matrix value from stack
	glMatrixMode(GL_PROJECTION); // change current matrix mode to PROJECTION
	glLoadMatrixd(matrix); // reset
	glMatrixMode(GL_MODELVIEW); // change current matrix mode to MODELVIEW
}

std::vector<vector<Vertex>> cubes;

void drawCube(int index)
{
	if (index == -1)
		return;

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
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), ((float*)cubes[index].data()) + 0);
	glNormalPointer(GL_FLOAT, sizeof(Vertex), ((float*)cubes[index].data()) + 3);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), ((float*)cubes[index].data()) + 10);
	glColorPointer(4, GL_FLOAT, sizeof(Vertex), ((float*)cubes[index].data()) + 6);
	glDrawArrays(GL_QUADS, 0, cubes[index].size());

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
}

void drawMenu() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 1, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(0, 0);
	glTexCoord2f(1, 0); glVertex2f(1, 0);
	glTexCoord2f(1, 1); glVertex2f(1, 1);
	glTexCoord2f(0, 1); glVertex2f(0, 1);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

int menuItem = 1;

void display()
{
	ShowCursor(false);

	glClearColor(0.9f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, width / (float)height, 0.1f, 50.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float x, y, z, length;

	length = cos(camera.rotX * PI / 180);
	z = cos(camera.rotY * PI / 180) * length;
	x = sin(camera.rotY * PI / 180) * length;
	y = sin(camera.rotX * PI / 180);

	gluLookAt(player->position.x, player->position.y, player->position.z,
		player->position.x + x, player->position.y - y, player->position.z - z,
		0, 1, 0);

	if (showMenu) {
		glBindTexture(GL_TEXTURE_2D, textures[menuItem]);
		drawMenu();
		if (keys['w'])
			menuItem = 1;
		if (keys['s'])
			menuItem = 2;
		if (keys[13]) {
			if (menuItem == 1) {
				showMenu = false;
				PlaySound(NULL, NULL, NULL);
			}
			else
				exit(0);
		}
	}
	else {
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		for (auto &o : objects)
			o->draw();
		if (keys[27])
			showMenu = true;
	}

	glColor3f(1, 0, 0);
	drawText(std::to_string((int)(1/(double)deltaTime)));

	glutSwapBuffers();
}

void idle()
{
	float frameTime = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	deltaTime = frameTime - lastFrameTime;
	lastFrameTime = frameTime;

	for (auto &o : objects)
		o->update(deltaTime, camera.rotX, camera.rotY);

	glutPostRedisplay();
}

bool justMovedMouse = false;
void mousePassiveMotion(int x, int y)
{
	if (!showMenu) {
		int dx = x - width / 2;
		int dy = y - height / 2;
		if ((dx != 0 || dy != 0) && abs(dx) < 400 && abs(dy) < 400 && !justMovedMouse)
		{
			camera.rotY += dx / 10.0f;
			camera.rotX += dy / 10.0f;

			if (camera.rotX > 90)
				camera.rotX = 90;
			else if (camera.rotX < -90)
				camera.rotX = -90;
		}
		if (!justMovedMouse)
		{
			glutWarpPointer(width / 2, height / 2);
			justMovedMouse = true;
		}
		else
			justMovedMouse = false;
	}
}

void keyboard(unsigned char key, int, int)
{
	//cout << to_string(key) << endl;
	keys[key] = true;
}

void keyboardUp(unsigned char key, int,int)
{
	keys[key] = false;
}

int main(int argc, char* argv[])
{
	int maxHeight = 10;

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInit(&argc, argv);
	glutCreateWindow("3D-Graphics Eindopdracht");

	memset(keys, 0, sizeof(keys));
	glEnable(GL_DEPTH_TEST);

	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc([](int w, int h) { width = w; height = h; glViewport(0, 0, w, h); });
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutPassiveMotionFunc(mousePassiveMotion);

	glutWarpPointer(width / 2, height / 2);
	
	glGenTextures(3, textures);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	stbi_set_flip_vertically_on_load(true);

	int bpp;
	stbi_uc* data = stbi_load("terrain.png", &width, &height, &bpp, 4);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(false);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	data = stbi_load("menu_1.jpg", &width, &height, &bpp, 4);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	data = stbi_load("menu_2.jpg", &width, &height, &bpp, 4);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int terrainWidth = 16;
	int terrainHeight = 16;

	for (size_t index = 0; index < terrainWidth*terrainHeight; index++)
	{
		std::vector<Vertex> cubeVertices;
		float originWidth, originHeight, blockWidth, blockHeight;
		originWidth = 0.0625 * (index % 16);
		originHeight = 0.9375 - (index / 16) * 0.0625;
		blockWidth = originWidth + 0.0625;
		blockHeight = originHeight + 0.0625;

		cubeVertices.push_back(Vertex{ -1, -1, -1, 0,1,0, 1,1,1,1 , originWidth, originHeight });
		cubeVertices.push_back(Vertex{ -1,  1, -1, 0,1,0, 1,1,1,1 , originWidth, blockHeight });
		cubeVertices.push_back(Vertex{ 1,  1, -1, 0,1,0, 1,1,1,1 , blockWidth, blockHeight });
		cubeVertices.push_back(Vertex{ 1, -1, -1, 0,1,0, 1,1,1,1 , blockWidth, originHeight });

		cubeVertices.push_back(Vertex{ -1, -1,  1, 0,1,0, 1,1,1,1 ,  blockWidth, originHeight });
		cubeVertices.push_back(Vertex{ -1,  1,  1, 0,1,0, 1,1,1,1 ,  blockWidth,  blockHeight });
		cubeVertices.push_back(Vertex{ 1,  1,  1, 0,1,0, 1,1,1,1,	originWidth, blockHeight });
		cubeVertices.push_back(Vertex{ 1, -1,  1, 0,1,0, 1,1,1,1, originWidth, originHeight });

		cubeVertices.push_back(Vertex{ -1,  -1, -1,		0,1,0,		1,1,1,1, originWidth, originHeight });
		cubeVertices.push_back(Vertex{ -1,  -1,  1,		0,1,0,		1,1,1,1, originWidth, blockHeight });
		cubeVertices.push_back(Vertex{ 1,  -1,  1,		0,1,0,		1,1,1,1, blockWidth, blockHeight });
		cubeVertices.push_back(Vertex{ 1,  -1, -1,		0,1,0,		1,1,1,1, blockWidth, originHeight });

		cubeVertices.push_back(Vertex{ -1,  1, -1,		0,1,0,		1,1,1,1, originWidth, originHeight });
		cubeVertices.push_back(Vertex{ -1,  1,  1,		0,1,0,		1,1,1,1, originWidth, blockHeight });
		cubeVertices.push_back(Vertex{ 1,  1,  1,		0,1,0,		1,1,1,1, blockWidth, blockHeight });
		cubeVertices.push_back(Vertex{ 1,  1, -1,		0,1,0,		1,1,1,1, blockWidth, originHeight });

		cubeVertices.push_back(Vertex{ -1, -1, -1,		0,1,0,		1,1,1,1, blockWidth, originHeight });
		cubeVertices.push_back(Vertex{ -1, -1,  1,		0,1,0,		1,1,1,1, originWidth, originHeight });
		cubeVertices.push_back(Vertex{ -1,  1,  1,		0,1,0,		1,1,1,1, originWidth, blockHeight });
		cubeVertices.push_back(Vertex{ -1,  1, -1,		0,1,0,		1,1,1,1, blockWidth, blockHeight });

		cubeVertices.push_back(Vertex{ 1, -1, -1,		0,1,0,		1,1,1,1, originWidth, originHeight });
		cubeVertices.push_back(Vertex{ 1, -1,  1,		0,1,0,		1,1,1,1, blockWidth, originHeight });
		cubeVertices.push_back(Vertex{ 1,  1,  1,		0,1,0,		1,1,1,1, blockWidth, blockHeight });
		cubeVertices.push_back(Vertex{ 1,  1, -1,		0,1,0,		1,1,1,1, originWidth, blockHeight });
		cubes.push_back(cubeVertices);
	}

	data = stbi_load("heightmap.png", &width, &height, &bpp, 1);

	for (int x = 10; x < 50; x++) {
		for (int y = 10; y < 50; y++) {
			int heightData = (((float)data[(x * 128) + y]) / 256) * maxHeight;
			//cout << heightData << endl;
			for (int z = 0; z < maxHeight; z++) {
				if (z == heightData) {
					GameObject* block = new GameObject();
					block->addComponent(new CubeComponent(2, cubes[0], textures[0]));
					block->position = Vec3f((float)x * 2 - 20, (float)z * 2, (float)y * 2 - 20);
					objects.push_back(block);
				}
				else {
				}
			}
		}
	}

	stbi_image_free(data);

	GameObject* o = new GameObject();
	o->addComponent(new PlayerComponent());
	o->position = Vec3f(0, 10, 0);
	objects.push_back(o);

	player = o;

	glutFullScreen();
	PlaySound("menu_music.wav", NULL, SND_LOOP | SND_ASYNC);

	glutMainLoop();

	return 0;
}