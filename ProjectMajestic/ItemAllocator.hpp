#pragma once

#include "Main.hpp"

class Item;
class Dataset;

class ItemAllocator {
public:
	virtual ~ItemAllocator() {}
	void initalaize();
	Item* allocate(string id, Dataset& slot);
private:
	map<string, function<Item*(Dataset&)>> allocs;
};

ItemAllocator itemAllocator;

template<typename ItemType>
Item* allocItem(Dataset& slot) { return new ItemType(slot); }
