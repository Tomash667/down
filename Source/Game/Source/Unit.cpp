#include "GameCore.h"
#include "Unit.h"

void Unit::Save(FileWriter& f)
{

}

void Unit::Load(FileReader& f)
{

}

static const bool is_enemy[UNIT_TYPE_MAX][UNIT_TYPE_MAX] = {
//	player	enemy	npc
	{false,	true,	false}, // player
	{true,	false,	false}, // enemy
	{false,	false,	false} // npc
};

bool Unit::IsEnemy(UnitType other_type) const
{
	// FIXME - verify
	return is_enemy[type][other_type];
}
