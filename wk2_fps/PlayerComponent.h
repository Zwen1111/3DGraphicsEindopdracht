#pragma once

#include "Component.h"
#include <list>

class PlayerComponent : public Component
{
	float speed = 10;
public:
	void jump() {
		jumping = true;
		jumpValue = 1.1f;
	}

	PlayerComponent();
	~PlayerComponent();

	GameObject* selectedBlock;

	void setMapData(int mapData[128][128]);
	virtual void update(float elapsedTime, float rotX, float rotY, std::list<GameObject*>& objects) override;
private:
	int mapData[128][128];
	bool jumping = false;
	float jumpValue;
};

