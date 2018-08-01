#pragma once

class PhysicalWorld
{
public:
	void Init();

private:
	btDefaultCollisionConfiguration* config;
	btCollisionDispatcher* dispatcher;
	btDbvtBroadphase* broadphase;
	btCollisionWorld* world;
};
