#pragma once

#include "PlayerCharacter.hpp"

// 在 TH06 东方红魔乡 中登场的人物
namespace Th06 {

	// 帕秋莉
	class PatchouliKnowldege :public PlayerCharacter {
	public:
		PatchouliKnowldege() :PlayerCharacter() {
			pref.Element = 0.8;
			pref.Phantom = 0.3;
			pref.ElementMetal = 0.8;
			pref.ElementWood = 0.8;
			pref.ElementWater = 0.8;
			pref.ElementFire = 0.75;  // 确保阿空的符卡（火0.8，日1.0）不能用
			pref.ElementSoil = 0.8;
			pref.ElementSun = 0.65;   // 确保阿空的符卡（火0.8，日1.0）不能用
			pref.ElementMoon = 0.6;   // 月都角色月属性普遍高于0.7
			pref.PhantomYoukai = 0.3; // 毕竟是魔法使

			majors.push_back(SevenElements); // 七曜的魔法使
			majors.push_back(PhantomYoukai); // 魔法使是妖怪
		}
	};

	// 咲夜
	class IzayoiSakuya :public PlayerCharacter {
	public:
		IzayoiSakuya() :PlayerCharacter() {
			pref.Element = 0.4;
			pref.Phantom = 0.6;
			pref.ElementMoon = 0.4;   // （月）月计时？
			pref.PhantomHuman = 0.6;  // 人类（咲夜的符卡月0.4人0.6，月都中月兔月曜妖灵，月都人纯月灵）

			majors.push_back(ElementMoon);
			majors.push_back(PhantomHuman);
		}
	};

	// 蕾米
	class RemiliaScarlet :public PlayerCharacter {
	public:
		RemiliaScarlet() :PlayerCharacter() {
			pref.Element = 0.6;
			pref.Phantom = 0.75;
			pref.ElementFire = 0.6;
			pref.PhantomYoukai = 0.75;

			majors.push_back(ElementFire);
			majors.push_back(PhantomYoukai);
		}
	};

	// 芙兰
	class FlandreScarlet :public PlayerCharacter {
	public:
		FlandreScarlet() :PlayerCharacter() {
			pref.Element = 0.5;
			pref.Phantom = 0.8;
			pref.ElementFire = 0.5;
			pref.PhantomYoukai = 0.8;

			majors.push_back(ElementFire);
			majors.push_back(PhantomYoukai);
		}
	};



}
