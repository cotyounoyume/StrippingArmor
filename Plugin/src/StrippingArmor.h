#pragma once
#include "Common.h"
#include "Config.h"
#include "Event.h"

namespace StrippingArmor
{
	void                                                        MainLoop();
	int                                                         GetStrippingArmorIndex();
	//RE::TESObjectMISC*                                          GetMiscFromString(std::string name);
	void                                                        ResetParameter();
	void                                                        UpdateCrosshairTarget();
	void                                                        StateSelector();
	bool                                                        IsTargetValid(bool isLastTarget);
	bool                                                        IsTargetDead(bool isLastTarget);
	void                                                        State_Common(bool targetOn, bool crosshairOn);
	void                                                        State_Dialogue();
	void                                                        State_Pickpocket();
	void                                                        State_DebugToSameCell();
	void                                                        StateTargetOffCrosshairOff();
	void                                                        StateTargetOnCrosshairOff_CommonRoute();
	void                                                        StateTargetOnCrosshairOff_LivingRoute();
	void                                                        StateTargetOnCrosshairOn_LivingRoute();
	void                                                        StateTargetOffCrosshairOn_Common();
	void                                                        StateTargetOffCrosshairOn_LivingRoute();
	void                                                        StateTargetOnCrosshairOff_CorpseRoute();
	void                                                        StateTargetOnCrosshairOn_CorpseRoute();
	void                                                        StateTargetOffCrosshairOn_CorpseRoute();
	void                                                        MemberChecker();
	void                                                        MemberCheckerCandidate();
	void                                                        MemberCheckerCorpse();
	void                                                        PrepareForCorpseLooted();

	bool                                                        MCHasKeyword(RE::TESObjectREFR* object, std::string keywordString);
	void                                                        ReadyForLoot2(RE::TESObjectREFR* actor);
	void                                                        LootAndRemove();
	bool                                                        IsTapped();
	bool                                                        IsReady();
	bool                                                        IsLooted();
	void                                                        AddKeywordForCandidates(RE::TESObjectREFR* member, bool byKey, bool isForcedByDebugging = false);
	void                                                        AddKeywordForCorpses(RE::TESObjectREFR* member);
	bool                                                        IsReady(RE::TESObjectREFR* member);
	bool                                                        IsReadyForCorpse(RE::TESObjectREFR* member);
	bool                                                        CheckByKey(RE::TESObjectREFR* member);
	bool                                                        ShouldShowSpaceSuit(RE::TESObjectREFR* member);
	bool                                                        CheckConditionOK(RE::TESObjectREFR* member);
	void                                                        StrippingArmorWithKeyword(RE::TESObjectREFR* member);
	int                                                         GetHighOrLow(RE::TESObjectREFR* member);
	void                                                        ChangingCorpseWithKeyword(RE::TESObjectREFR* member);
	void                                                        RemoveCandidateKeywords(RE::TESObjectREFR* member);
	void                                                        RemoveCorpseKeywords(RE::TESObjectREFR* member);
	void                                                        DropEquipItems(RE::TESObjectREFR* member);
	void                                                        RemoveEquipItems(bool leftOne);
	bool                                                        NeedDummysuit(RE::TESObjectREFR* member);
	void                                                        EquipDummysuit(RE::TESObjectREFR* member);
	void                                                        DoEffectShader(RE::TESObjectREFR* member);
	RE::TESObjectARMO*                                          GetCorpsesuit(RE::TESObjectREFR* member);
	int                                                         GetArmorClothCombination(RE::TESObjectREFR* member);
	bool                                                        IsKeyPressed();
	void                                                        ChangingCorpse2(RE::TESObjectREFR* targetActor, int HighOrLow);
	RE::TESObjectARMO*                                          GetArmor(std::string editorID);
	RE::BGSKeyword*                                             GetKeyword(std::string editorID);
	void                                                        MakeKeywordMapIfNeeded();
	int                                                         GetEquipmentStackCount(const RE::BGSInventoryItem& item);
	std::unordered_map<RE::TESObjectARMO*, std::string>         GetArmorTypes(RE::TESObjectREFR* actor);
	std::unordered_map<std::string, RE::TESObjectARMO*>         GetArmorTypesInverse(RE::TESObjectREFR* actor);
	std::vector<RE::TESObjectARMO*>                             GetLootedArmors(RE::TESObjectREFR* actor);
	bool                                                        IsDummySuits(RE::TESObjectARMO* item);
	bool                                                        IsPickpocketItems(RE::TESBoundObject* item);
	bool                                                        HasDummySuits(RE::TESObjectREFR* member);
	bool                                                        HasPickpocketItems(RE::TESObjectREFR* member);
	std::unordered_map<std::string, RE::TESObjectWEAP*>         GetPickpocketFlagItems(RE::TESObjectREFR* member);
	void                                                        AddPickpocketItems(RE::TESObjectREFR* member);
	void                                                        MemorizeArmorType(RE::TESObjectREFR* member);
	void                                                        UnmemorizeArmorType(RE::TESObjectREFR* member);
	void                                                        CheckPickpocket(RE::TESObjectREFR* member);
	void                                                        AddKeyword(RE::TESObjectREFR* member, std::string keyword);
	void                                                        RemoveKeyword(RE::TESObjectREFR* member, std::string keyword);
	void                                                        SubKeyword(RE::TESObjectREFR* member, std::string keyword, std::string scriptname);
	bool                                                        AllowTypeByTheftLevel(std::string type, int level);

	inline int                                                  StrippingArmorIndex = 0;
	inline int                                                  FormIDForModIndex = 0x827;
	inline std::string                                          NameForModIndex = "SACandidateCheckReady";
	inline std::unordered_map<std::string, RE::BGSKeyword*>     KeywordMap;
	inline bool                                                 EffectON = false;
	inline bool                                                 NeedReset = false;
	inline RE::TESObjectREFR*                                   target;
	inline RE::TESObjectREFR*                                   LastTarget;
	inline bool                                                 crosshairrefOn = false;
	inline int                                                  RepickTimer = 5;
	inline std::unordered_map<RE::TESObjectREFR*, bool>         Candidates;
	inline std::unordered_map<RE::TESObjectREFR*, bool>         Corpses;
	inline std::unordered_map<RE::TESObjectREFR*, bool>         ShouldShowSpacesuitMap;
	inline int                                                  WaitCount = 0;
	inline int                                                  WaitCountPlus = 10;
	inline std::unordered_map<RE::TESObjectREFR*, bool>         ReadyStateMap;
	inline std::unordered_map<RE::TESObjectREFR*, int>          ArmorClothCombinationMap;
	inline std::unordered_map<RE::TESObjectREFR*, bool>         LootedCorpseMap;
	inline std::unordered_map<RE::TESObjectREFR*, bool>         StrippingKeyTappedMap;
	inline std::unordered_map<std::string, std::string>         Misc2PartMap = {
        { "Dummy_Cloth_for_PickPocket", "Cloth" },
        { "Dummy_Hat_for_PickPocket", "Hat" },
        { "Dummy_Spacesuit_for_PickPocket", "Spacesuit" },
        { "Dummy_Helmet_for_PickPocket", "Helmet" },
        { "Dummy_Backpack_for_PickPocket", "Backpack" },
	};
	inline std::unordered_map<std::string, std::string> Part2MiscMap = {
		{ "Cloth"    , "Dummy_Cloth_for_PickPocket" },
		{ "Hat"      , "Dummy_Hat_for_PickPocket" },
		{ "Spacesuit", "Dummy_Spacesuit_for_PickPocket" },
		{ "Helmet"   , "Dummy_Helmet_for_PickPocket" },
		{ "Backpack" , "Dummy_Backpack_for_PickPocket" },
	};
	inline std::vector<std::string> PartList = {
		"Cloth",
		"Hat",
		"Spacesuit",
		"Helmet",
		"Backpack"
	};
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
	};
}