#pragma once

#include <string>
#include <map>
#include "Main.hpp"

#include "TextureManager.hpp"
#include "Data.hpp"
#include "Chunk.hpp"
#include "BlockAllocator.hpp"

class Entity;

const int maxLightingLevel = 12;

class Block {
public:

	static Block* alloc(string id) {
		return blockAllocator.allocate(id);
	}

public:

	friend class TerrainManager;

	virtual ~Block() {}

	virtual const string getBlockId() = 0;
	Block* copyAllocate() {
		Block* b = blockAllocator.allocate(getBlockId());
		b->inChunkPos = inChunkPos;
		b->chunk = chunk;
		b->datasets = datasets;
		_copyCustomData(b);
		return b;
	}

	string getTextureId() { return "block_" + getBlockId(); }
	TextureInfo getTextureInfo() { return textureManager.getTextureInfo(getTextureId()); }

	virtual const bool requestSpeicalRendering() { return false; }
	virtual void pushTriangleVertexes(VertexArray& arr) {}

	virtual bool isSolid() { return true; }

	// Zerom means none
	virtual int getLightStrength() { return 0; }

	Data& getData(string id) {
		return datasets[id];
	}

	template<typename Type>
	void setData(string id, Type data) {
		datasets[id].setData(data);
	}

	Dataset& getDataset() { return datasets; }

	Vector2i getInChunkPosition() { return inChunkPos; }
	void setInChunkPosition(int x, int y) { inChunkPos.x = x; inChunkPos.y = y; }
	void setInChunkPosition(Vector2i pos) { this->inChunkPos = pos; }

	// Global block coords
	Vector2i getPosition();

	virtual DoubleRect getHitbox();

	Vector2i getChunkId() { return chunk; }
	void setChunkId(Vector2i chunkId) { chunk = chunkId; }

	void onDestroy(Entity* destoryer = nullptr, bool drop = true);

public:

	virtual void _copyCustomData(Block* b) {}

	virtual void _onPlaced(Entity* placer) {}
	virtual void _updateLogic() {}
	virtual void _onCollision(Entity* entity) {}
	virtual void _onDestroy(Entity* destroyer) {}
	virtual void _onRightClick() {}

protected:

	Vector2i inChunkPos, chunk;

	Dataset datasets;

};
