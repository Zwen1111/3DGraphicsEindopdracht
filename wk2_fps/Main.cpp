#include <GL/freeglut.h>
#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <string>
#include "GameObject.h"
#include "CubeComponent.h"
#include "PlayerComponent.h"
#include <math.h>
#include <Windows.h>
#include <Mmsystem.h>
#include "ModelComponent.h"

using namespace std;

#define PI 3.14159265

float lastFrameTime = 0; 
float deltaTime = 0;

int width, height;
const int maxHeight = 10;
int mapData[128][128];

bool showMenu = true;

GLuint textures[3];
int heightmap[128][128][10];

std::list<GameObject*> objects;
GameObject* player;
GameObject* car;

std::vector<vector<Vertex>> cubes;

int imageWidth, imageHeight;

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
void drawText(string text, int x, int y) {
	glMatrixMode(GL_PROJECTION); // change the current matrix to PROJECTION
	double matrix[16]; // 16 doubles in stack memory
	glGetDoublev(GL_PROJECTION_MATRIX, matrix); // get the values from PROJECTION matrix to local variable
	glLoadIdentity(); // reset PROJECTION matrix to identity matrix
	glOrtho(0, width, 0, height, -5, 5); // orthographic perspective
	glMatrixMode(GL_MODELVIEW); // change current matrix to MODELVIEW matrix again
	glLoadIdentity(); // reset it to identity matrix
	glPushMatrix(); // push current state of MODELVIEW matrix to stack
	glLoadIdentity(); // reset it again. (may not be required, but it my convention)
	glRasterPos2i(x, height - y); // raster position in 2D
	for (int i = 0; i<text.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)text[i]); // generation of characters in our text with 9 by 15 GLU font
	}
	glPopMatrix(); // get MODELVIEW matrix value from stack
	glMatrixMode(GL_PROJECTION); // change current matrix mode to PROJECTION
	glLoadMatrixd(matrix); // reset
	glMatrixMode(GL_MODELVIEW); // change current matrix mode to MODELVIEW
}

void drawMenu() {
	glDisable(GL_LIGHTING);

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

	glEnable(GL_LIGHTING);
}

int menuItem = 1;
GLfloat qaLightPosition[] = { 0, 6, 0, 1 };

void display()
{
	//ShowCursor(false);

	glClearColor(0.9f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, width / (float)height, 0.1f, 45.0f);

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

	glPushMatrix();
	glTranslatef(qaLightPosition[0], qaLightPosition[1], qaLightPosition[2]);
	glPopMatrix();

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
		for (auto &o : objects) {
			if(o->position.x > player->position.x - 80.f && o->position.x < player->position.x + 80.f && o->position.z > player->position.z - 80.f && o->position.z < player->position.z + 80.f)
				o->draw();
		}
		if (keys[27])
			showMenu = true;
	}

	glColor3f(1, 0, 0);
	drawText(to_string((int)(1/(double)deltaTime)), 0, 20);

	glColor3f(1, 1, 1);
	drawText(to_string(int(player->position.x + 1) / 2), 0, 40);
	drawText(to_string(player->position.y), 0, 60);
	drawText(to_string(int(player->position.z + 1) / 2), 0, 80);

	/*std::list<ModelComponent*> models;

	for (auto object : objects)
	{
		for (auto component : object->getComponents()) {
			if (ModelComponent* m = dynamic_cast<ModelComponent*>(component)) {
				models.push_back(m);
			}
		}
	}

	drawText(to_string(models.front()->lowestX * 0.1f + 20), 0, 120);
	drawText(to_string(models.front()->lowestY * 0.1f + 10), 0, 140);
	drawText(to_string(models.front()->lowestZ * 0.1f + 20), 0, 160);

	drawText(to_string(models.front()->highestX * 0.1f + 20), 0, 200);
	drawText(to_string(models.front()->highestY * 0.1f + 10), 0, 220);
	drawText(to_string(models.front()->lowestZ * 0.1f + 20), 0, 240);*/

	//player->position = Vec3f(player->position.x, mapData[(int(player->position.x + 1) / 2)][(int(player->position.z + 1) / 2)] + 4, player->position.z);

	glutSwapBuffers();
}

float rotation = 0;

void idle()
{
	float frameTime = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	deltaTime = frameTime - lastFrameTime;
	lastFrameTime = frameTime;

	/*Vec3f oldPos = car->position;
	car->position.x += 1;
	car->position.z *= 1.1f;
	Vec3f newPos = car->position;

	float angle = (atan2(newPos.x - oldPos.x, newPos.z - oldPos.z) * 180) / PI + 90;
	car->rotation.y = angle;

	if (newPos.x > imageWidth * 2 || newPos.z == 0) {
		car->position.x = 0;
		car->position.z = 1;
	}*/

	for (auto component : car->getComponents()) {
		if (ModelComponent* m = dynamic_cast<ModelComponent*>(component)) {
			if (!m->collision()) {
				rotation += 0.001f;

				Vec3f oldPos = car->position;
				car->position.x = cos(rotation) * 28 + 32;
				car->position.z = sin(rotation) * 28 + 32;
				Vec3f newPos = car->position;

				float angle = (atan2(newPos.x - oldPos.x, newPos.z - oldPos.z) * 180) / PI + 90;
				car->rotation.y = angle;
			}
		}
	}
	

	for (auto &o : objects) {
		if (o->position.x > player->position.x - 80.f && o->position.x < player->position.x + 80.f && o->position.z > player->position.z - 80.f && o->position.z < player->position.z + 80.f)
			o->update(deltaTime, camera.rotX, camera.rotY, objects);
	}

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

			if (camera.rotX > 89)
				camera.rotX = 89;
			else if (camera.rotX < -89)
				camera.rotX = -89;
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

	glShadeModel(GL_SMOOTH);

	GLfloat qaAmbientLight[] = { 0,0,0,1 };
	GLfloat qaDiffuseLight[] = { 1,1,1,1 };
	GLfloat qaSpecularLight[] = { 1,1,1,1 };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);
	glPopMatrix();


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

	data = stbi_load("heightmap2.png", &imageWidth, &imageHeight, &bpp, 1);

	for (int x = 0; x < imageWidth; x++) {
		for (int y = 0; y < imageHeight; y++) {
			int heightData = (((float)data[(x * imageWidth) + y]) / 255) * maxHeight;
			//cout << heightData << endl;
			for (int z = 0; z < maxHeight; z++) {
				if (z == heightData) {
					GameObject* block = new GameObject();
					block->addComponent(new CubeComponent(2, cubes[0], textures[0]));
					block->position = Vec3f((float)x * 2, (float)z * 2, (float)y * 2);
					objects.push_back(block);

					int xx = (int(((float)x * 2) + 1) / 2);
					int yy = (float)z * 2;
					int zz = (int(((float)y * 2) + 1) / 2);


					mapData[xx][zz] = yy;
				}
				else {
				}
			}
		}
	}

	stbi_image_free(data);

	GameObject* o = new GameObject();
	o->addComponent(new PlayerComponent(mapData));
	o->position = Vec3f(0, 8, 0);
	objects.push_back(o);

	player = o;

	car = new GameObject();
	car->position = Vec3f(20, 4, 20);
	car->addComponent(new ModelComponent("models/car/honda_jazz.obj", o));
	car->scale = Vec3f(0.1f, 0.1f, 0.1f);
	objects.push_back(car);

	//glutFullScreen();
	PlaySound("menu_music.wav", NULL, SND_LOOP | SND_ASYNC);

	glutMainLoop();

	return 0;
}