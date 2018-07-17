#pragma once

#include "GunItem.hpp"

class M16A4 :public GunItem {
public:
	M16A4(Dataset& data) :GunItem(data) {}

	const string getItemId() override { return "m16a4"; }

	Time shootInterval() override { return milliseconds(150); } // 400 RPM
	double bulletDamage() override { return 6.0; }
	double bulletSpeed() override { return 30.0; }
	Time reloadTime() override { return seconds(2.0f); }
	int roundsPerMagazine() override { return 30; }
	string magazineItemName() override { return "item_m16a4_ammo"; }

};

class M16A4Ammo :public Item {
public:
	M16A4Ammo(Dataset& d) :Item(d) {}
	const string getItemId() override { return "m16a4_ammo"; }
	int getMaxItemsPerSlotCount() override { return 12; }
};
