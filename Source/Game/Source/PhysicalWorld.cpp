#include "GameCore.h"
#include "PhysicalWorld.h"
#include <DebugDrawer.h>
#include <Mesh.h>
#pragma warning(push, 0)
#include <btBulletCollisionCommon.h>
#pragma warning(pop)
// FIXME
#include "Player.h"


#ifdef _DEBUG
#	define assert_once(x) { static bool _once = true; if(_once && !(x)) { _once = false; assert(x); } }
#else
#	define assert_once(x)
#endif

const float margin = 0.01f;
const float player_margin = 0.05f;


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
	DrawCollisionObjects(debug_drawer, 64);
	debug_drawer->SetWireframe(true);
	DrawCollisionObjects(debug_drawer, 255);
}

void PhysicalWorld::DrawCollisionObjects(DebugDrawer* debug_drawer, int alpha)
{
	const auto& cobjs = world->getCollisionObjectArray();

	for(int i = 0, count = cobjs.size(); i < count; ++i)
	{
		const btCollisionObject* cobj = cobjs[i];
		const btCollisionShape* shape = cobj->getCollisionShape();

		bool is_active = false;
		for(btCollisionObject* active_cobj : active_colliders)
		{
			if(active_cobj == cobj)
			{
				is_active = true;
				break;
			}
		}
		debug_drawer->SetColor(is_active ? Color(0, 255, 100, alpha) : Color(163, 73, 164, alpha));

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
		case CAPSULE_SHAPE_PROXYTYPE:
			{
				const btCapsuleShape* capsule = (const btCapsuleShape*)shape;
				float radius = capsule->getRadius();
				debug_drawer->DrawCapsule(Matrix::Scale(Vec3(radius, capsule->getHalfHeight() + radius, radius)) * m);
			}
			break;
		case TRIANGLE_MESH_SHAPE_PROXYTYPE:
			{
				Mesh* mesh = (Mesh*)shape->getUserPointer();
				debug_drawer->DrawMesh(mesh, m);
			}
			break;
		default:
			assert_once(0);
			break;
		}
	}
}

void PhysicalWorld::AddFloor()
{
	btBoxShape* shape = new btBoxShape(btVector3(10.f + margin, margin, 10.f + margin));
	btCollisionObject* cobj = new btCollisionObject;
	cobj->setCollisionShape(shape);
	world->addCollisionObject(cobj);
	shapes.push_back(shape);
}

void PhysicalWorld::AddBox(const Vec3& pos)
{
	const float h = 2.871f;
	if(!box_shape)
	{
		box_shape = new btBoxShape(btVector3(1.f + margin, h / 2 + margin, 1.f + margin));
		shapes.push_back(box_shape);
	}
	btCollisionObject* cobj = new btCollisionObject;
	cobj->setCollisionShape(box_shape);
	cobj->getWorldTransform().setOrigin(btVector3(pos.x, pos.y + h / 2, pos.z));
	world->addCollisionObject(cobj);
}

void PhysicalWorld::AddPlayer()
{
	btCapsuleShape* shape = new btCapsuleShape(Player::radius, Player::height - Player::radius * 2);
	btCollisionObject* cobj = new btCollisionObject;
	cobj->setCollisionShape(shape);
	cobj->getWorldTransform().setOrigin(btVector3(0, Player::height / 2 + player_margin, 0));
	world->addCollisionObject(cobj);
	shapes.push_back(shape);
	player_cobj = cobj;
}

void PhysicalWorld::AddLevel(Mesh* mesh)
{
	assert(mesh && !mesh->vertex_data.empty());

	btTriangleIndexVertexArray* tri_array = new btTriangleIndexVertexArray;

	btIndexedMesh indexed_mesh;
	indexed_mesh.m_numTriangles = mesh->head.n_tris;
	indexed_mesh.m_triangleIndexBase = (byte*)mesh->index_data.data();
	indexed_mesh.m_triangleIndexStride = sizeof(word) * 3;
	indexed_mesh.m_numVertices = mesh->head.n_verts;
	indexed_mesh.m_vertexBase = mesh->vertex_data.data();
	indexed_mesh.m_vertexStride = sizeof(Vec3);

	tri_array->addIndexedMesh(indexed_mesh, PHY_SHORT);

	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(tri_array, true);
	shape->setUserPointer(mesh);
	shapes.push_back(shape);
	
	btCollisionObject* cobj = new btCollisionObject;
	cobj->setCollisionShape(shape);
	world->addCollisionObject(cobj);
}

struct ContactTestCallback : btCollisionWorld::ContactResultCallback
{
	ContactTestCallback(btCollisionObject* me) : me(me) {}

	btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
		const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
	{
		btCollisionObject* cobj1 = (btCollisionObject*)colObj0Wrap->getCollisionObject();
		btCollisionObject* cobj2 = (btCollisionObject*)colObj1Wrap->getCollisionObject();
		contacted.push_back(cobj1 == me ? cobj2 : cobj1);
		return 1.f;
	}

	btCollisionObject* me;
	vector<btCollisionObject*> contacted;
};

struct ClosestConvexNotMeCallback : btCollisionWorld::ConvexResultCallback
{
	ClosestConvexNotMeCallback(btCollisionObject* me) : me(me), m_hitCollisionObject(nullptr) {}

	btVector3 m_hitNormalWorld;
	btVector3 m_hitPointWorld;
	const btCollisionObject* m_hitCollisionObject;
	btCollisionObject* me;

	virtual	btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
	{
		//caller already does the filter on the m_closestHitFraction
		btAssert(convexResult.m_hitFraction <= m_closestHitFraction);

		if(convexResult.m_hitCollisionObject == me)
			return 1.f;

		m_closestHitFraction = convexResult.m_hitFraction;
		m_hitCollisionObject = convexResult.m_hitCollisionObject;
		if(normalInWorldSpace)
		{
			m_hitNormalWorld = convexResult.m_hitNormalLocal;
		}
		else
		{
			//need to transform normal into worldspace
			m_hitNormalWorld = m_hitCollisionObject->getWorldTransform().getBasis()*convexResult.m_hitNormalLocal;
		}
		m_hitPointWorld = convexResult.m_hitPointLocal;
		return convexResult.m_hitFraction;
	}
};

void PhysicalWorld::UpdatePlayerPos(Vec3& pos)
{
	btVector3 old_pos = player_cobj->getWorldTransform().getOrigin();
	btVector3 new_pos(pos.x, pos.y + Player::height / 2 + player_margin, pos.z);
	btTransform target(btQuaternion::getIdentity(), new_pos);
	ClosestConvexNotMeCallback callback(player_cobj);
	world->convexSweepTest((btConvexShape*)player_cobj->getCollisionShape(), player_cobj->getWorldTransform(), target, callback);
	if(!callback.hasHit())
	{
		// no collision, move to pos
		player_cobj->getWorldTransform().setOrigin(new_pos);
		return;
	}

	// try to slide along wall
	btVector3 dir = new_pos - old_pos;
	float length = dir.length();
	dir.normalize();
	btVector3 normal = callback.m_hitNormalWorld;
	btVector3 new_dir = dir - (normal * dir.dot(normal));
	new_pos = old_pos + new_dir * length;
	target.setOrigin(new_pos);
	ClosestConvexNotMeCallback callback2(player_cobj);
	world->convexSweepTest((btConvexShape*)player_cobj->getCollisionShape(), player_cobj->getWorldTransform(), target, callback2);
	if(!callback2.hasHit())
	{
		player_cobj->getWorldTransform().setOrigin(new_pos);
		pos = Vec3(new_pos.getX(), new_pos.getY() - Player::height / 2 - player_margin, new_pos.getZ());
	}
	
	//ContactTestCallback callback(player_cobj);
	//world->contactTest(player_cobj, callback);
	//active_colliders = callback.contacted;
}
