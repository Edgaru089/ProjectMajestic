#pragma once

#include "ChestInventory.hpp"
#include "Chest.hpp"
#include "TerrainManager.hpp"


////////////////////////////////////////
void ChestInventory::runImGui() {
	Block* b = terrainManager.getBlock(chestBlockCoord);
	Chest* c = dynamic_cast<Chest*>(b);
	if (c == nullptr) {
		uiManager.changeUI(nullptr);
		return;
	}

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 9; j++) {
			if (j != 0)
				imgui::SameLine();
			PlayerInventoryUI::ImGuiInventorySlot(c->chestSlots[i][j], i * 9 + j);
		}

}
