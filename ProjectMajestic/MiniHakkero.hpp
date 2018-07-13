#pragma once

#include "MagicItem.hpp"


// ¥ß¥Ë°ËØÔÂ¯ / ÃÔÄã°ËØÔÂ¯
class MiniHakkero :public MagicItem {
public:
	MiniHakkero(Dataset& data) :MagicItem(data) {
		pref.Element = 0.8;
		pref.Phantom = 0.6;
		pref.ElementFire = 0.8;
		pref.PhantomHuman = 0.6;
		majors.push_back(ElementFire);
		majors.push_back(PhantomHuman);
	}

	const string getItemId() override { return "mini_hakkero"; }

	void _sendMagic() override {
		// TODO Magic
		mlog << "[Magic] Mini-Hakkero Invoked" << dlog;
	}

private:

};
