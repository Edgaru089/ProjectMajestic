#pragma once

#include "FallingSand.hpp"


////////////////////////////////////////
void FallingSand::_updateLogic() {
	if (terrainManager.getBlock(TerrainManager::convertWorldPositionToBlockCoord(getPosition())) == nullptr) {
		if (onGround) {
			terrainManager.placeBlock(TerrainManager::convertWorldPositionToBlockCoord(getPosition()),
				getData("block_id").getDataString());
			kill();
		}
	}
	else {
		if (onGround || isSame(vecY, 0)) {
			// Drop item
			// Borrow code from Block::onDestory()

			// Tile Drop
			ItemEntity* e = new ItemEntity("block_" + getData("block_id").getDataString());
			for (auto& i : datasets.getDatasets()) {
				e->getDataset().getDatasets().insert(i);
			}
			// Give a random velocity
			e->accelerateVector(1.0, 180 + rand() % 180);

			entityManager.insert(e, getPosition());

			kill();
		}
	}
}
