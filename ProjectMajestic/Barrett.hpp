#pragma once

#include "GunItem.hpp"

class Barrett :public GunItem {
public:
	Barrett(Dataset& data) :GunItem(data) {}

	const string getItemId() override { return "barrett"; }

	Time shootInterval() override { return seconds(1.2f); }
	double bulletDamage() override { return 25.0; }
	double bulletSpeed() override { return 40.0; }
	Time reloadTime() override { return seconds(2.0f); }
	int roundsPerMagazine() override { return 10; }
	string magazineItemName() override { return "item_barrett_ammo"; }

};

class BarrettAmmo :public Item {
public:
	BarrettAmmo(Dataset& d) :Item(d) {}
	const string getItemId() override { return "barrett_ammo"; }
	int getMaxItemsPerSlotCount() override { return 6; }
};
