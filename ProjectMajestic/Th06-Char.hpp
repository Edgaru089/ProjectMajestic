#pragma once

#include "PlayerCharacter.hpp"

// �� TH06 ������ħ�� �еǳ�������
namespace Th06 {

	// ������
	class PatchouliKnowldege :public PlayerCharacter {
	public:
		PatchouliKnowldege() :PlayerCharacter() {
			pref.Element = 0.8;
			pref.Phantom = 0.3;
			pref.ElementMetal = 0.8;
			pref.ElementWood = 0.8;
			pref.ElementWater = 0.8;
			pref.ElementFire = 0.75;  // ȷ�����յķ�������0.8����1.0��������
			pref.ElementSoil = 0.8;
			pref.ElementSun = 0.65;   // ȷ�����յķ�������0.8����1.0��������
			pref.ElementMoon = 0.6;   // �¶���ɫ�������ձ����0.7
			pref.PhantomYoukai = 0.3; // �Ͼ���ħ��ʹ

			majors.push_back(SevenElements); // ���׵�ħ��ʹ
			majors.push_back(PhantomYoukai); // ħ��ʹ������
		}
	};

	// �Dҹ
	class IzayoiSakuya :public PlayerCharacter {
	public:
		IzayoiSakuya() :PlayerCharacter() {
			pref.Element = 0.4;
			pref.Phantom = 0.6;
			pref.ElementMoon = 0.4;   // ���£��¼�ʱ��
			pref.PhantomHuman = 0.6;  // ���ࣨ�Dҹ�ķ�����0.4��0.6���¶��������������飬�¶��˴����飩

			majors.push_back(ElementMoon);
			majors.push_back(PhantomHuman);
		}
	};

	// ����
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

	// ܽ��
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
