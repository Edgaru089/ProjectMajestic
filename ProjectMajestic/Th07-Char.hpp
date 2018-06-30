#pragma once

#include "PlayerCharacter.hpp"

// �� TH07 ���������� �еǳ�������
namespace Th07 {

	// ����˿ 
	class AliceMargatroid :public PlayerCharacter {
	public:
		AliceMargatroid() :PlayerCharacter() {
			pref.Element = 0.7;       // �������Ա���������һЩ������˿�ķ�����������ã���˫����Ԥ���������˲�վ˫�߰���
			pref.Phantom = 0.4;       // ���������������Ը�һЩ
			pref.ElementMetal = 0.7;
			pref.ElementWood = 0.7;
			pref.ElementWater = 0.7;
			pref.ElementFire = 0.65;  // ȷ�����յķ�������0.8����1.0��������
			pref.ElementSoil = 0.7;
			pref.ElementSun = 0.6;    // ȷ�����յķ�������0.8����1.0��������
			pref.ElementMoon = 0.6;   // �¶���ɫ�������ձ����0.7
			pref.PhantomYoukai = 0.4; // �Ͼ���ħ��ʹ

			majors.push_back(SevenElements); // ��ɫ����żʹ
			majors.push_back(PhantomYoukai); // ħ��ʹ������
		}
	};

	// ����
	class KonpakuYoumu :public PlayerCharacter {
	public:
		KonpakuYoumu() :PlayerCharacter() {
			pref.Element = 0.5;
			pref.Phantom = 0.6;
			pref.ElementMetal = 0.5;  // ��¥��¥�۽�->�����ԣ�
			pref.PhantomHuman = 0.4;  // ���˰���
			pref.PhantomYoukai = 0.4;
			majors.push_back(ElementMetal);
			majors.push_back(PhantomHuman);
			majors.push_back(PhantomYoukai);
		}
	};

	// ������
	class SaigyoujiYuyuko :public PlayerCharacter {
	public:
		SaigyoujiYuyuko() :PlayerCharacter() {
			pref.Phantom = 1.0;
			pref.PhantomYoukai = 1.0;
			majors.push_back(PurePhantomYoukai);
		}
	};

	// ������
	class YakumoYukari :public PlayerCharacter {
	public:
		YakumoYukari() :PlayerCharacter() {
			pref.Phantom = 1.0;
			pref.PhantomYoukai = 1.0;
			majors.push_back(PurePhantomYoukai);
		}
	};
}
