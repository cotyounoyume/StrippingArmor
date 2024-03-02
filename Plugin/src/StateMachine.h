#pragma once
#include "Common.h"
#include "Config.h"
#include "Event.h"
#include "StrippingArmorCommon.h"

namespace StateMachine
{
	enum class STAGE
	{
		kDefault,
		kDead,
		kLootCandidate,
		kLootWaiting,
		kLooted,
		kCorpseCandidate,
		kCorpsed,
		kCorpsedAndNaked,
		kError,
		kRemove,
	};

	bool IsListed(RE::TESObjectREFR* member);
	void ResetLootStageMap();
	STAGE              GetStage(RE::TESObjectREFR* member, bool fromMap = true);
	void               SetStage(RE::TESObjectREFR* member, STAGE stage);

	STAGE              GetStageFromKeyword(RE::TESObjectREFR* member);
	STAGE              GetStageFromArmor(RE::TESObjectREFR* member);
	STAGE              GetStageFromDetails(RE::TESObjectREFR* member);
	std::vector<RE::TESObjectREFR*> GetTargetBodies(STAGE stage);
	std::vector<RE::TESObjectREFR*> GetTargetBodiesAll();
	void                            ResetStage(RE::TESObjectREFR* member);
	bool               CheckCandidateCondition(RE::TESObjectREFR* member);
	void                            UnList(RE::TESObjectREFR* member);
	void                                                 RemoveUnnecessaryMember();

	void AddToListForPickpocketTarget(RE::TESObjectREFR* member);
	void RemoveFromListForPickpocketTarget(RE::TESObjectREFR* member);
	bool               IsListedInPickpocketTarget(RE::TESObjectREFR* member);
	void ClearListForPickpocketTarget();
	std::vector<RE::TESObjectREFR*> GetPickpocketTargetList();

	void                            AddToListForKeytap(RE::TESObjectREFR* member, bool inDialog);
	void                            RemoveFromListForKeytap(RE::TESObjectREFR* member);
	bool                            IsListedInForKeytap(RE::TESObjectREFR* member, bool inDialog);
	void                            ClearListForForKeytap();
	std::vector<RE::TESObjectREFR*> GetForKeytapList(bool inDialog);
	void                            ForDebugGetMember(bool inDialog);

	inline std::unordered_map<RE::TESObjectREFR*, STAGE> LootStageMap;
	inline std::unordered_map<RE::TESObjectREFR*, bool>  PickpocketMap;
	inline std::unordered_map<RE::TESObjectREFR*, bool>  KeytappedMap;
	inline std::unordered_map<RE::TESObjectREFR*, bool>  DialogMap;

}