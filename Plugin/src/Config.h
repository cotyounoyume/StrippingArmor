#pragma once
#include "Common.h"

namespace Config
{
	inline bool        TraditionalLootingOnlyOn = true;
	inline bool        PerfectTouchOn = true;
	inline std::string StrippingKey = "T";
	inline bool        EffectOn = true;
	inline bool        StrippingKeyOn = true;
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
	//inline bool        EasyPickpocketOn = false;
	inline bool        DebugExecuteToAllActorsOn = true;
	inline bool        DebugExecuteToCrossRefActorForcedOn = true;
	inline int         PickpocketHat = 1;
	inline int         PickpocketHelmet = 1;
	inline int         PickpocketCloth = 2;
	inline int         PickpocketBackpack = 3;
	inline int         PickpocketSpacesuit = 4;
	


	void        ReadIni();
	std::string GetStrippingKey();
	char        GetStrippingKeyNumber();
	bool        GetEffectEnabled();
	bool        GetStrippingKeyOn();
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
	bool        GetPerfectTouchOn();
	int         GetPPHat();
	int         GetPPCloth();
	int         GetPPHelmet();
	int         GetPPBackpack();
	int         GetPPSpacesuit();
	int         GetPPLevel(std::string type);
	bool        GetDebugExecuteToAllActorsOn();
	bool        GetDebugExecuteToCrossRefActorForcedOn();
}