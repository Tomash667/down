#pragma once

#include "EngineCore.h"

class Game;
class Level;
class PhysicalWorld;

struct Chest;
struct Enemy;
struct Npc;
struct Player;
struct Unit;

// bullet physics
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btCollisionWorld;
class btCollisionShape;
struct btDbvtBroadphase;
