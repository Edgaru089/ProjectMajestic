#pragma once

#include "PlayerCharacter.hpp"

// ������
namespace Major {

	// ����
	class HakureiReimu :public PlayerCharacter {
	public:
		const string getCharacterId() override { return "hakurei_reimu"; }
		HakureiReimu() :PlayerCharacter() {
			pref.Phantom = 1.0;       // ������
			pref.PhantomHuman = 1.0;
			majors.push_back(PurePhantomHuman);
		}
	};

	// ħ��ɳ
	class KirisameMarisa :public PlayerCharacter {
	public:
		const string getCharacterId() override { return "kirisame_marisa"; }
		KirisameMarisa() :PlayerCharacter() {
			pref.Element = 0.8;
			pref.Phantom = 0.6;
			pref.ElementFire = 0.8;
			pref.PhantomHuman = 0.6;
			majors.push_back(ElementFire);
			majors.push_back(PhantomHuman);
		}
	};

}
