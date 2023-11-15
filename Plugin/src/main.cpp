#include "DKUtil/Hook.hpp"
#include "PCH.h"
#include "Utility.h"
#include "Config.h"
#include "Event.h"

// SFSE message listener, use this to do stuff at specific moments during runtime
void Listener(SFSE::MessagingInterface::Message* message) noexcept
{
	if (message->type == SFSE::MessagingInterface::kPostLoad) {
		RE::UI::GetSingleton()->RegisterSink(Events::EventHandlerForMenu::GetSingleton());
		//RE::UI::GetSingleton()->RegisterSink(Events::EventHandlerForContainer::GetSingleton());
	}
}

namespace Main
{
	REL::Relocation<__int64 (*)(double, char*, ...)> ExecuteCommand{ REL::ID(166307) };  // From Console-Command-Runner-SF

	bool        IsKeyPressed();
	void        StrippingArmor(RE::TESObjectREFR* obj);
	void        StrippingArmorAlt(RE::TESObjectREFR* obj, std::string itemFormID);
	void        UpdateCrosshairTarget();
	void        StateSelector();
	void        StateTargetOnCrosshairOn();
	void        StateTargetOnCrosshairOff();
	void        StateTargetOffCrosshairOn();
	void        StateTargetOffCrosshairOff();
	int         GetArmorClothCombination();
	void        ChangingCorpse(RE::TESObjectREFR* obj);
	void               ResetParameter();
	int                GetBitParams();
	void               RemoveEquipItems();
	bool        EffectON = false;
	bool               NeedReset = false;
	RE::TESObjectREFR* target;
	RE::TESObjectREFR* LastTarget;
	bool               crosshairrefOn = false;


	int WaitCount = 0;
	int WaitCountPlus = 10;

	std::unordered_map<RE::TESBoundObject*, std::string> ArmorTypesMap;
	std::unordered_map<RE::TESObjectREFR*, bool> ReadyStateMap;
	std::unordered_map<RE::TESObjectREFR*, int> ArmorClothCombinationMap;
	std::unordered_map<RE::TESObjectREFR*, bool>         LootedCorpseMap;
	std::unordered_map<RE::TESObjectREFR*, bool>         StrippingKeyTappedMap;

	static DWORD MainLoop(void* unused)
	{
		Utility::Notification("Main Start");
		Config::ReadIni();

		while (true) {
			Sleep(Config::GetTimePerFrame());
			if (!Utility::InGameScene())
				continue;
			UpdateCrosshairTarget();
			StateSelector();
		}

		return 0;
	}

	void UpdateCrosshairTarget()
	{
		auto tmpTarget = RE::PlayerCharacter::GetSingleton()->crosshairRef;
		if (tmpTarget != nullptr && tmpTarget->IsActor())
			target = tmpTarget;
		else
			target = nullptr;
	}

	void ResetParameter()
	{
		Events::NeedReset = false;
		target = nullptr;
		LastTarget = nullptr;
		crosshairrefOn = false;
		WaitCount = 0;
		ArmorTypesMap.clear();
		ReadyStateMap.clear();
		ArmorClothCombinationMap.clear();
		LootedCorpseMap.clear();
		StrippingKeyTappedMap.clear();
	}

	void StateSelector() 
	{
		if (Events::NeedReset) {
			ResetParameter();
			return;
		}
		if (target == nullptr && !crosshairrefOn) {
			StateTargetOffCrosshairOff();
		} else if(target != nullptr && crosshairrefOn) {
			StateTargetOnCrosshairOn();
		} else if (target && !crosshairrefOn) {
			StateTargetOnCrosshairOff();
		} else {
			StateTargetOffCrosshairOn();
		}
	}

	void StateTargetOffCrosshairOff()
	{
		return;
	}

	void StateTargetOnCrosshairOff()
	{
		if (target == nullptr) {
			Utility::Notification("UNKNOWN ERROR: StateTargetOnCrosshairOff: target is nullptr");
			return;
		}
		if (!target->IsActor())
			return;
		crosshairrefOn = true;
		LastTarget = target;
		ArmorTypesMap = Utility::GetArmorTypes(LastTarget);
		if (!ArmorClothCombinationMap.contains(LastTarget))
			ArmorClothCombinationMap[LastTarget] = GetArmorClothCombination();

		if (!LastTarget->IsDead(true))
			return;

		//target is dead
		ReadyStateMap[LastTarget] = true;
		//Utility::PrintArmorStacks2(target);
		if (!StrippingKeyTappedMap[LastTarget]) {
			Utility::ReadyForLoot2(LastTarget);
			if (!LootedCorpseMap.contains(LastTarget))
				LootedCorpseMap[LastTarget] = false;
			if (!StrippingKeyTappedMap.contains(LastTarget))
				StrippingKeyTappedMap[LastTarget] = false;
			WaitCount = WaitCountPlus;
		}
		//Utility::PrintArmorStacks2(target);

	}

	void StateTargetOnCrosshairOn()
	{
		if (LastTarget == nullptr) {
			Utility::Notification("UNKNOWN ERROR: StateTargetOnCrosshairOn: LastTarget is nullptr");
			return;
		}

		if (WaitCount > 0) {
			WaitCount--;
			return;
		}

		if (Config::GetUseStrippingKeyToCorpse() && LastTarget->IsDead(true) && IsKeyPressed()) {
			WaitCount = 10;
			RemoveEquipItems();
			Sleep(Config::GetTimePerFrame());
			StrippingArmor(LastTarget);
			LootedCorpseMap[LastTarget] = true;
			StrippingKeyTappedMap[LastTarget] = true;
		} else if (IsKeyPressed()) {
			WaitCount = 10;
			StrippingArmor(LastTarget);			
		} else {
			if (!LastTarget->IsDead(true))
				return;
			if (ReadyStateMap[LastTarget] == false)
				return;
			if (StrippingKeyTappedMap[LastTarget])
				return;
			//if (LootedCorpseMap.contains(LastTarget) && LootedCorpseMap[LastTarget])
			//	return;

			auto armors = Utility::GetLootedArmors(LastTarget);
			for (auto armor : armors) {
				if (!ArmorTypesMap.contains(armor))
					continue;
				Utility::Notification(fmt::format("  Looted:{}: {}: {}", Utility::num2hex(armor->formID), armor->GetFormEditorID(), ArmorTypesMap[armor]));

				if (ArmorTypesMap.contains(armor) && ArmorTypesMap[armor] == "Spacesuit") {
					if (LootedCorpseMap.contains(LastTarget))
						LootedCorpseMap[LastTarget] = true;
					StrippingArmorAlt(LastTarget, Utility::num2hex(armor->formID));
					if (Config::GetEffectEnabled())
						Sleep(Config::GetTimePerFrame());
					Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("UnequipItem {}", Utility::num2hex(armor->formID)));
					Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("RemoveItem {} 99", Utility::num2hex(armor->formID)));
				} else {
					Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("UnequipItem {}", Utility::num2hex(armor->formID)));
					Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("RemoveItem {} 99", Utility::num2hex(armor->formID)));
				}
			}
		}
	}

	void StateTargetOffCrosshairOn()
	{
		crosshairrefOn = false;
		if (LastTarget == nullptr) {
			Utility::Notification("UNKNOWN ERROR: StateTargetOffCrosshairOn: LastTarget is nullptr");
			return;
		}
		ReadyStateMap.erase(LastTarget);
		Utility::Notification(fmt::format("StateTargetOffCrosshairOn: route1"));
		Utility::Notification(fmt::format("StateTargetOffCrosshairOn: route1:{}", LastTarget->GetFormEditorID()));
		Utility::Notification(fmt::format("StateTargetOffCrosshairOn: route1:{}", LastTarget->IsDead(true)));
		Utility::Notification(fmt::format("StateTargetOffCrosshairOn: route1:{}", Config::GetChangingAppearanceOfCorpseEnabled()));
		Utility::Notification(fmt::format("StateTargetOffCrosshairOn: route1:{}", LootedCorpseMap[LastTarget]));
		Utility::Notification(fmt::format("  isDead:{}, CorpseEnabled:{}, LootedCorpseMap:{}", LastTarget->IsDead(true), Config::GetChangingAppearanceOfCorpseEnabled(), LootedCorpseMap[LastTarget]));

		if (LastTarget->IsDead(true) 
			&&LastTarget->GetCurrentLocation() == RE::PlayerCharacter::GetSingleton()->GetCurrentLocation()
			&& Config::GetChangingAppearanceOfCorpseEnabled() 
			&& LootedCorpseMap.contains(LastTarget) 
			&& LootedCorpseMap[LastTarget] == true) {

			Utility::Notification(fmt::format("StateTargetOffCrosshairOn: route2"));
			ChangingCorpse(LastTarget);
		}
		Utility::Notification(fmt::format("StateTargetOffCrosshairOn: route3"));
		ArmorClothCombinationMap.erase(LastTarget);
		LastTarget = nullptr;
	}

	void RemoveEquipItems()
	{
		Utility::Notification(fmt::format("RemoveEquipItem: start"));
		auto armors = Utility::CollectEquipItems(LastTarget, "ARMOR");
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;

			Utility::Notification(fmt::format("  Equipped:{}: {}", Utility::num2hex(item->formID), item->GetFormEditorID()));
			//if (count > 1) {
				Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("RemoveItem {} {}", Utility::num2hex(item->formID), count - 1));
			//}
		}

		Utility::Notification(fmt::format("RemoveEquipItem: finish"));
	}


	int GetArmorClothCombination()
	{
		bool clothOn = false;
		bool suitOn = false;
		for (auto itr = ArmorTypesMap.begin(); itr != ArmorTypesMap.end(); ++itr) {
			if (itr->second == "Cloth")
				clothOn = true;
			if (itr->second == "Spacesuit")
				suitOn = true;
		}

		int result = (clothOn) ? 2 : 0;
		result += (suitOn) ? 1 : 0;
		Utility::Notification(fmt::format("GetArmorClothCombination: result:{}, clothOn:{}, suitOn:{}", result, clothOn, suitOn));
		return result;
	}

	bool IsKeyPressed()
	{
		return SFSE::WinAPI::GetKeyState(Config::GetStrippingKeyNumber()) & 0x8000;
	}

	int GetBitParams()
	{
		//0:bForced, 1:
		int param = 0;
		//param += bForced ? 1 : 0;
		param += Config::GetAlternativeClothEnabled() ? 2 : 0;
		param += Config::GetChangingAppearanceOfCorpseEnabled() ? 4 : 0;
		param += Config::GetEffectEnabled() ? 8 : 0;
		param += Config::GetUseStrippingKeyToCorpse() ? 16 : 0;
		return param;
	}

	void StrippingArmor(RE::TESObjectREFR* obj)
	{
		if (obj == nullptr)
			return;
		int  BitParams = GetBitParams();
		bool bForced = RE::UI::GetSingleton()->IsMenuOpen("PickpocketMenu");
		BitParams += bForced ? 1 : 0;
		//Function CommonRunMe(string type, string sFormIDDec, int comboPattern, bool alternativeClothEnabled, string effectFormID, int corpseTimer, bool bForced) Global
		//Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.RunMe\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
		//	obj->formID, Utility::num2hex(obj->formID), ArmorClothCombinationMap[obj], Config::GetEffectEnabled(), Config::GetAlternativeClothEnabled(), Config::GetEffectFormID(), false ,bForced));
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.CommonRunMe\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
			"StrippingKey", obj->formID, ArmorClothCombinationMap[obj], BitParams, Config::GetEffectFormID(), Config::GetCorpseTimer()));
	}

	void StrippingArmorAlt(RE::TESObjectREFR* obj, std::string itemFormID)
	{
		if (obj == nullptr)
			return;
		int BitParams = GetBitParams();
		//Function CommonRunMe(string type, string sFormIDDec, int comboPattern, bool alternativeClothEnabled, string effectFormID, int corpseTimer, bool bForced) Global
		//Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.RunMeAlt\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
		//	obj->formID, Utility::num2hex(obj->formID), ArmorClothCombinationMap[obj], Config::GetEffectEnabled(), Config::GetAlternativeClothEnabled(), Config::GetEffectFormID(), false));
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.CommonRunMe\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
			"Looting", obj->formID, ArmorClothCombinationMap[obj], BitParams, Config::GetEffectFormID(), Config::GetCorpseTimer()));
	}

	void ChangingCorpse(RE::TESObjectREFR* obj)
	{
		if (obj == nullptr)
			return;
		int BitParams = GetBitParams();
		//Function CommonRunMe(string type, string sFormIDDec, int comboPattern, bool alternativeClothEnabled, string effectFormID, int corpseTimer, bool bForced) Global
		//Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.ChangingCorpse\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
		//	obj->formID, Utility::num2hex(obj->formID), ArmorClothCombinationMap[obj], Config::GetEffectEnabled(), Config::GetAlternativeClothEnabled(), Config::GetEffectFormID(), Config::GetChangingAppearanceOfCorpseEnabled(), Config::GetCorpseTimer()));
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.CommonRunMe\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
			"ChangingCorpse", obj->formID, ArmorClothCombinationMap[obj], BitParams, Config::GetEffectFormID(), Config::GetCorpseTimer()));
		LootedCorpseMap.erase(obj);
	}

}

DLLEXPORT bool SFSEAPI SFSEPlugin_Load(const SFSE::LoadInterface* a_sfse)
{
#ifndef NDEBUG
	MessageBoxA(NULL, "Loaded. You can now attach the debugger or continue execution.", Plugin::NAME.data(), NULL);
#endif

	SFSE::Init(a_sfse, false);
	DKUtil::Logger::Init(Plugin::NAME, std::to_string(Plugin::Version));
	INFO("{} v{} loaded", Plugin::NAME, Plugin::Version);

	if (const auto messaging{ SFSE::GetMessagingInterface() }; !messaging->RegisterListener(Listener))
		return false;
	CreateThread(NULL, 0, Main::MainLoop, NULL, 0, NULL);

	return true;
}
