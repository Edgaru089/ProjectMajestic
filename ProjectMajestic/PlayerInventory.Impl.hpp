#pragma once

#include "PlayerInventory.hpp"
#include "TextureManager.hpp"


////////////////////////////////////////
PlayerInventory::PlayerInventory() {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 9; j++) {
			_emptyDatasetSlot(slots[i][j]);
		}
	_emptyDatasetSlot(slotCursor);
	slots[0][0].setData("item_name", "item_bow"s);
	slots[0][0].setData("count", 1);
	slots[0][1].setData("item_name", "block_chest"s);
	slots[0][1].setData("count", 1);
	slots[0][2].setData("item_name", "block_torch"s);
	slots[0][2].setData("count", 1);
	slots[0][3].setData("item_name", "block_ladder"s);
	slots[0][3].setData("count", 5);
	slots[0][4].setData("item_name", "item_mini_hakkero"s);
	slots[0][4].setData("count", 1);
}


////////////////////////////////////////
void PlayerInventory::runImGui() {
	imgui::PushStyleColor(ImGuiCol_Button, Color::Transparent);
	imgui::PushStyleColor(ImGuiCol_ButtonHovered, Color(255, 255, 255, 64));
	imgui::PushStyleColor(ImGuiCol_ButtonActive, Color(255, 255, 255, 48));
	imgui::PushStyleColor(ImGuiCol_WindowBg, Color(0, 0, 0, 128));
	imgui::SetNextWindowPos(
		ImVec2(imgui::GetIO().DisplaySize.x / 2.0f, imgui::GetIO().DisplaySize.y + 1),
		ImGuiCond_Always,
		ImVec2(0.5f, 1.0f));
	imgui::Begin("BottomInventory", nullptr,
				 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
	for (int j = 0; j < 9; j++) {
		if (j != 0)
			imgui::SameLine(0, 3);
		if (j == cursorId) {
			imgui::PushStyleColor(ImGuiCol_Border, Color(192, 192, 192));
			imgui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.5);
		}

		TextureInfo info;
		if (slots[0][j]["item_name"].getDataString().substr(0, 5) == "item_")
			info = itemAllocator.allocate(slots[0][j]["item_name"].getDataString().substr(5), slots[0][j])->getTextureInfo();
		else
			info = textureManager.getTextureInfo(slots[0][j]["item_name"].getDataString());
		if (!info.vaild)
			info = textureManager.getTextureInfo("ui_transparent_32px");

		if (imgui::ImageButton(info.getSprite(), Vector2f(32, 32), 3)) {
			cursorId = j;
			imgui::PushStyleColor(ImGuiCol_Border, Color(192, 192, 192));
			imgui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.5);
		}

		if (info.id != "ui_transparent_32px") {
			ImVec2 pos = imgui::GetItemRectMin(); pos.x += 2.0;
			if (slots[0][j]["count"].getDataInt() != 1)
				imgui::GetCurrentWindow()->DrawList->AddText(pos, ImU32(0xFFFFFFFF), StringParser::toString(slots[0][j]["count"].getDataInt()).c_str());
		}

		if (j == cursorId) {
			imgui::PopStyleColor();
			imgui::PopStyleVar();
		}
	}
	Vector2f pos = imgui::GetWindowPos(), size = imgui::GetWindowSize();
	imgui::End();

	imgui::SetNextWindowPos(
		ImVec2(imgui::GetIO().DisplaySize.x / 2.0f, imgui::GetIO().DisplaySize.y - size.y + 2),
		ImGuiCond_Always,
		ImVec2(0.5f, 1.0f));
	imgui::SetNextWindowSize(ImVec2(size.x, 0.0f), ImGuiCond_Always);
	imgui::Begin("BottomInventoryExtend", nullptr,
				 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	imgui::Text(slots[0][cursorId]["item_name"].getDataString().c_str());

	imgui::End();

	imgui::PopStyleColor(4);
}


////////////////////////////////////////
void PlayerInventory::_emptyDatasetSlot(Dataset& slot) {
	slot.getDatasets().clear();
	slot.setData("item_name", ""s);
	slot.setData("count", 0);
}

