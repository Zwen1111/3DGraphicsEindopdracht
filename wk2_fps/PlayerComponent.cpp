#include "PlayerComponent.h"
#include "GameObject.h"
#include <math.h>

extern bool keys[256];

using namespace std;

PlayerComponent::PlayerComponent()
{
}

PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::update(float elapsedTime, float rotX, float rotY)
{
	if (keys['w'])
		gameObject->position.z -= elapsedTime * speed;
	if (keys['s'])
		gameObject->position.z += elapsedTime * speed;
	if (keys['a'])
		gameObject->position.x -= elapsedTime * speed;
	if (keys['d'])
		gameObject->position.x += elapsedTime * speed;
	if (keys['q'])
		gameObject->position.y -= elapsedTime * speed;
	if (keys['e'])
		gameObject->position.y += elapsedTime * speed;

}
