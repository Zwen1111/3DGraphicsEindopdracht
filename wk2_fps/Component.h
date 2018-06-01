#pragma once

#include <list>

class GameObject;


class Component
{
protected:
	GameObject* gameObject;
public:
	Component();
	~Component();

	virtual void update(float elapsedTime, float rotX, float rotY, std::list<GameObject*>& objects) {};

	inline void setGameObject(GameObject* gameObject) { this->gameObject = gameObject; }
};

