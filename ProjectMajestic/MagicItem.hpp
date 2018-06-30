#pragma once

#include "Main.hpp"
#include "Item.hpp"
#include "PlayerCharacter.hpp"


class MagicItem :public Item, public PlayerCharacter {
public:

	const string getCharacterId() override { return "magicitem_"s + getItemId(); }
	int getMaxItemsPerSlotCount() override { return 1; }
	PlayerCharacter& getUserCharactor() { return *localCharacter; /* TODO Stop using local variable (Network) */ }

	bool isControllable(PlayerCharacter& controller = *localCharacter) { return pref.isControllable(controller.getMagicPreference()); }

	// Set up magic options just like a character
	virtual void _setupMagicPreference() = 0;




	// Inherited from Item
	virtual const string getItemId() = 0;
	// Returns false when nothing is done(and break block), true when the item is used
	virtual bool _onLeftPressed() { return false; }
	virtual void _onLeftReleased() {}
	// Returns false when nothing is done(and send right click signal to the block), true when the item is used
	virtual bool _onRightPressed() {
		if (isControllable()) {
			_sendMagic();
			return true;
		}
		else
			return false;
	}
	virtual void _onRightReleased() {}

	// Send magic (Called only if controllable)
	virtual void _sendMagic() = 0;


private:


};

