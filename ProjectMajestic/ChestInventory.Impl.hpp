#pragma once

#include "ChestInventory.hpp"
#include "Chest.hpp"
#include "TerrainManager.hpp"


////////////////////////////////////////
void ChestInventory::runImGui() {
	shared_ptr<Block> b = terrainManager.getBlock(chestBlockCoord);
	if (b == nullptr) {
		uiManager.changeUI(nullptr);
		return;
	}
	try {
		Chest& c = dynamic_cast<Chest&>(*b);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 9; j++) {
				if (j != 0)
					imgui::SameLine();
				PlayerInventoryUI::ImGuiInventorySlot(c.chestSlots[i][j], i * 9 + j);
			}
	}
	catch (bad_cast) {
		uiManager.changeUI(nullptr);
		return;
	}
}
