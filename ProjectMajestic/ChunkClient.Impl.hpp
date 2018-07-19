#pragma once

#include "ChunkClient.hpp"
#include "TerrainManager.hpp"


/*
	Packet

	(int)chunkId.x, (int)chunkId.y
	for (chunkSize*chunkSize) {
		(string)blockId, (int)lightLevel, (int)datasetCount
		for (datasetCount) {
			(string)dataId, (Uint8)dataType
			if (dataType==0) // Int
				(int)data
			else if(dataType==1) // String
				(string)data
		}
	}
*/

////////////////////////////////////////
void ChunkClient::parseChunkData(Packet & packet) {
	Vector2i chunkId;
	packet >> chunkId.x >> chunkId.y;

	if (chunkId == Vector2i(INT_MIN, INT_MAX))
		return; // Empty chunk data; just return

	AUTOLOCKABLE(terrainManager);
	terrainManager.unloadChunk(chunkId);

	auto func = [&](Vector2i chunkIdEx, Chunk& chunk) {
		if (chunkIdEx != chunkId)
			return;

		string id, data;
		Int32 light;

		chunk.blocks.resize(chunkSize);
		chunk.lightLevel.resize(chunkSize);
		for (int i = 0; i < chunkSize; i++) {
			chunk.blocks[i].resize(chunkSize);
			chunk.lightLevel[i].resize(chunkSize);
			for (int j = 0; j < chunkSize; j++) {
				packet >> id >> light;
				chunk.blocks[i][j] = blockAllocator.allocate(id);
				if (chunk.blocks[i][j] != nullptr) {
					chunk.blocks[i][j]->setInChunkPosition(i, j);
					chunk.blocks[i][j]->setChunkId(chunkId);
					packet >> chunk.blocks[i][j]->getDataset();
				}
				chunk.lightLevel[i][j] = light;
			}
		}
	};

	ChunkProviderWrapper wrapper(func);
	terrainManager.loadChunk(chunkId, wrapper);

}


////////////////////////////////////////
void ChunkClient::compressChunkData(Vector2i chunkId, Packet & packet) {
	AUTOLOCKABLE(terrainManager);

	shared_ptr<Chunk> c = terrainManager.getChunk(chunkId);
	Vector2i offset = TerrainManager::convertChunkToWorldCoord(chunkId, Vector2i(0, 0));
	if (c == nullptr) {
		packet << INT_MIN << INT_MAX;
		return;
	}

	packet << (Int32)chunkId.x << (Int32)chunkId.y;

	for (int i = 0; i < chunkSize; i++)
		for (int j = 0; j < chunkSize; j++) {
			if (c->blocks[i][j] == nullptr)
				packet << "NULL"s;
			else
				packet << c->blocks[i][j]->getBlockId();
			packet << (Int32)c->lightLevel[i][j];
			if (c->blocks[i][j] != nullptr) {
				packet << c->blocks[i][j]->getDataset();
			}
		}

}


/*
	Block

	(int)cord.x, (int)coord.y
	(string)blockId, (int)lightLevel, (int)datasetCount
	for (datasetCount) {
		(string)dataId, (Uint8)dataType
		if (dataType==0) // Int
			(int)data
		else if(dataType==1) // String
			(string)data
	}

*/

////////////////////////////////////////
void ChunkClient::parseBlockData(Packet & packet) {
	Vector2i coord;
	packet >> coord.x >> coord.y;

	if (coord == Vector2i(INT_MIN, INT_MAX))
		return;

	AUTOLOCKABLE(terrainManager);
	Vector2i chunkId = TerrainManager::convertWorldCoordToChunkId(coord),
		inChunk = TerrainManager::convertWorldCoordToInChunkCoord(coord);
	shared_ptr<Chunk> c = terrainManager.getChunk(chunkId);

	string id;
	int light;

	packet >> id >> light;
	c->setBlock(inChunk, blockAllocator.allocate(id));
	if (c->getBlock(inChunk) != nullptr) {
		c->getBlock(inChunk)->setInChunkPosition(inChunk);
		c->getBlock(inChunk)->setChunkId(chunkId);
		packet >> c->getBlock(inChunk)->getDataset();
	}
	c->lightLevel[inChunk.x][inChunk.y] = light;

}


////////////////////////////////////////
void ChunkClient::compressBlockData(Vector2i coord, Packet & packet) {
	AUTOLOCKABLE(terrainManager);
	Vector2i chunkId = TerrainManager::convertWorldCoordToChunkId(coord),
		inChunk = TerrainManager::convertWorldCoordToInChunkCoord(coord);
	shared_ptr<Chunk> c = terrainManager.getChunk(chunkId);

	if (c == nullptr) {
		packet << INT_MIN << INT_MAX;
		return;
	}

	packet << coord.x << coord.y;

	if (c->blocks[inChunk.x][inChunk.y] == nullptr)
		packet << "NULL"s;
	else
		packet << c->blocks[inChunk.x][inChunk.y]->getBlockId();
	packet << (Int32)c->lightLevel[inChunk.x][inChunk.y];
	if (c->blocks[inChunk.x][inChunk.y] != nullptr) {
		packet << c->blocks[inChunk.x][inChunk.y]->getDataset();
	}
}

