#pragma once

#include "MagicItem.hpp"


// �ߥ˰���¯ / �������¯
class MiniHakkero :public MagicItem {
public:

	const string getItemId() override { return "mini_hakkero"; }

	void _setupMagicPreference() override {
		pref.Element = 0.8;
		pref.Phantom = 0.6;
		pref.ElementFire = 0.8;
		pref.PhantomHuman = 0.6;
		majors.push_back(ElementFire);
		majors.push_back(PhantomHuman);
	}

	void _sendMagic() override {
		// TODO Magic
		mlog << "[Magic] Mini-Hakkero Invoked" << dlog;
	}

private:

};
