#include "PlayerComponent.h"
#include "GameObject.h"
#include <math.h>
#include <iostream>
#include "ModelComponent.h"

extern bool keys[256];

#define PI 3.14159265

using namespace std;

PlayerComponent::PlayerComponent(int mapData[128][128])
{
	for (size_t x = 0; x < 128; x++)
	{
		for (size_t y = 0; y < 128; y++)
		{
			this->mapData[x][y] = mapData[x][y];
		}
	}
}

PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::update(float elapsedTime, float rotX, float rotY, std::list<GameObject*>& objects)
{
	std::list<ModelComponent*> models;

	for (auto object : objects)
	{
		for (auto component : object->getComponents()) {
			if (ModelComponent* m = dynamic_cast<ModelComponent*>(component)) {
				models.push_back(m);
			}
		}
	}

	if (keys['w'] || keys['W']) {
		float tempX = gameObject->position.x + elapsedTime * speed * sin(rotY * PI / 180);
		float tempZ = gameObject->position.z - elapsedTime * speed * cos(rotY * PI / 180);

		bool collided = false;

		for (auto model : models) {
			if (model->collision(Vec3f(tempX + 0.5f, (gameObject->position.y), tempZ + 0.5f))) {
				collided = true;
				break;
			}
		}

		if (mapData[(int(tempX + 1) / 2)][(int(tempZ + 1) / 2)] < (int)(gameObject->position.y - 4.0f) && !collided) {
			gameObject->position.z = tempZ;
			gameObject->position.x = tempX;
		}
	}
	if (keys['s'] || keys['S']) {
		float tempX = gameObject->position.x + elapsedTime * speed * sin((rotY + 180) * PI / 180);
		float tempZ = gameObject->position.z - elapsedTime * speed * cos((rotY + 180) * PI / 180);

		bool collided = false;

		for (auto model : models) {
			if (model->collision(Vec3f(tempX + 0.5f, (gameObject->position.y), tempZ + 0.5f))) {
				collided = true;
				break;
			}
		}

		if (mapData[(int(tempX - 1) / 2)][(int(tempZ - 1) / 2)] < (int)(gameObject->position.y - 4.0f) && !collided) {
			gameObject->position.z = tempZ;
			gameObject->position.x = tempX;
		}
	}
	if (keys['a'] || keys['A']) {
		float tempX = gameObject->position.x + elapsedTime * speed * sin((rotY - 90) * PI / 180);
		float tempZ = gameObject->position.z - elapsedTime * speed * cos((rotY - 90) * PI / 180);

		bool collided = false;

		for (auto model : models) {
			if (model->collision(Vec3f(tempX + 0.5f, (gameObject->position.y), tempZ + 0.5f))) {
				collided = true;
				break;
			}
		}

		if (mapData[(int(tempX + 1) / 2)][(int(tempZ + 1) / 2)] < (int)(gameObject->position.y - 4.0f) && !collided) {
			gameObject->position.z = tempZ;
			gameObject->position.x = tempX;
		}
	}
	if (keys['d'] || keys['D']) {
		float tempX = gameObject->position.x + elapsedTime * speed * sin((rotY + 90) * PI / 180);
		float tempZ = gameObject->position.z - elapsedTime * speed * cos((rotY + 90) * PI / 180);

		bool collided = false;

		for (auto model : models) {
			if (model->collision(Vec3f(tempX + 0.5f, (gameObject->position.y), tempZ + 0.5f))) {
				collided = true;
				break;
			}
		}

		if (mapData[(int(tempX + 1) / 2)][(int(tempZ + 1) / 2)] < (int)(gameObject->position.y - 4.0f) && !collided) {
			gameObject->position.z = tempZ;
			gameObject->position.x = tempX;
		}
	}
	if (keys['q']) {
		float tempY = gameObject->position.y - elapsedTime * speed;
		if(mapData[(int(gameObject->position.x + 1) / 2)][(int(gameObject->position.z + 1) / 2)] < tempY - 5.0f)
			gameObject->position.y -= elapsedTime * speed;
	}
	if (keys[32]) {
		jump();
	}
	if (keys['e'])
		gameObject->position.y += elapsedTime * speed;
	if (jumping) {
		gameObject->position.y += (jumpValue / gameObject->position.y);
		jumpValue = jumpValue / gameObject->position.y;
	}
}
