#pragma once

#include "Main.hpp"
#include "PlayerEntity.hpp"


struct MagicPreference {
	MagicPreference() {}
	MagicPreference(double Element, double Phantom, double ElementMetal, double ElementWood, double ElementWater, double ElementFire, double ElementSoil, double ElementSun, double ElementMoon, double PhantomHuman, double PhantomYoukai, double PhantomDivine) :
		Element(Element), Phantom(Phantom), ElementMetal(ElementMetal), ElementWood(ElementWood), ElementWater(ElementWater), ElementFire(ElementFire), ElementSoil(ElementSoil), ElementSun(ElementSun), ElementMoon(ElementMoon), PhantomHuman(PhantomHuman), PhantomYoukai(PhantomYoukai), PhantomDivine(PhantomDivine) {}

	// All prefence factors are ranged in [0, 1]
	double Element;  // ����
	double Phantom;  // ����

	double ElementMetal; // ����
	double ElementWood;  // ľ��
	double ElementWater; // ˮ��
	double ElementFire;  // ����
	double ElementSoil;  // ����
	double ElementSun;   // ����
	double ElementMoon;  // ����

	double PhantomHuman;  // ����
	double PhantomYoukai; // ����
	double PhantomDivine; // ����

	bool isControllable(const MagicPreference& controller) const {
		if (controller.Element >= Element &&
			controller.Phantom >= Element &&
			controller.ElementMetal >= ElementMetal &&
			controller.ElementWood >= ElementWood &&
			controller.ElementWater >= ElementWater &&
			controller.ElementFire >= ElementFire &&
			controller.ElementSoil >= ElementSoil &&
			controller.PhantomHuman >= PhantomHuman &&
			controller.PhantomYoukai >= PhantomYoukai &&
			controller.PhantomDivine >= PhantomDivine)
			return true;
		else
			return false;
	}
};


class PlayerCharacter {
public:

	PlayerCharacter() :pref(.0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0) {}
	virtual ~PlayerCharacter() {}

	enum MagicFactor {
		ElementMetal,  // ����
		ElementWood,   // ľ��
		ElementWater,  // ˮ��
		ElementFire,   // ����
		ElementSoil,   // ����
		ElementSun,    // ����
		ElementMoon,   // ����
		PhantomHuman,  // ����
		PhantomYoukai, // ����
		PhantomDivine, // ����

		// �������������ֻ��ѡ��һ���ұ�����ڵ�һ��
		// �������Բ��ܺ������κ����Բ��棨�����׿��ԣ�
		SevenElements,     // ����
		PureElementMoon,   // �����ף������¶����
		PurePhantomHuman,  // ������
		PurePhantomYoukai, // ������
		PurePhantomDivine  // ������
	};

	virtual const string getCharacterId() = 0;
	const MagicPreference getMagicPreference() { return pref; }

	const vector<MagicFactor>& getMajorMagicFactors() { return majors; }

protected:

	MagicPreference pref;
	vector<MagicFactor> majors;

};


PlayerCharacter* localCharacter;
