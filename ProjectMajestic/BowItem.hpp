#pragma once

#include "Item.hpp"

class BowItem :public Item {
public:
	BowItem(Dataset& data) :Item(data) {}
	const string getItemId() override { return "bow"; }
	int getMaxItemsPerSlotCount() override { return 1; }
	TextureInfo getTextureInfo() override {
		if (slotDataset["bow_start_time"].getDataInt() == 0)
			return textureManager.getTextureInfo("item_bow");
		else
			return textureManager.getTextureInfo(StringParser::toStringF("item_bow_pulling_%d",
				min(3, 1 + (programRunTimeClock.getElapsedTime().asMilliseconds()
					- slotDataset["bow_start_time"].getDataInt()) / 400)));
	}

	bool _onRightPressed() override;
	void _onRightReleased() override;

};
