#pragma once

#include "Main.hpp"


class InGameUI {
public:

	virtual void runImGui() = 0;

	virtual string windowTitle() = 0;
	virtual bool showInventory() = 0;

};


class InGameUIManager :public Lockable {
public:

	~InGameUIManager();

	void updateLogic();


	void runImGui();

	bool isUIOpen() { return curUI != nullptr; }

public:

	void changeUI(shared_ptr<InGameUI> gui) {
		lock();
		curUI = gui;
		unlock();
	}


private:

	void _runInventoryUI();

	shared_ptr<InGameUI> curUI;

};

InGameUIManager uiManager;


class PlayerInventoryUI :public InGameUI {
public:

	static void ImGuiInventorySlot(Dataset& slotData, int pushId = -1);

	virtual void runImGui() override;
	virtual string windowTitle() override { return "Player Inventory"; }
	virtual bool showInventory() override { return true; }
};
