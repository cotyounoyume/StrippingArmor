#pragma once
#include "Common.h"

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
	inline bool        EnableDroppingItemsOn = true;
	inline int         RePickTimer = 5;
	inline bool        CanStealDroppedItemOn = false;
	inline bool        ConditionTalkingOn = true;
	inline bool        ConditionSleepingOn = true;
	inline bool        ConditionIsCommandedOn = true;
	inline bool        ConditionUnconsciousOn = true;
	inline bool        ConditionPickingPocketOn = true;
	inline bool        ConditionBleedingOutOn = true;



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
	int         GetRePickTimer();
	bool        GetEnableDroppingItemsOn();
	bool        GetCanStealDroppedItemOn();
	bool        GetConditionTalkingOn();
	bool        GetConditionSleepingOn();
	bool        GetConditionUnconsciousOn();
	bool        GetConditionIsCommandedOn();
	bool        GetConditionPickingPocketOn();
	bool        GetConditionBleedingOutOn();
}