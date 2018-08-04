#pragma once

class PhysicalWorld
{
public:
	PhysicalWorld();
	~PhysicalWorld();
	void Init();
	void Draw(DebugDrawer* debug_drawer);
	void AddBoxCollider();

private:
	void DrawCollisionObjects(DebugDrawer* debug_drawer);

	btDefaultCollisionConfiguration* config;
	btCollisionDispatcher* dispatcher;
	btDbvtBroadphase* broadphase;
	btCollisionWorld* world;
	vector<btCollisionShape*> shapes;
};
