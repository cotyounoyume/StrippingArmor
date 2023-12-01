#pragma once
#include "PCH.h"

namespace Utility
{
	//common
	inline REL::Relocation<__int64 (*)(double, char*, ...)> ExecuteCommand{ REL::ID(166307) };  // From Console-Command-Runner-SF

	void Notification(std::string message);
	void Notification(std::string message, bool flg);
	void Info(std::string message);
	void Notify(std::string message);

	void        ExecuteCommandString(std::string command, std::string displayPrefix = "  ConsoleCommand: ");
	void        ExecuteCommandStringOnFormID(int formID, std::string subCommand);
	std::string num2hex(uint32_t formID, bool prefixOn = true, bool paddingOn = false);
	uint32_t    hex2num(std::string formID);
	std::string ToLower(std::string str);
	std::string GetFormIDsFromVector(std::vector<int> list, std::string separator, bool quoteOn, bool hexOn);

	//common
	bool                InGameScene();
	std::vector<int>    DecomposeSlot(uint32_t value);
	bool                HasSlot(uint32_t value, int slot);
	int                 GetModIndex(int formID, std::string name);
	RE::BGSKeyword*     GetKeywordFromString(std::string editorID);
	RE::BGSKeyword*     GetKeywordFromID(int formID, int modIndex);
	RE::TESBoundObject* GetArmorFromString(std::string editorID);
	RE::TESBoundObject* GetArmorFromID(int formID, int modIndex);
	bool                IsMenuOpen(std::string MenuName, bool DialogOnly = true);
	bool                IsMenuOthersOpen();
	bool                IsMenuForSystemOpen();
	bool                IsMenuForTradeOpen();
	bool                IsMenuForTerminalOpen();
	bool                IsMenuInGameOpen();

	std::unordered_map<RE::TESBoundObject*, int> CollectInventoryItems(RE::TESObjectREFR* actor, std::string itemType);
	std::unordered_map<RE::TESBoundObject*, int> CollectEquipItems(RE::TESObjectREFR* actor, std::string itemType);
	int                                          GetEquipmentStackCount(const RE::BGSInventoryItem& item);

	inline std::unordered_map<RE::TESBoundObject*, int>         ItemForScanner;
	inline std::unordered_map<RE::TESBoundObject*, std::string> ItemTypesForScanner;
	inline std::string                                          ItemType;
}