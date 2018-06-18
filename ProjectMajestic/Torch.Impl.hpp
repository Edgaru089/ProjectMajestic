
#include "Torch.hpp"
#include "TerrainManager.hpp"


////////////////////////////////////////
Torch::~Torch() {
	//if (lightId != Uuid())
		//terrainManager.removeLight(lightId);
}


////////////////////////////////////////
void Torch::_updateLogic() {
	if (lightId() == Uuid())
		lightId() = terrainManager.poseLight(TerrainManager::convertChunkToWorldCoord(getChunkId(), getInChunkPosition()), 12);
}


////////////////////////////////////////
inline void Torch::_onPlaced(Entity * placer) {
	lightId() = terrainManager.poseLight(TerrainManager::convertChunkToWorldCoord(getChunkId(), getInChunkPosition()), 12);
}


////////////////////////////////////////
void Torch::_onDestroy(Entity* destroyer) {
	terrainManager.removeLight(lightId());
	lightId() = Uuid();
}

