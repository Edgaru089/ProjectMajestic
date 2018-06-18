#pragma once

#include <utility>
#include "Main.hpp"
#include "Chunk.hpp"
#include "EntityManager.hpp"
#include "ChunkProvider.hpp"


class TerrainManager : public Lockable {
public:

	static Vector2i convertWorldCoordToChunkId(Vector2i coord);
	static Vector2i convertWorldCoordToInChunkCoord(Vector2i coord);
	static Vector2i convertChunkToWorldCoord(Vector2i chunkId, Vector2i inChunkCoord);
	static Vector2i convertWorldPositionToBlockCoord(Vector2d position);

	static Vector2i convertScreenPixelToWorldBlockCoord(Vector2d pixel);
	static Vector2d convertScreenPixelToWorldCoord(Vector2d pixel);

public:

	~TerrainManager();

	void updateLogic();
	void getRenderList(VertexArray& array); // Triangles
	void getLightMask(VertexArray& array); // Triangles

	void loadChunk(Vector2i id, ChunkProvider& provider);

	void unloadChunk(Vector2i id);

	// Just stores the data; never uses it
	void setChunkCount(Vector2i chunkCount) { this->chunkCount = chunkCount; }
	Vector2i getChunkCount() { return chunkCount; }

	vector<Vector2i>& getSpawnpoints() { return spawns; }
	
	// First: pos(global coords); Second: strength
	map<Uuid, pair<Vector2i, int>>& getLightSources() { return lightSources; }

	void clear();

	Chunk* getChunk(Vector2i chunkId);
	Block* getBlock(Vector2i coord);

	// SetBlock() doesn't call the block's OnDestroy() function
	// Use PlaceBlock() when player places a block
	void setBlock(Vector2i coord, string blockId, bool isForced = false);

public:

	// Destroy/Break a block
	void breakBlock(Vector2i pos, Entity * breaker = nullptr, bool isForced = false);

	// Place a block
	// Does nothing if the block isn't empty
	void placeBlock(Vector2i pos, string blockId, Entity * placer = nullptr, bool isForced = false);

	// Pose a beam of light on (pos), strength: 0~11 (maxLightingLevel)
	// Return: lighting id
	Uuid poseLight(Vector2i pos, int strength);

	void poseLightForced(Uuid id, Vector2i pos, int strength);

	// Vector2i: global coords; int: light strength
	pair<Vector2i, int> getLight(Uuid id);

	// Remove the beam of light with Uuid id
	// Does nothing if not found
	void removeLight(Uuid id);

	void _updateLighting(Vector2i chunkId);

private:


	map<Vector2i, Chunk*, Vector2Less<int>> chunks;
	map<Uuid, pair<Vector2i, int>> lightSources; // First: pos(global coords); Second: strength

	// Just stores the data; never uses it
	Vector2i chunkCount;
	vector<Vector2i> spawns;
};


TerrainManager terrainManager;
