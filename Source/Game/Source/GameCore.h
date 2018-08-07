#pragma once

#include "EngineCore.h"

class Game;
class Level;
class PhysicalWorld;

// gui
class MainMenu;

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
class btCollisionObject;
struct btDbvtBroadphase;
