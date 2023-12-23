#pragma once
#include "PCH.h"

namespace Utility
{
	//common
	inline REL::Relocation<__int64 (*)(double, char*, ...)> ExecuteCommand{ REL::ID(166307) };  // From Console-Command-Runner-SF

	void Notification(std::string message);
	void Notification(std::string message, bool flg);
	void Info(std::string message);
	void Debug(std::string message);
	void Error(std::string message);
	void Notify(std::string message);
	void SetLogLevel(int level);

	void        ExecuteCommandString(std::string command, std::string displayPrefix = "  ConsoleCommand: ");
	void        ExecuteCommandStringOnFormID(int formID, std::string subCommand);
	std::string num2hex(uint32_t formID, bool prefixOn = true, bool paddingOn = false);
	uint32_t    hex2num(std::string formID);
	std::string ToLower(std::string str);
	std::string GetFormIDsFromVector(std::vector<int> list, std::string separator, bool quoteOn, bool hexOn);

	//common
	bool                InGameScene(bool withOutMenu = false);
	std::vector<int>    DecomposeSlot(uint32_t value);
	bool                HasSlot(uint32_t value, int slot);
	int                 GetModIndex(int formID, std::string name);
	RE::BGSKeyword*     GetKeywordFromString(std::string editorID);
	RE::BGSKeyword*     GetKeywordFromID(int formID, int modIndex);
	RE::TESBoundObject* GetArmorFromStringAsBoundObject(std::string editorID);
	RE::TESObjectARMO*  GetArmorFromString(std::string editorID);
	RE::TESBoundObject* GetArmorFromID(int formID, int modIndex);
	RE::TESObjectMISC*  GetMiscFromString(std::string editorID);
	RE::TESObjectMISC*  GetMiscFromID(int formID, int modIndex);
	RE::TESObjectWEAP*  GetWeaponFromString(std::string editorID);
	RE::TESBoundObject* GetWeaponFromID(int formID, int modIndex);
	bool                IsMenuOpen(std::string MenuName, bool DialogOnly = true);
	bool                IsMenuOthersOpen();
	bool                IsMenuForSystemOpen();
	bool                IsMenuForTradeOpen();
	bool                IsMenuForTerminalOpen();
	bool                IsMenuInGameOpen();
	bool                HasKeyword(RE::TESObjectARMO* armor, std::string keyword);
	bool                HasKeyword(RE::TESBoundObject* item, std::string keyword);
	bool                HasKeyword(RE::TESObjectWEAP* weapon, std::string keyword);
	bool                HasKeyword(RE::TESObjectMISC* item, std::string keyword);
	bool                HasKeyword(RE::TESObjectREFR* member, std::string keyword);

	std::unordered_map<RE::TESBoundObject*, int>                CollectInventoryItems(RE::TESObjectREFR* actor, std::string itemType);
	std::unordered_map<RE::TESObjectARMO*, int>                 CollectInventoryArmors(RE::TESObjectREFR* actor);
	std::unordered_map<RE::TESObjectWEAP*, int>                 CollectInventoryWeapons(RE::TESObjectREFR* actor);
	std::unordered_map<RE::TESObjectMISC*, int>                 CollectInventoryMiscItems(RE::TESObjectREFR* actor);
	std::unordered_map<RE::TESBoundObject*, int>                CollectEquipItems(RE::TESObjectREFR* actor, std::string itemType);
	std::unordered_map<RE::TESObjectARMO*, int>                 CollectEquipArmors(RE::TESObjectREFR* actor);
	std::vector<RE::TESObjectARMO*>                             CollectEquipArmorsWithoutCount(RE::TESObjectREFR* actor);
	int                                                         GetEquipmentStackCount(const RE::BGSInventoryItem& item);
	RE::TESObjectARMO*                                          BoundObjectToArmor(RE::TESBoundObject* item);
	RE::TESObjectMISC*                                          BoundObjectToMisc(RE::TESBoundObject* item);
	RE::TESObjectWEAP*                                          BoundObjectToWeapon(RE::TESBoundObject* item);

	void AddItem(RE::TESObjectREFR* member, int itemFormID, int num);
	void AddItem(int actorFormID, int itemFormID, int num);
	void RemoveItem(RE::TESObjectREFR* member, int itemFormID, int num);
	void RemoveItem(int actorFormID, int itemFormID, int num);
	void UnequipItem(RE::TESObjectREFR* member, int itemFormID);
	void UnequipItem(int actorFormID, int itemFormID);
	void EquipItem(RE::TESObjectREFR* member, int itemFormID);
	void EquipItem(int actorFormID, int itemFormID);
	int  GetItemCount(RE::TESObjectREFR* member, int itemFormID, std::string itemType = "All");
	int  GetItemCount(int actorFormID, int itemFormID, std::string itemType = "ALL");
	int  GetPerkLevel(RE::TESObjectREFR* member, int formID);

	std::string                                                 GetArmorType(RE::TESObjectARMO* armor);
	inline std::unordered_map<RE::TESBoundObject*, int>         ItemForScanner;
	inline std::unordered_map<RE::TESBoundObject*, std::string> ItemTypesForScanner;
	inline std::vector<RE::TESObjectARMO*>                      ArmorVector;
	inline std::unordered_map<RE::TESObjectARMO*, int>          ArmorForScanner;
	inline std::unordered_map<RE::TESObjectWEAP*, int>          WeaponForScanner;
	inline std::unordered_map<RE::TESObjectMISC*, int>          MiscForScanner;
	inline std::string                                          ItemType;
	inline int                                                  LogLevel = 0;
}