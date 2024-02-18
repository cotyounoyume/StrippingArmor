#pragma once
#include "Common.h"
#include "Config.h"
#include "Event.h"
#include "StateMachine.h"
#include "StrippingArmorCommon.h"

namespace StrippingArmorPickpocket
{

	void State_Pickpocket();
	bool                                                IsPickpocketItems(RE::TESBoundObject* item);
	bool                                                HasPickpocketItems(RE::TESObjectREFR* member);
	std::unordered_map<std::string, RE::TESObjectWEAP*> GetPickpocketFlagItems(RE::TESObjectREFR* member);
	void                                                AddPickpocketItems(RE::TESObjectREFR* member);
	void                                                CheckPickpocket(RE::TESObjectREFR* member);
	bool                                                AllowTypeByTheftLevel(std::string type, int level);
	void                                                RemovePickpocketKeywords(RE::TESObjectREFR* member);
	void                                                HouseKeepPickpocket();
	std::unordered_map<RE::TESObjectARMO*, std::string> GetArmorTypes(RE::TESObjectREFR* actor);
	std::unordered_map<std::string, RE::TESObjectARMO*> GetArmorTypesInverse(RE::TESObjectREFR* actor);

	inline std::unordered_map<std::string, std::string> Misc2PartMap = {
		{ "Dummy_Cloth_for_PickPocket", "Cloth" },
		{ "Dummy_Hat_for_PickPocket", "Hat" },
		{ "Dummy_Spacesuit_for_PickPocket", "Spacesuit" },
		{ "Dummy_Helmet_for_PickPocket", "Helmet" },
		{ "Dummy_Backpack_for_PickPocket", "Backpack" },
	};
	inline std::unordered_map<std::string, std::string> Part2MiscMap = {
		{ "Cloth", "Dummy_Cloth_for_PickPocket" },
		{ "Hat", "Dummy_Hat_for_PickPocket" },
		{ "Spacesuit", "Dummy_Spacesuit_for_PickPocket" },
		{ "Helmet", "Dummy_Helmet_for_PickPocket" },
		{ "Backpack", "Dummy_Backpack_for_PickPocket" },
	};
	inline std::vector<std::string> PartList = {
		"Cloth",
		"Hat",
		"Spacesuit",
		"Helmet",
		"Backpack"
	};

}
