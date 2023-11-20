#pragma once
#include "PCH.h"
#include "Config.h"
#include "Utility.h"
#include "Event.h"

namespace StrippingArmor
{
	void MainLoop();

	bool IsKeyPressed();
	//void DoStrippingArmorPapyrusCommon(std::string scriptMethod, std::string type, RE::TESObjectREFR* targetActor);
	//void StrippingArmor(RE::TESObjectREFR* obj);
	//void StrippingArmorAlt(RE::TESObjectREFR* obj);
	void UpdateCrosshairTarget();
	void StateSelector();
	void MemberChecker();
	void State_Common(bool targetOn, bool crosshairOn);

	void StateTargetOffCrosshairOff();
	void LootAndRemove();
	void StateTargetOnCrosshairOn_CorpseRoute();
	void StateTargetOffCrosshairOn_CorpseRoute();
	void StateTargetOnCrosshairOff_CorpseRoute();
	void StateTargetOnCrosshairOn_LivingRoute();
	void StateTargetOffCrosshairOn_LivingRoute();
	void StateTargetOnCrosshairOff_LivingRoute();

	void MemberCheckerCandidate();
	void MemberCheckerCorpse();
	void StrippingArmorWithKeyword(RE::TESObjectREFR* member);
	void AddKeywordForCandidates(RE::TESObjectREFR* member, bool byKey);
	void RemoveCandidateKeywords(RE::TESObjectREFR* member);
	void EquipDummysuit(RE::TESObjectREFR* member);
	void ChangingCorpseWithKeyword(RE::TESObjectREFR* member);
	void RemoveCorpseKeywords(RE::TESObjectREFR* member);
	void ChangingCorpse2(RE::TESObjectREFR* targetActor, int HighOrLow);
	int  GetHighOrLow(RE::TESObjectREFR* member);
	RE::TESBoundObject* GetCorpsesuit(RE::TESObjectREFR* member);
	void                                                    DoEffectShader(RE::TESObjectREFR* member);

	inline std::unordered_map<std::string, RE::BGSKeyword*> KeywordMap;
	void                                                    MakeKeywordMapIfNeeded();
	RE::BGSKeyword*                                         GetKeyword(std::string editorID);

	bool               IsTargetValid(bool isLastTarget = false);
	bool               IsTargetDead(bool isLastTarget = false);
	void               RemoveEquipItems(bool leftOne = false);
	int                GetArmorClothCombination();
	void               DropEquipItems();
	//void               ChangingCorpse(RE::TESObjectREFR* obj);
	void               ResetParameter();
	//int                GetBitParams();
	void               State_Dialogue();
	bool               IsDialogOpen();

	inline bool               EffectON = false;
	inline bool               NeedReset = false;
	inline RE::TESObjectREFR* target;
	inline RE::TESObjectREFR* LastTarget;
	inline bool               crosshairrefOn = false;
	inline int                RepickTimer = 5;
	inline std::unordered_map<RE::TESObjectREFR*, bool> Candidates;
	inline std::unordered_map<RE::TESObjectREFR*, bool> Corpses;
	inline int WaitCount = 0;
	inline int WaitCountPlus = 10;

	inline std::unordered_map<RE::TESBoundObject*, std::string> ArmorTypesMap;
	inline std::unordered_map<RE::TESObjectREFR*, bool>         ReadyStateMap;
	inline std::unordered_map<RE::TESObjectREFR*, int>          ArmorClothCombinationMap;
	inline std::unordered_map<RE::TESObjectREFR*, bool>         LootedCorpseMap;
	inline std::unordered_map<RE::TESObjectREFR*, bool>         StrippingKeyTappedMap;
}