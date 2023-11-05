#pragma once
#include "PCH.h"

namespace Utility
{
	inline REL::Relocation<__int64 (*)(double, char*, ...)> ExecuteCommand{ REL::ID(166307) };  // From Console-Command-Runner-SF
	void                                                    ExecuteCommandString(std::string command, std::string displayPrefix = "  ConsoleCommand: ");
	void                                                    ExecuteCommandStringOnFormID(int formID, std::string subCommand);

	std::string                                             num2hex(uint32_t formID);
	bool                                                    InGameScene();

	void Notification(std::string message);

	std::vector<int> DecomposeSlot(uint32_t value);
	bool             HasSlot(uint32_t value, int slot);
	void             ReadyForLoot(RE::TESObjectREFR* actor);
	void             ReadyForLoot2(RE::TESObjectREFR* actor);
	void             PrintArmorStacks(RE::TESObjectREFR* actor, bool isEquipped = true);
	void             PrintArmorStacks2(RE::TESObjectREFR* actor);

	std::vector<int>                             GetArmorFormIDs(RE::TESObjectREFR* actor, bool isEquipped = true);
	int                                                 GetEquipmentStackCount(const RE::BGSInventoryItem& item);
	std::unordered_map<int, RE::TESBoundObject*>        GetArmorFormIDPairs(RE::TESObjectREFR* actor, bool isEquipped = true);
	std::vector<int>                                    GetItemFormIDs(RE::TESObjectREFR* actor);
	std::unordered_map<int, bool>                       Vector2Map(std::vector<int> vector);
	inline RE::TESObjectREFR*                           CurrentActor;
	inline std::vector<int>                             SlotList;
	inline std::vector<RE::TESBoundObject*>             ArmorList;
	std::vector<RE::TESBoundObject*>                    GetLootedArmors(RE::TESObjectREFR* actor);
	inline int                                                 debugCount;
	inline std::vector<int>                             ItemList;
	inline std::unordered_map<int, RE::TESBoundObject*> SlotPair;
	std::string             GetFormIDsFromVector(std::vector<int> list, std::string separator, bool quoteOn, bool hexOn);

	inline std::unordered_map<RE::TESBoundObject*, int>   EquipmentPair;
	std::unordered_map<RE::TESBoundObject*, int> GetEquipmentCountPair(RE::TESObjectREFR* actor);

}