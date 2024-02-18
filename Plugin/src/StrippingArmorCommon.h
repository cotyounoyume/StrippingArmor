#pragma once
#include "Common.h"
#include "Config.h"
#include "Event.h"

namespace StrippingArmorCommon
{

	bool IsDummySuits(RE::TESObjectARMO* item, bool forCorpse = false);
	bool HasDummySuits(RE::TESObjectREFR* member, bool forCorpse = false);
	bool IsAltSuits(RE::TESObjectARMO* item);
	bool HasAltSuits(RE::TESObjectREFR* member);
	bool MCHasKeyword(RE::TESObjectREFR* object, std::string keywordString);

	RE::BGSKeyword*    GetKeyword(std::string editorID);
	void               MakeKeywordMapIfNeeded();

	RE::TESObjectCELL* GetCurrentCell();
	void               SetPlayerIfNeeded();
	std::unordered_map<RE::TESObjectREFR*, bool> CollectRefsInCell();

	void AddKeyword(RE::TESObjectREFR* member, std::string keyword);
	void RemoveKeyword(RE::TESObjectREFR* member, std::string keyword);
	void SubKeyword(RE::TESObjectREFR* member, std::string keyword, std::string scriptname);

	bool IsSuit(RE::TESObjectREFR* member);
	std::unordered_map<std::string, bool> GetKeywordMapFromArmor(RE::TESObjectREFR* member);
	std::unordered_map<std::string, bool> GetKeywordMapFromActor(RE::TESObjectREFR* member);
	RE::TESObjectARMO*                    GetArmorFromKeywordMap(std::unordered_map<std::string, bool> keywords, bool isSuit);


	bool IsMemorizedArmor(RE::TESObjectREFR* member);
	void MemorizeArmorType(RE::TESObjectREFR* member);
	void UnmemorizeArmorType(RE::TESObjectREFR* member);
	int  GetArmorClothCombination(RE::TESObjectREFR* member);
	bool ShouldShowSpaceSuit(RE::TESObjectREFR* member);
	bool               NeedDummysuit(RE::TESObjectREFR* member);
	void               EquipDummysuit(RE::TESObjectREFR* member);
	void               EquipToggleSuit(RE::TESObjectREFR* member, bool isSuit);
	void               DoEffectShader(RE::TESObjectREFR* member);

	RE::TESObjectARMO* GetArmor(std::string editorID);
	void               ToggleBodyRoute(RE::TESObjectREFR* member);
	bool               IsKeyPressed();
	bool               IsToggleKeyPressed();
	bool               HasArmorsForLoot(RE::TESObjectREFR* actor);
	void               RemoveCandidateKeywords(RE::TESObjectREFR* member);



	inline RE::TESObjectREFR*                               Player;
	inline RE::Actor*                                       PlayerActor;
	inline std::unordered_map<std::string, RE::BGSKeyword*> KeywordMap;
	inline std::unordered_map<RE::TESObjectREFR*, int>      ArmorClothCombinationMap;
	inline std::unordered_map<RE::TESObjectREFR*, bool>     ShouldShowSpacesuitMap;

	inline int                                              WaitCount = 0;
	inline int                                              WaitCountPlus = 10;

	inline std::vector<std::string> KeywordList = {
		"SACandidateCheckReady",
		"SACorpseCheckReady",
		"SADontStripThis",
		"SANeedDummysuit",
		"SAConditionNG",
		"SAConditionOK",
		"SADetailSleeping",
		"SADetailUnconscious",
		"SADetailBleedingOut",
		"SADetailCommanded",
		"SADetailEtc",
		"SATemparetureNormal",
		"SATemparetureLow",
		"SATemparetureHigh",
		"SACorpseNormal",
		"SACorpseFrozen",
		"SACorpseDusty",
		"SABreathableNG",
		"SABreathableOK",
		"SACandidateCheckByKey",
		"SACandidateCheckByLoot",
		"SAConditionSealed",
		"SAConditionShouldShowSuit",
		"SAPickpocketCloth",
		"SAPickpocketHat",
		"SAPickpocketSpacesuit",
		"SAPickpocketHelmet",
		"SAPickpocketBackpack",
		"SAPickpocketDone",
		"SAPickpocketStart",
		"SAThisIsForToggle",
		"SAAltClothes",
		"SAStateLooted",
		"SAStateCorpsed",
		"SAIAMSpeaker",
		"SAIAMNOTSpeaker",
	};

}