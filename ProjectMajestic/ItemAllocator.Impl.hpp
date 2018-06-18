#pragma once

#include "Data.hpp"
#include "ItemAllocator.hpp"

// Include all Item headers
#include "ArrowItem.hpp"
#include "BowItem.hpp"

#define REGISTER_ITEM_TYPE(type) allocs.insert_or_assign(type(data).getItemId(), allocItem<type>)

////////////////////////////////////////
void ItemAllocator::initalaize() {
	Dataset data;
	REGISTER_ITEM_TYPE(ArrowItem);
	REGISTER_ITEM_TYPE(BowItem);
}


////////////////////////////////////////
Item* ItemAllocator::allocate(string id, Dataset& slot) {
	auto i = allocs.find(id);
	if (i == allocs.end())
		return nullptr;
	else
		return i->second(slot);
}

