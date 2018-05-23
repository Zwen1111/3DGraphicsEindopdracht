#include "PlayerComponent.h"
#include "GameObject.h"

extern bool keys[256];



PlayerComponent::PlayerComponent()
{
}

PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::update(float elapsedTime)
{
	if (keys['w'])
		gameObject->position.z -= elapsedTime * speed;
	if (keys['s'])
		gameObject->position.z += elapsedTime * speed;
	if (keys['a'])
		gameObject->position.x -= elapsedTime * speed;
	if (keys['d'])
		gameObject->position.x += elapsedTime * speed;
	if (keys['e'])
		gameObject->position.y += elapsedTime * speed;

}
