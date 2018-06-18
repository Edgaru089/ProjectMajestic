#pragma once

#include "InGameUI.hpp"


class ChestInventory :public InGameUI {
public:
	ChestInventory(Vector2i chestBlockCoord) :chestBlockCoord(chestBlockCoord) {}

	void runImGui() override;
	string windowTitle() override { return "Chest Inventory"s; }
	bool showInventory() override { return true; }
private:
	Vector2i chestBlockCoord;
};

