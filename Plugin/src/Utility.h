#pragma once
#include "PCH.h"

namespace Utility
{
	inline REL::Relocation<__int64 (*)(double, char*, ...)> ExecuteCommand{ REL::ID(166307) };  // From Console-Command-Runner-SF
	void                                                    ExecuteCommandString(std::string command);
	std::string                                             num2hex(uint32_t formID);
	bool                                                    InGameScene();

	void Notification(std::string message);

	std::vector<int> DecomposeSlot(uint32_t value);
	bool             HasSlot(uint32_t value, int slot);

	void                                                ReadyForLoot(RE::TESObjectREFR* actor);
	std::vector<int>        GetArmorFormIDs(RE::TESObjectREFR* actor);
	std::unordered_map<int, RE::TESBoundObject*>        GetArmorFormIDPairs(RE::TESObjectREFR* actor);
	//std::vector<int>                                    GetItemFormIDs(RE::TESObjectREFR* actor);
	std::unordered_map<int, bool>                       Vector2Map(std::vector<int> vector);
	inline RE::TESObjectREFR*                           CurrentActor;
	inline std::vector<int>                             SlotList;
	//inline std::vector<int>                             ItemList;
	inline std::unordered_map<int, RE::TESBoundObject*> SlotPair;
	std::string             GetFormIDsFromVector(std::vector<int> list, std::string separator, bool quoteOn, bool hexOn);


}