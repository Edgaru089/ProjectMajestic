#pragma once

#include "Item.hpp"

class RpgAmmoItem :public Item {
public:
	RpgAmmoItem(Dataset& data) :Item(data) {}

	const string getItemId() override { return "rpg_ammo"; }
	int getMaxItemsPerSlotCount() override { return 16; }

};

