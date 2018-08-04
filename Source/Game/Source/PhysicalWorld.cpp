#include "GameCore.h"
#include "PhysicalWorld.h"
#include <DebugDrawer.h>
#pragma warning(push, 0)
#include <btBulletCollisionCommon.h>
#pragma warning(pop)


#ifdef _DEBUG
#	define assert_once(x) { static bool _once = true; if(_once && !(x)) { _once = false; assert(x); } }
#else
#	define assert_once(x)
#endif


inline Vec3 ToVec3(const btVector3& v)
{
	return Vec3(v.getX(), v.getY(), v.getZ());
}


PhysicalWorld::PhysicalWorld() : config(nullptr), dispatcher(nullptr), broadphase(nullptr), world(nullptr)
{
}

PhysicalWorld::~PhysicalWorld()
{
	for(btCollisionShape* shape : shapes)
		delete shape;
	btOverlappingPairCache* cache = broadphase->getOverlappingPairCache();
	auto& cobjs = world->getCollisionObjectArray();
	for(int i = 0, count = cobjs.size(); i < count; ++i)
	{
		btCollisionObject* cobj = cobjs[i];
		btBroadphaseProxy* bp = cobj->getBroadphaseHandle();
		if(bp)
		{
			cache->cleanProxyFromPairs(bp, dispatcher);
			broadphase->destroyProxy(bp, dispatcher);
		}
		delete cobj;
	}
	cobjs.clear();
	broadphase->resetPool(dispatcher);
	delete world;
	delete broadphase;
	delete dispatcher;
	delete config;
}

void PhysicalWorld::Init()
{
	config = new btDefaultCollisionConfiguration;
	dispatcher = new btCollisionDispatcher(config);
	broadphase = new btDbvtBroadphase;
	world = new btCollisionWorld(dispatcher, broadphase, config);
}

void PhysicalWorld::Draw(DebugDrawer* debug_drawer)
{
	// TODO: list collision objects visible from frustum
	debug_drawer->SetColor(Color(163, 73, 164, 64));
	DrawCollisionObjects(debug_drawer);
	debug_drawer->SetColor(Color(163, 73, 164));
	debug_drawer->SetWireframe(true);
	DrawCollisionObjects(debug_drawer);
}

void PhysicalWorld::DrawCollisionObjects(DebugDrawer* debug_drawer)
{
	const auto& cobjs = world->getCollisionObjectArray();

	for(int i = 0, count = cobjs.size(); i < count; ++i)
	{
		const btCollisionObject* cobj = cobjs[i];
		const btCollisionShape* shape = cobj->getCollisionShape();
		Matrix m;
		cobj->getWorldTransform().getOpenGLMatrix(&m._11);
		switch(shape->getShapeType())
		{
		case BOX_SHAPE_PROXYTYPE:
			{
				const btBoxShape* box = (const btBoxShape*)shape;
				debug_drawer->DrawCube(Matrix::Scale(ToVec3(box->getHalfExtentsWithMargin())) * m);
			}
			break;
		default:
			assert_once(0);
			break;
		}
	}
}

void PhysicalWorld::AddBoxCollider()
{
	const float margin = 0.01f;
	{
		btBoxShape* shape = new btBoxShape(btVector3(10.f + margin, margin, 10.f + margin));
		btCollisionObject* cobj = new btCollisionObject;
		cobj->setCollisionShape(shape);
		world->addCollisionObject(cobj);
		shapes.push_back(shape);
	}
	{
		const float h = 2.871f;
		btBoxShape* shape = new btBoxShape(btVector3(1.f + margin, h / 2 + margin, 1.f + margin));
		btCollisionObject* cobj = new btCollisionObject;
		cobj->setCollisionShape(shape);
		cobj->getWorldTransform().setOrigin(btVector3(3.f, h / 2, 0));
		world->addCollisionObject(cobj);
		shapes.push_back(shape);
	}
}
