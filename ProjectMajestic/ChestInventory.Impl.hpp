#pragma once

#include "ChestInventory.hpp"
#include "Chest.hpp"
#include "TerrainManager.hpp"
#include "TextSystem.hpp"


////////////////////////////////////////
string ChestInventory::windowTitle() {
	return text.getstr("block_chest.name");
}


////////////////////////////////////////
void ChestInventory::runImGui() {
	if (imgui::BeginPopupModal(windowTitle().c_str())) {
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
		imgui::EndPopup();
	}
}
