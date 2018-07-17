#pragma once

#include "Data.hpp"
#include "ItemAllocator.hpp"

// Include all Item headers
#include "ArrowItem.hpp"
#include "BowItem.hpp"
#include "MinigunItem.hpp"
#include "GrenadeItem.hpp"
#include "RpgLauncherItem.hpp"
#include "M16A4.hpp"
#include "Barrett.hpp"

#define REGISTER_ITEM_TYPE(type) allocs.insert_or_assign(type(data).getItemId(), allocItem<type>)

////////////////////////////////////////
void ItemAllocator::initalaize() {
	Dataset data;
	REGISTER_ITEM_TYPE(ArrowItem);
	REGISTER_ITEM_TYPE(BowItem);
	REGISTER_ITEM_TYPE(MinigunItem);
	REGISTER_ITEM_TYPE(MinigunAmmoItem);
	REGISTER_ITEM_TYPE(GrenadeItem);
	REGISTER_ITEM_TYPE(RpgAmmoItem);
	REGISTER_ITEM_TYPE(RpgLauncherItem);
	REGISTER_ITEM_TYPE(M16A4);
	REGISTER_ITEM_TYPE(M16A4Ammo);
	REGISTER_ITEM_TYPE(Barrett);
	REGISTER_ITEM_TYPE(BarrettAmmo);
}


////////////////////////////////////////
Item* ItemAllocator::allocate(string id, Dataset& slot, bool hasFocus) {
	auto i = allocs.find(id);
	if (i == allocs.end())
		return nullptr;
	else {
		Item* it = i->second(slot);
		it->setFocus(hasFocus);
		return it;
	}
}

