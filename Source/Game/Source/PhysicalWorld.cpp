#include "GameCore.h"
#include "PhysicalWorld.h"
#include <btBulletCollisionCommon.h>

void PhysicalWorld::Init()
{
	config = new btDefaultCollisionConfiguration;
	dispatcher = new btCollisionDispatcher(phy_config);
	broadphase = new btDbvtBroadphase;
	world = new btCollisionWorld(phy_dispatcher, phy_broadphase, phy_config);
}
