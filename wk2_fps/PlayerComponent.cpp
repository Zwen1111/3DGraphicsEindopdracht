#include "PlayerComponent.h"
#include "GameObject.h"
#include <math.h>
#include <iostream>

extern bool keys[256];

#define PI 3.14159265

using namespace std;

PlayerComponent::PlayerComponent()
{
}

PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::update(float elapsedTime, float rotX, float rotY)
{
	if (keys['w']) {
		gameObject->position.z -= elapsedTime * speed * cos(rotY * PI / 180);
		gameObject->position.x += elapsedTime * speed * sin(rotY * PI / 180);
	}
	if (keys['s']) {
		gameObject->position.z += elapsedTime * speed * cos(rotY * PI / 180);
		gameObject->position.x -= elapsedTime * speed * sin(rotY * PI / 180);
	}
	if (keys['a']) {
		gameObject->position.z -= elapsedTime * speed * cos((rotY - 90) * PI / 180);
		gameObject->position.x += elapsedTime * speed * sin((rotY - 90) * PI / 180);
	}
	if (keys['d']) {
		gameObject->position.z -= elapsedTime * speed * cos((rotY+90) * PI / 180);
		gameObject->position.x += elapsedTime * speed * sin((rotY+90) * PI / 180);
	}
	if (keys['q'])
		gameObject->position.y -= elapsedTime * speed;
	if (keys['e'])
		gameObject->position.y += elapsedTime * speed;

}
