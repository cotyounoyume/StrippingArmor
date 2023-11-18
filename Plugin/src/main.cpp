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
	void               State_Common(bool targetOn, bool crosshairOn);
	//void        StateTargetOnCrosshairOn();
	//void        StateTargetOnCrosshairOff();
	//void        StateTargetOffCrosshairOn();
	void        StateTargetOffCrosshairOff();
	void LootAndRemove();
	void StateTargetOnCrosshairOn_CorpseRoute();
	void StateTargetOffCrosshairOn_CorpseRoute();
	void StateTargetOnCrosshairOff_CorpseRoute();
	void StateTargetOnCrosshairOn_LivingRoute();
	void StateTargetOffCrosshairOn_LivingRoute();
	void StateTargetOnCrosshairOff_LivingRoute();

	bool               IsTargetValid(bool isLastTarget = false);
	bool               IsTargetDead(bool isLastTarget = false);
	void          RemoveEquipItems(bool leftOne = false);
	int         GetArmorClothCombination();
	void               DropEquipItems();
	void        ChangingCorpse(RE::TESObjectREFR* obj);
	void               ResetParameter();
	int                GetBitParams();
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

	static DWORD MainLoop(void* unused)
	{
		Utility::Notification("Main Start");
		Config::ReadIni();
		bool atOnce = true;

		while (true) {
			Sleep(Config::GetTimePerFrame());
			if (!Utility::InGameScene())
				continue;

			if (atOnce) {
				Events::SetupForms();
				atOnce = false;
			}
			UpdateCrosshairTarget();
			StateSelector();
		}

		return 0;
	}

	void UpdateCrosshairTarget()
	{
		auto tmpTarget = RE::PlayerCharacter::GetSingleton()->crosshairRef;
		if (tmpTarget != nullptr && tmpTarget->IsActor()) {
			target = tmpTarget;
			Events::DialogueTarget = target;
		} else if (Events::DialogueTarget != nullptr && Events::DialogueTarget->IsActor()) {
			target = Events::DialogueTarget;
		} else {
			target = nullptr;
		}
	}

	void StateSelector() 
	{
		if (Events::NeedReset) {
			ResetParameter();
			return;
		}
		if (target == nullptr && !crosshairrefOn) {
			State_Common(false, false);
		} else if(target != nullptr && crosshairrefOn) {
			State_Common(true, true);
		} else if (target && !crosshairrefOn) {
			State_Common(true, false);
		} else {
			State_Common(false, true);
		}
	}

	bool IsTargetValid(bool isLastTarget)
	{
		if (isLastTarget) {
			if (LastTarget == nullptr || !LastTarget->IsActor()) {
				//Utility::Notification("UNKNOWN ERROR: IsTargetValid: false");
				return false;
			} else {
				return true;
			}
		} else {
			if (target == nullptr || !target->IsActor()) {
				//Utility::Notification("UNKNOWN ERROR: IsTargetValid: false");
				return false;
			} else {
				return true;
			}
		}
	}

	bool IsTargetDead(bool isLastTarget)
	{
		return (isLastTarget) ? LastTarget->IsDead(true) : target->IsDead(true); 
	}

	void State_Common(bool targetOn, bool crosshairOn)
	{
		if (!targetOn && !crosshairOn)
			StateTargetOffCrosshairOff();

		bool isLastTarget = crosshairOn;
		if (!IsTargetValid(isLastTarget))
			return;

		if (IsTargetDead(isLastTarget)) {
			if (targetOn && crosshairOn) {
				StateTargetOnCrosshairOn_CorpseRoute();
			} else if (!targetOn && crosshairOn) {
				StateTargetOffCrosshairOn_CorpseRoute();
			} else if (targetOn && !crosshairOn) {
				StateTargetOnCrosshairOff_CorpseRoute();
			} 
		} else {
			if (targetOn && crosshairOn) {
				StateTargetOnCrosshairOn_LivingRoute();
			} else if (!targetOn && crosshairOn) {
				StateTargetOffCrosshairOn_LivingRoute();
			} else if (targetOn && !crosshairOn) {
				StateTargetOnCrosshairOff_LivingRoute();
			}
		}
	}

	void StateTargetOffCrosshairOff()
	{
		return;
	}

	void StateTargetOnCrosshairOff_CommonRoute()
	{
		crosshairrefOn = true;
		LastTarget = target;
		ArmorTypesMap = Utility::GetArmorTypes(LastTarget);
		if (!ArmorClothCombinationMap.contains(LastTarget))
			ArmorClothCombinationMap[LastTarget] = GetArmorClothCombination();
	}

	void StateTargetOnCrosshairOff_LivingRoute()
	{
		StateTargetOnCrosshairOff_CommonRoute();
	}

	void StateTargetOnCrosshairOn_LivingRoute()
	{
		if (IsKeyPressed()) {
			WaitCount = 10;
			DropEquipItems();
			StrippingArmor(LastTarget);
		}
	}

	void StateTargetOffCrosshairOn_Common()
	{
		crosshairrefOn = false;
		Utility::Notification(fmt::format("StateTargetOffCrosshairOn: route3"));
		ArmorClothCombinationMap.erase(LastTarget);
		LastTarget = nullptr;
	}

	void StateTargetOffCrosshairOn_LivingRoute()
	{
		StateTargetOffCrosshairOn_Common();
	}

	bool IsTapped() { return StrippingKeyTappedMap[LastTarget]; }
	bool IsReady() { return ReadyStateMap[LastTarget]; }
	bool IsLooted() { return LootedCorpseMap[LastTarget]; }

	void StateTargetOnCrosshairOff_CorpseRoute()
	{
		StateTargetOnCrosshairOff_CommonRoute();

		if (IsTapped())
			return;
		if (IsReady())
			return;

		//Utility::PrintArmorStacks2(target);
		Utility::ReadyForLoot2(LastTarget);
		LootedCorpseMap[LastTarget] = false;
		StrippingKeyTappedMap[LastTarget] = false;
		ReadyStateMap[LastTarget] = true;
		WaitCount = WaitCountPlus;
		//Utility::PrintArmorStacks2(target);
	}

	void StateTargetOnCrosshairOn_CorpseRoute(){
		if (!IsKeyPressed()){
			if (IsTapped())
				return;
			LootAndRemove();
			return;
		}

		if (!Config::GetUseStrippingKeyToCorpse())
			return;

		WaitCount = 10;
		if (!IsLooted()) {
			RemoveEquipItems();
			Sleep(Config::GetTimePerFrame());
			StrippingArmor(LastTarget);
			LootedCorpseMap[LastTarget] = true;
			StrippingKeyTappedMap[LastTarget] = true;
		} else {
			RemoveEquipItems(true);
			Sleep(Config::GetTimePerFrame());
			LootedCorpseMap[LastTarget] = true;
			StrippingKeyTappedMap[LastTarget] = true;
		}
	}

	void LootAndRemove()
	{
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

	void StateTargetOffCrosshairOn_CorpseRoute()
	{
		ReadyStateMap.erase(LastTarget);
		if (LastTarget->GetCurrentLocation() == RE::PlayerCharacter::GetSingleton()->GetCurrentLocation()
			&& Config::GetChangingAppearanceOfCorpseEnabled() 
			&& LootedCorpseMap.contains(LastTarget) 
			&& LootedCorpseMap[LastTarget] == true) {

			Utility::Notification(fmt::format("StateTargetOffCrosshairOn: route2"));
			ChangingCorpse(LastTarget);
		}
		StateTargetOffCrosshairOn_Common();
	}

	void DropEquipItems()
	{
		Utility::Notification(fmt::format("DropEquipItems: start"));
		auto armors = Utility::CollectEquipItems(LastTarget, "ARMOR");
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			auto name = std::string(item->GetFormEditorID());
			if (name.contains("Crowd")) {
				Utility::Notification(fmt::format("  Skip. This item is for crowd race.: {}, {}", Utility::num2hex(item->GetFormID()), name));
				continue;
			}

			item->SetPlayable(true);
			Utility::Notification(fmt::format("  Equipped:{}: {}", Utility::num2hex(item->formID), item->GetFormEditorID()));
			//Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("Drop {} {}", Utility::num2hex(item->formID), 1));

			Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.DropObjectScript\" \"{}\" \"{}\"",
				LastTarget->formID, item->formID));

		}
		Utility::Notification(fmt::format("DropEquipItems: finish"));
	}

	void RemoveEquipItems(bool leftOne)
	{
		Utility::Notification(fmt::format("RemoveEquipItem: start"));
		auto armors = Utility::CollectEquipItems(LastTarget, "ARMOR");
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;

			Utility::Notification(fmt::format("  Equipped:{}: {}", Utility::num2hex(item->formID), item->GetFormEditorID()));
			Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("RemoveItem {} {}", Utility::num2hex(item->formID), count - 1));
			if (leftOne) {
				Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("UnequipItem {}", Utility::num2hex(item->formID)));
			}
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
		param += Config::GetEffectForCorpseEnabled() ? 32 : 0;
		return param;
	}

	void StrippingArmor(RE::TESObjectREFR* obj)
	{
		if (obj == nullptr)
			return;
		int  BitParams = GetBitParams();
		bool bForced = (RE::UI::GetSingleton()->IsMenuOpen("PickpocketMenu") || RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu"));
		BitParams += bForced ? 1 : 0;
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.CommonRunMe\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
			"StrippingKey", obj->formID, ArmorClothCombinationMap[obj], BitParams, Config::GetEffectFormID(), Config::GetCorpseTimer()));
	}

	void StrippingArmorAlt(RE::TESObjectREFR* obj, std::string itemFormID)
	{
		if (obj == nullptr)
			return;
		int BitParams = GetBitParams();
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.CommonRunMe\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
			"Looting", obj->formID, ArmorClothCombinationMap[obj], BitParams, Config::GetEffectFormID(), Config::GetCorpseTimer()));
	}

	void ChangingCorpse(RE::TESObjectREFR* obj)
	{
		if (obj == nullptr)
			return;
		int BitParams = GetBitParams();
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
