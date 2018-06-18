#pragma once

#include "EntityAllocator.hpp"

// Include all entity headers
#include "TestEntity.hpp"
#include "ItemEntity.hpp"
#include "FallingSand.hpp"
#include "PlayerEntity.hpp"
#include "ArrowEntity.hpp"

#define REGISTER_ENTITY_TYPE(type) allocs.insert_or_assign(type().getEntityId(), allocEntity<type>)

////////////////////////////////////////
void EntityAllocator::initalaize() {
	REGISTER_ENTITY_TYPE(TestEntity);
	REGISTER_ENTITY_TYPE(ItemEntity);
	REGISTER_ENTITY_TYPE(FallingSand);
	REGISTER_ENTITY_TYPE(PlayerEntity);
	REGISTER_ENTITY_TYPE(ArrowEntity);
}


////////////////////////////////////////
Entity* EntityAllocator::allocate(string id) {
	auto i = allocs.find(id);
	if (i == allocs.end())
		return nullptr;
	else
		return i->second();
}
