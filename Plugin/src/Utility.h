#pragma once
#include "PCH.h"

namespace Utility
{
	//common
	inline REL::Relocation<__int64 (*)(double, char*, ...)> ExecuteCommand{ REL::ID(166307) };  // From Console-Command-Runner-SF

	void        Notification(std::string message);
	void        Notification(std::string message, bool flg);
	void        ExecuteCommandString(std::string command, std::string displayPrefix = "  ConsoleCommand: ");
	void ExecuteCommandStringOnFormID(int formID, std::string subCommand);
	std::string num2hex(uint32_t formID, bool prefixOn = true, bool paddingOn = false);
	uint32_t hex2num(std::string formID);

	//common
	bool InGameScene();
	std::vector<int> DecomposeSlot(uint32_t value);
	bool HasSlot(uint32_t value, int slot);
	RE::BGSKeyword*                                         GetKeywordFromString(std::string editorID); 
	RE::TESBoundObject*                                     GetArmorFromString(std::string editorID);

	//for Inventory Handling
	inline std::unordered_map<RE::TESBoundObject*, int> ItemForScanner;
	inline std::unordered_map<RE::TESBoundObject*, std::string>  ItemTypesForScanner;
	inline std::string                                   ItemType;
	int                                                 GetEquipmentStackCount(const RE::BGSInventoryItem& item);
	std::unordered_map<RE::TESBoundObject*, int>        CollectInventoryItems(RE::TESObjectREFR* actor, std::string itemType);
	std::unordered_map<RE::TESBoundObject*, int>                 CollectEquipItems(RE::TESObjectREFR* actor, std::string itemType);
	std::unordered_map<RE::TESBoundObject*, std::string> GetArmorTypes(RE::TESObjectREFR* actor);
	std::string                                                  GetArmorType(const RE::BGSInventoryItem& item);

	//for StrippingArmor
	void ReadyForLoot2(RE::TESObjectREFR* actor);
	void PrintArmorStacks2(RE::TESObjectREFR* actor);
	//void SurveyInventory(RE::TESObjectREFR* actor);

	bool                             HasDontStripKeyword(const RE::BGSInventoryItem& item);
	std::vector<RE::TESBoundObject*> GetLootedArmors(RE::TESObjectREFR* actor);
	std::string                      GetFormIDsFromVector(std::vector<int> list, std::string separator, bool quoteOn, bool hexOn);
	bool                             IsDummySuits(const RE::BGSInventoryItem& item);

	class NumAndType
	{
	public:
		int  num;
		std::string type;

		NumAndType(int _num, std::string _type);
	};
}