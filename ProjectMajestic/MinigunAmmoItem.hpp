#pragma once

#include "Item.hpp"

class MinigunAmmoItem :public Item {
public:

	MinigunAmmoItem(Dataset& d) :Item(d) {}

	const string getItemId() override { return "minigun_ammo"; }
	int getMaxItemsPerSlotCount() override { return 8; }

private:

};

