#pragma once

#include "Unit.h"

struct Player : Unit
{
	Player() : Unit(UNIT_PLAYER) {}
	void Update(float dt) override;
	void Save(FileWriter& f) override;
	void Load(FileReader& f) override;

	int gold,
		potions,
		collectibles,
		air_potions,
		weapon_upgrades,
		armor_upgrades;
	bool have_avarice,
		have_air_potion,
		have_fireproof,
		have_coldproof,
		have_wings;

	static const int MAX_POTIONS = 5;
	static const int MAX_COLLECTIBLES = 20;
	static const float radius;
	static const float height;
};
