#pragma once

#include "Main.hpp"

class Block;

class BlockAllocator {
public:
	virtual ~BlockAllocator() {}
	void initalaize();
	Block* allocate(string id);
private:
	map<string, function<Block*(void)>> allocs;
};

BlockAllocator blockAllocator;

template<typename BlockType>
Block* alloc() { return new BlockType(); }
