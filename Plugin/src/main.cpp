#include "DKUtil/Hook.hpp"
#include "PCH.h"
#include "Utility.h"

// SFSE message listener, use this to do stuff at specific moments during runtime
void Listener(SFSE::MessagingInterface::Message* message) noexcept
{
	if (message->type == SFSE::MessagingInterface::kPostLoad) {
		RE::UI::GetSingleton()->RegisterSink(Events::EventHandlerForMenu::GetSingleton());
		//RE::UI::GetSingleton()->RegisterSink(Events::EventHandlerForContainer::GetSingleton());
	}
}

namespace Config
{
	std::string StrippingKey = "U";
	bool        EffectOn = true;
	std::string EffectFormID = "0002e54d";
	bool        AlternativeClothOn = true;

	void ReadIni()
	{
		constexpr auto        pathString = L"Data/SFSE/Plugins/StrippingArmor.ini";
		std::filesystem::path path = std::filesystem::path(pathString);
		if (!std::filesystem::exists(path))
			return;

		Utility::Notification(fmt::format("StrippingArmor: path={}", path.string()));
		auto        config = toml::parse_file(path.string());

		std::string key = config["Config"]["StrippingKey"].value_or("");
		if (key == "")
			return;
		StrippingKey = key;
		Utility::Notification(fmt::format("StrippingArmor: key={}, keynum={}", key, key.c_str()[0]));

		std::string effect = config["Config"]["Effect"].value_or("");
		if (effect == "")
			return;
		EffectOn = (effect == "True" || effect == "true");
		Utility::Notification(fmt::format("StrippingArmor: effect={}, EffectOn={}", effect, EffectOn));

		std::string effectFormID = config["Config"]["EffectFormID"].value_or("");
		if (effectFormID == "")
			return;
		EffectFormID = effectFormID;
		Utility::Notification(fmt::format("StrippingArmor: effectFormID={}, EffectFormID={}", effectFormID, EffectFormID));

		std::string alternativeCloth = config["Config"]["AlternativeCloth"].value_or("");
		if (alternativeCloth == "")
			return;
		AlternativeClothOn = (alternativeCloth == "True" || alternativeCloth == "true");
		Utility::Notification(fmt::format("StrippingArmor: alternativeCloth={}, AlternativeClothOn={}", alternativeCloth, AlternativeClothOn));
	}

	std::string GetStrippingKey()
	{
		return StrippingKey;
	}

	char GetStrippingKeyNumber()
	{
		return StrippingKey.c_str()[0];
	}

	bool GetEffectEnabled()
	{
		return EffectOn;
	}

	std::string GetEffectFormID()
	{
		return EffectFormID;
	}

	bool GetAlternativeClothEnabled()
	{
		return AlternativeClothOn;
	}
}

namespace Events
{
	REL::Relocation<__int64 (*)(double, char*, ...)> ExecuteCommand{ REL::ID(166307) };  // From Console-Command-Runner-SF

	RE::BSEventNotifyControl EventHandlerForMenu::ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		Utility::Notification(fmt::format("Menu: name:{}, opening:{}", a_event.menuName.c_str(), a_event.opening));
		//for future release
		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventHandlerForContainer::ProcessEvent(const RE::TESContainerChangedEvent& a_event, RE::BSTEventSource<RE::TESContainerChangedEvent>*)
	{
		//for future release
		return RE::BSEventNotifyControl::kContinue;
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
	bool        EffectON = false;

	RE::TESObjectREFR* target;
	RE::TESObjectREFR* LastTarget;
	bool               crosshairrefOn = false;


	int WaitCount = 0;
	int WaitCountPlus = 10;
	int TimePerFrame = 200;

	std::unordered_map<RE::TESBoundObject*, std::string> ArmorTypesMap;
	std::unordered_map<RE::TESObjectREFR*, bool> ReadyStateMap;
	std::unordered_map<RE::TESObjectREFR*, int> ArmorClothCombinationMap;

	static DWORD MainLoop(void* unused)
	{
		Utility::Notification("Main Start");
		Config::ReadIni();

		while (true) {
			Sleep(TimePerFrame);
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

	void StateSelector() 
	{
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
		if (!target->IsActor())
			return;
		crosshairrefOn = true;
		LastTarget = target;
		if (!target->IsDead(true))
			return;
		ReadyStateMap[target] = true;
		//Utility::PrintArmorStacks2(target);
		Utility::ReadyForLoot2(target);
		//Utility::PrintArmorStacks2(target);
		ArmorTypesMap = Utility::GetArmorTypes(target);
		WaitCount = WaitCountPlus;
		if (!ArmorClothCombinationMap.contains(target))
			ArmorClothCombinationMap[target] = GetArmorClothCombination();
	}

	void StateTargetOnCrosshairOn()
	{
		if (WaitCount > 0) {
			WaitCount--;
			return;
		}
		if (target->IsDead(true)) {
			if (ReadyStateMap[target] == false)
				return;
			auto armors = Utility::GetLootedArmors(target);
			for (auto armor : armors) {
				if (!ArmorTypesMap.contains(armor))
					continue;
				Utility::Notification(fmt::format("  Looted:{}: {}: {}", Utility::num2hex(armor->formID), armor->GetFormEditorID(), ArmorTypesMap[armor]));

				if (ArmorTypesMap.contains(armor) && ArmorTypesMap[armor] == "Spacesuit") {
					StrippingArmorAlt(target, Utility::num2hex(armor->formID));
					if (Config::GetEffectEnabled())
						Sleep(100);
					Utility::ExecuteCommandStringOnFormID(target->formID, fmt::format("UnequipItem {}", Utility::num2hex(armor->formID)));
					Utility::ExecuteCommandStringOnFormID(target->formID, fmt::format("RemoveItem {} 99", Utility::num2hex(armor->formID)));
				} else {
					Utility::ExecuteCommandStringOnFormID(target->formID, fmt::format("UnequipItem {}", Utility::num2hex(armor->formID)));
					Utility::ExecuteCommandStringOnFormID(target->formID, fmt::format("RemoveItem {} 99", Utility::num2hex(armor->formID)));
				}
			}
		} else {
			if (IsKeyPressed()) {
				WaitCount = 10;
				StrippingArmor(target);
			}
		}
	}

	void StateTargetOffCrosshairOn()
	{
		crosshairrefOn = false;
		ReadyStateMap.erase(target);
		LastTarget = nullptr;
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

	void StrippingArmorAlt(RE::TESObjectREFR* obj, std::string itemFormID)
	{
		if (obj == nullptr)
			return;
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.RunMeAlt\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
			obj->formID, Utility::num2hex(obj->formID), ArmorClothCombinationMap[target], Config::GetEffectEnabled(), Config::GetAlternativeClothEnabled(), Config::GetEffectFormID()));
		ArmorClothCombinationMap.erase(target);

	}

	void StrippingArmor(RE::TESObjectREFR* obj)
	{
		if (obj == nullptr)
			return;
		bool bForced = RE::UI::GetSingleton()->IsMenuOpen("PickpocketMenu");
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.RunMe\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\"", 
			obj->formID, Utility::num2hex(obj->formID), bForced, Config::GetEffectEnabled(), Config::GetAlternativeClothEnabled(), Config::GetEffectFormID()));		
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
