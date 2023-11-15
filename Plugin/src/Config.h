#pragma once
#include "Utility.h"

namespace Config
{
	inline std::string StrippingKey = "T";
	inline bool        EffectOn = true;
	inline bool        EffectForCorpseOn = true;
	inline std::string EffectFormID = "0002e54d";
	inline bool        AlternativeClothOn = false;
	inline bool        UseStrippingKeyToCorpse = false;
	inline bool        ChangingAppearanceOfCorpse = true;
	inline int         CorpseTimer = 5;
	inline int         TimePerFrame = 50;

	void        ReadIni();
	std::string GetStrippingKey();
	char        GetStrippingKeyNumber();
	bool        GetEffectEnabled();
	bool        GetEffectForCorpseEnabled();
	std::string GetEffectFormID();
	bool        GetAlternativeClothEnabled();
	bool        GetChangingAppearanceOfCorpseEnabled();
	int         GetCorpseTimer();
	bool        GetUseStrippingKeyToCorpse();
	int         GetTimePerFrame();
}