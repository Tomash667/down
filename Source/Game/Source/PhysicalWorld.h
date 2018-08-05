#pragma once

class PhysicalWorld
{
public:
	PhysicalWorld();
	~PhysicalWorld();
	void Init();
	void Draw(DebugDrawer* debug_drawer);
	//
	void AddFloor();
	void AddPlayer();
	void AddBox(const Vec3& pos);
	void AddLevel(Mesh* mesh);
	void UpdatePlayerPos(Vec3& pos);

private:
	void DrawCollisionObjects(DebugDrawer* debug_drawer, int alpha);

	btDefaultCollisionConfiguration* config;
	btCollisionDispatcher* dispatcher;
	btDbvtBroadphase* broadphase;
	btCollisionWorld* world;
	vector<btCollisionShape*> shapes;
	btCollisionShape* box_shape;

	btCollisionObject* player_cobj;
	vector<btCollisionObject*> active_colliders;
};
