#pragma once

#include "Main.hpp"

class Entity;

class EntityAllocator {
public:
	void initalaize();
	Entity* allocate(string id);
private:
	map<string, function<Entity*(void)>> allocs;
};

EntityAllocator entityAllocator;

template<typename EntityType>
Entity* allocEntity() { return new EntityType(); }
