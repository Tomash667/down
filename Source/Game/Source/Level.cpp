#include "GameCore.h"
#include "Level.h"
#include "Unit.h"

void Level::Update(float dt)
{
	for(Unit* unit : units)
		unit->Update(dt);
}

void Level::Save(FileWriter& f)
{

}

void Level::Load(FileReader& f)
{

}
