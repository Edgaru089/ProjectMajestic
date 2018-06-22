#pragma once

#include "Main.hpp"
#include "PlayerEntity.hpp"


class PlayerCharacter {
public:

	PlayerCharacter() :pref(.0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0) {}

	enum MagicFactor {
		ElementMetal,  // 金曜
		ElementWood,   // 木曜
		ElementWater,  // 水曜
		ElementFire,   // 火曜
		ElementSoil,   // 土曜
		ElementSun,    // 日曜
		ElementMoon,   // 月曜
		PhantomHuman,  // 人灵
		PhantomYoukai, // 妖灵
		PhantomDivine, // 神灵

		// 以下五种中最多只能选有一种且必须放在第一个
		// 纯灵属性不能和其他任何属性并存（纯月曜可以）
		SevenElements,     // 七曜
		PureElementMoon,   // 纯月曜（用于月都人物）
		PurePhantomHuman,  // 纯人灵
		PurePhantomYoukai, // 纯妖灵
		PurePhantomDivine  // 纯神灵
	};

	struct MagicPreference {
		MagicPreference() {}
		MagicPreference(double Element, double Phantom, double ElementMetal, double ElementWood, double ElementWater, double ElementFire, double ElementSoil, double ElementSun, double ElementMoon, double PhantomHuman, double PhantomYoukai, double PhantomDivine) :
			Element(Element), Phantom(Phantom), ElementMetal(ElementMetal), ElementWood(ElementWood), ElementWater(ElementWater), ElementFire(ElementFire), ElementSoil(ElementSoil), ElementSun(ElementSun), ElementMoon(ElementMoon), PhantomHuman(PhantomHuman), PhantomYoukai(PhantomYoukai), PhantomDivine(PhantomDivine) {}

		// All prefence factors are ranged in [0, 1]
		double Element;  // 七曜
		double Phantom;  // 三灵

		double ElementMetal; // 金曜
		double ElementWood;  // 木曜
		double ElementWater; // 水曜
		double ElementFire;  // 火曜
		double ElementSoil;  // 土曜
		double ElementSun;   // 日曜
		double ElementMoon;

		double PhantomHuman;  // 人灵
		double PhantomYoukai; // 妖灵
		double PhantomDivine; // 神灵
	};

	virtual const string getCharacterId() = 0;
	const MagicPreference getMagicPreference() { return pref; }

	const vector<MagicFactor>& getMajorMagicFactors() { return majors; }

protected:

	MagicPreference pref;
	vector<MagicFactor> majors;

};
