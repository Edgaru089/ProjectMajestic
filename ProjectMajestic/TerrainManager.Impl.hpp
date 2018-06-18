
#include "TerrainManager.hpp"
#include "IO.hpp"


////////////////////////////////////////
Vector2i TerrainManager::convertWorldCoordToChunkId(Vector2i coord) {
	int X, Y;

	if (coord.x >= 0)
		X = coord.x / chunkSize;
	else
		X = -((-coord.x - 1) / chunkSize) - 1;

	if (coord.y >= 0)
		Y = coord.y / chunkSize;
	else
		Y = -((-coord.y - 1) / chunkSize) - 1;

	return Vector2i(X, Y);
}


////////////////////////////////////////
Vector2i TerrainManager::convertWorldCoordToInChunkCoord(Vector2i coord) {
	int X, Y;

	if (coord.x >= 0)
		X = coord.x % chunkSize;
	else
		X = chunkSize - ((-coord.x - 1) % chunkSize) - 1;

	if (coord.y >= 0)
		Y = coord.y % chunkSize;
	else
		Y = chunkSize - ((-coord.y - 1) % chunkSize) - 1;

	return Vector2i(X, Y);
}


////////////////////////////////////////
Vector2i TerrainManager::convertChunkToWorldCoord(Vector2i chunkId, Vector2i inChunkCoord) {
	return chunkId * chunkSize + inChunkCoord;
}


////////////////////////////////////////
Vector2i TerrainManager::convertWorldPositionToBlockCoord(Vector2d position) {
	int X, Y;
	X = position.x;
	Y = position.y;
	if (position.x < 0)
		X--;
	if (position.y < 0)
		Y--;
	return Vector2i(X, Y);
}


////////////////////////////////////////
Vector2i TerrainManager::convertScreenPixelToWorldBlockCoord(Vector2d pixel) {
	int X, Y;

	if (pixel.x - renderIO.gameScenePosOffset.x >= 0)
		X = (pixel.x - renderIO.gameScenePosOffset.x) / renderIO.gameScaleFactor;
	else
		X = (pixel.x - renderIO.gameScenePosOffset.x) / renderIO.gameScaleFactor - 1;

	if (pixel.y - renderIO.gameScenePosOffset.y >= 0)
		Y = (pixel.y - renderIO.gameScenePosOffset.y) / renderIO.gameScaleFactor;
	else
		Y = (pixel.y - renderIO.gameScenePosOffset.y) / renderIO.gameScaleFactor - 1;

	return Vector2i(X, Y);
}


////////////////////////////////////////
Vector2d TerrainManager::convertScreenPixelToWorldCoord(Vector2d pixel) {
	double X, Y;

	if (pixel.x - renderIO.gameScenePosOffset.x >= 0)
		X = (pixel.x - renderIO.gameScenePosOffset.x) / renderIO.gameScaleFactor;
	else
		X = (pixel.x - renderIO.gameScenePosOffset.x) / renderIO.gameScaleFactor - 1;

	if (pixel.y - renderIO.gameScenePosOffset.y >= 0)
		Y = (pixel.y - renderIO.gameScenePosOffset.y) / renderIO.gameScaleFactor;
	else
		Y = (pixel.y - renderIO.gameScenePosOffset.y) / renderIO.gameScaleFactor - 1;

	return Vector2d(X, Y);
}


////////////////////////////////////////
TerrainManager::~TerrainManager() {
	for (auto i : chunks) {
		delete i.second;
	}
}


////////////////////////////////////////
void TerrainManager::updateLogic() {
	AUTOLOCKABLE(*this);
	if (role == Server)
		for (pair<const Vector2i, Chunk*>&i : chunks) {
			i.second->updateLogic();
		}

	// Update right click
	if (!imgui::GetIO().WantCaptureMouse && logicIO.mouseState[Mouse::Right] == LogicIO::JustPressed) {
	}
}


////////////////////////////////////////
void TerrainManager::getRenderList(VertexArray& array) {
	AUTOLOCKABLE(*this);
	for (pair<const Vector2i, Chunk*>&i : chunks) {
		i.second->getRenderList(array);
	}
}


////////////////////////////////////////
void TerrainManager::getLightMask(VertexArray& array) {
	AUTOLOCKABLE(*this);
	for (pair<const Vector2i, Chunk*>&i : chunks) {
		i.second->getLightMask(array);
	}
}


////////////////////////////////////////
void TerrainManager::loadChunk(Vector2i id, ChunkProvider& provider) {
	AUTOLOCKABLE(*this);
	if (chunks.find(id) != chunks.end())
		delete chunks[id];
	map<Vector2i, Chunk*, Vector2Less<int>>::iterator i = chunks.insert_or_assign(id, new Chunk()).first;
	i->second->id = id;
	i->second->setChunkId(id.x, id.y);
	provider.loadChunk(id, *chunks[id]);
}


////////////////////////////////////////
void TerrainManager::unloadChunk(Vector2i id) {
	AUTOLOCKABLE(*this);
	map<Vector2i, Chunk*>::iterator i;
	if ((i = chunks.find(id)) != chunks.end()) {
		delete chunks[id];
		chunks.erase(i);
	}
}


////////////////////////////////////////
void TerrainManager::clear() {
	AUTOLOCKABLE(*this);
	for (auto i : chunks) {
		delete i.second;
	}

	chunks.clear();
	lightSources.clear();
}


////////////////////////////////////////
Chunk* TerrainManager::getChunk(Vector2i chunkId) {
	AUTOLOCKABLE(*this);
	map<Vector2i, Chunk*, Vector2Less<int>>::iterator i = chunks.find(chunkId);
	if (i != chunks.end())
		return i->second;
	else
		return nullptr;
}


////////////////////////////////////////
Block* TerrainManager::getBlock(Vector2i coord) {
	AUTOLOCKABLE(*this);
	map<Vector2i, Chunk*, Vector2Less<int>>::iterator i = chunks.find(
		convertWorldCoordToChunkId(coord));
	if (i != chunks.end() && i->second != nullptr)
		return i->second->getBlock(convertWorldCoordToInChunkCoord(coord));
	else
		return nullptr;
}


////////////////////////////////////////
void TerrainManager::setBlock(Vector2i coord, string blockId, bool isForced) {
	AUTOLOCKABLE(*this);
	if (!isForced || role == Server) {
		networkClient.notifyBlockSet(coord, blockId);
		networkServer.notifyBlockSet(coord, blockId);
	}
	if (isForced || role == Server)
		if (chunks.find(convertWorldCoordToChunkId(coord)) != chunks.end()) {
			chunks[convertWorldCoordToChunkId(coord)]->setBlock(
				convertWorldCoordToInChunkCoord(coord), blockAllocator.allocate(blockId));
		}
}


////////////////////////////////////////
void TerrainManager::breakBlock(Vector2i pos, Entity * breaker, bool isForced) {
	AUTOLOCKABLE(*this);
	if (!isForced || role == Server) {
		networkClient.notifyBlockBreak(pos);
		networkServer.notifyBlockBreak(pos);
	}
	if (isForced || role == Server) {
		map<Vector2i, Chunk*, Vector2Less<int>>::iterator i = chunks.find(convertWorldCoordToChunkId(pos));
		if (i == chunks.end()) // Chunk not loaded
			return;

		Chunk* c = i->second;
		if (c->getBlock(convertWorldCoordToInChunkCoord(pos)) != nullptr) {
			c->getBlock(convertWorldCoordToInChunkCoord(pos))->onDestroy(breaker, role == Server);
			c->setBlock(convertWorldCoordToInChunkCoord(pos), nullptr); // nullptr means empty
		}
	}
}


////////////////////////////////////////
void TerrainManager::placeBlock(Vector2i pos, string blockId, Entity * placer, bool isForced) {
	AUTOLOCKABLE(*this);
	if (!isForced || role == Server) {
		networkClient.notifyBlockPlace(pos, blockId);
		networkServer.notifyBlockPlace(pos, blockId);
	}
	if (isForced || role == Server) {
		map<Vector2i, Chunk*, Vector2Less<int>>::iterator i = chunks.find(convertWorldCoordToChunkId(pos));
		if (i == chunks.end() || i->second == nullptr)
			return;
		if (i->second->getBlock(convertWorldCoordToInChunkCoord(pos)) == nullptr) {
			Block* b = blockAllocator.allocate(blockId);
			i->second->setBlock(convertWorldCoordToInChunkCoord(pos), b);
			b->_onPlaced(placer);
		}
	}
}


////////////////////////////////////////
Uuid TerrainManager::poseLight(Vector2i pos, int strength) {
	AUTOLOCKABLE(*this);
	Uuid id = Uuid::get();

	if (getChunk(convertWorldCoordToChunkId(pos)) == nullptr)
		return Uuid();

	lightSources.insert_or_assign(id, pair<Vector2i, int>(pos, strength));
	getChunk(convertWorldCoordToChunkId(pos))->lightSources.insert_or_assign(
		id, pair<Vector2i, int>(pos, strength));

	Vector2i chunk = convertWorldCoordToChunkId(pos);
	int offsetX[9] = { 0, 1, 0, -1, 0, 1, 1, -1, -1 };
	int offsetY[9] = { 0, 0, 1, 0, -1, 1, -1, -1, 1 };
	for (int i = 0; i < 9; i++) {
		int X = chunk.x + offsetX[i];
		int Y = chunk.y + offsetY[i];
		Chunk* ct;
		if ((ct = getChunk(Vector2i(X, Y))) != nullptr) {
			_updateLighting(Vector2i(X, Y));
		}
	}

	return id;
}


////////////////////////////////////////
void TerrainManager::poseLightForced(Uuid id, Vector2i pos, int strength) {
	AUTOLOCKABLE(*this);

	lightSources.insert_or_assign(id, pair<Vector2i, int>(pos, strength));
	getChunk(convertWorldCoordToChunkId(pos))->lightSources.insert_or_assign(id, pair<Vector2i, int>(pos, strength));

	//Don't update lighting
}


////////////////////////////////////////
void TerrainManager::removeLight(Uuid id) {
	AUTOLOCKABLE(*this);
	if (id == Uuid::nil())
		return;

	auto i = lightSources.find(id);
	if (i == lightSources.end())
		return;
	Vector2i chunk = convertWorldCoordToChunkId(i->second.first);
	lightSources.erase(i);
	chunks[chunk]->lightSources.erase(id);

	int offsetX[9] = { 0, 1, 0, -1, 0, 1, 1, -1, -1 };
	int offsetY[9] = { 0, 0, 1, 0, -1, 1, -1, -1, 1 };
	for (int i = 0; i < 9; i++) {
		int X = chunk.x + offsetX[i];
		int Y = chunk.y + offsetY[i];
		Chunk* ct;
		if ((ct = getChunk(Vector2i(X, Y))) != nullptr) {
			_updateLighting(Vector2i(X, Y));
		}
	}

}


////////////////////////////////////////
void TerrainManager::_updateLighting(Vector2i chunkId) {
	AUTOLOCKABLE(*this);
	map<Vector2i, Chunk*, Vector2Less<int>>::iterator i = chunks.find(chunkId);
	if (i == chunks.end() || i->second == nullptr)
		return;

	Chunk* c = i->second;
	vector<pair<Vector2i, int>> lights;

	int offsetX[9] = { 0, 1, 0, -1, 0, 1, 1, -1, -1 };
	int offsetY[9] = { 0, 0, 1, 0, -1, 1, -1, -1, 1 };
	for (int i = 0; i < 9; i++) {
		int X = c->getChunkId().x + offsetX[i];
		int Y = c->getChunkId().y + offsetY[i];
		Chunk* ct;
		if ((ct = getChunk(Vector2i(X, Y))) != nullptr) {
			for (auto i : ct->lightSources) {
				lights.push_back(i.second);
			}
		}
	}

	for (int i = 0; i < chunkSize; i++)
		for (int j = 0; j < chunkSize; j++) {
			Vector2i coord = convertChunkToWorldCoord(chunkId, Vector2i(i, j));
			int Max = 0;
			for (pair<Vector2i, int> k : lights) {
				int level;
				if (k.first == coord)
					level = k.second;
				else
					level = k.second - (abs(k.first.x - coord.x) + abs(k.first.y - coord.y));
				Max = max(Max, level);
			}
			c->lightLevel[i][j] = Max;
		}

}

