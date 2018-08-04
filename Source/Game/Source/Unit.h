#pragma once

enum UnitType
{
	UNIT_PLAYER,
	UNIT_ENEMY,
	UNIT_NPC,
	UNIT_TYPE_MAX
};

struct Unit
{
	Unit(UnitType type) : type(type) {}
	virtual void Update(float dt) = 0;
	virtual void Save(FileWriter& f);
	virtual void Load(FileReader& f);

	bool IsEnemy(UnitType other_type) const;

	SceneNode* node;
	UnitType type;
};
