#include "DKUtil/Hook.hpp"
#include "PCH.h"

// SFSE message listener, use this to do stuff at specific moments during runtime
void Listener(SFSE::MessagingInterface::Message* message) noexcept
{
	if (message->type == SFSE::MessagingInterface::kPostLoad) {
		RE::UI::GetSingleton()->RegisterSink(Events::EventHandlerForMenu::GetSingleton());
		//RE::UI::GetSingleton()->RegisterSink(Events::EventHandlerForContainer::GetSingleton());
	}
}

namespace Debug
{
	void Notification(std::string message)
	{
		bool NotificationOn = false;
		bool InfoOn = false;
		//NotificationOn = true;
		InfoOn = true;
		if (NotificationOn)
			RE::DebugNotification(message.c_str());
		if (InfoOn)
			logger::info("{}", message);
	}
}

namespace Config
{
	std::string StrippingKey = "U";
	bool        EffectOn = true;
	bool        AlternativeClothOn = true;

	void ReadIni()
	{
		constexpr auto        pathString = L"Data/SFSE/Plugins/StrippingArmor.ini";
		std::filesystem::path path = std::filesystem::path(pathString);
		if (!std::filesystem::exists(path))
			return;

		Debug::Notification(fmt::format("StrippingArmor: path={}", path.string()));
		auto        config = toml::parse_file(path.string());

		std::string key = config["Config"]["StrippingKey"].value_or("");
		if (key == "")
			return;
		StrippingKey = key;
		Debug::Notification(fmt::format("StrippingArmor: key={}, keynum={}", key, key.c_str()[0]));

		std::string effect = config["Config"]["Effect"].value_or("");
		if (effect == "")
			return;
		EffectOn = (effect == "True" || effect == "true");
		Debug::Notification(fmt::format("StrippingArmor: effect={}, EffectOn={}", effect, EffectOn));

		std::string alternativeCloth = config["Config"]["AlternativeCloth"].value_or("");
		if (alternativeCloth == "")
			return;
		AlternativeClothOn = (alternativeCloth == "True" || alternativeCloth == "true");
		Debug::Notification(fmt::format("StrippingArmor: alternativeCloth={}, AlternativeClothOn={}", alternativeCloth, AlternativeClothOn));
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

	std::string num2hex(int num);
	bool        IsKeyPressed();
	void        Undress(RE::TESObjectREFR* obj);
	void        Test(RE::TESObjectREFR* obj);
	void        StrippingArmor(RE::TESObjectREFR* obj);
	bool        EffectON = false;

	void ExecuteCommandString(std::string command)
	{
		ExecuteCommand(0, command.data());
		Debug::Notification(command);
	}

	static DWORD MainLoop(void* unused)
	{
		Debug::Notification("Main Start");
		Config::ReadIni();
		int WaitCount = 0;
		(void)unused;
		bool               crosshairrefOn = false;
		int                TimePerFrame = 50;
		RE::TESObjectREFR* LastObject;

		while (true) {
			Sleep(TimePerFrame);
			auto* player = RE::PlayerCharacter::GetSingleton();
			if (player == nullptr)
				continue;
			auto obj = RE::PlayerCharacter::GetSingleton()->crosshairRef;
			if (obj == nullptr && !crosshairrefOn)
				continue;
			if (obj != nullptr && crosshairrefOn) {
				if (!LastObject->IsActor())
					continue;
				if (WaitCount > 0) {
					WaitCount--;
					continue;
				}
				if (IsKeyPressed()) {
					WaitCount = 10;
					StrippingArmor(obj);
				}
			}

			if (obj && !crosshairrefOn) {
				LastObject = obj;
				crosshairrefOn = true;
			} else {
				crosshairrefOn = false;
				LastObject = nullptr;
			}
		}

		return 0;
	}

	bool IsKeyPressed()
	{
		if (RE::UI::GetSingleton()->IsMenuOpen("Console"))
			return false;
		else
			return SFSE::WinAPI::GetKeyState(Config::GetStrippingKeyNumber()) & 0x8000;
	}

	void StrippingArmor(RE::TESObjectREFR* obj)
	{
		if (obj == nullptr)
			return;
		bool bForced = RE::UI::GetSingleton()->IsMenuOpen("PickpocketMenu");
		Debug::Notification(fmt::format("in Undress: bForced:{}", bForced));

		ExecuteCommandString(fmt::format("prid {}", num2hex(obj->formID)));
		ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.RunMe\" \"{}\" \"{}\" \"{}\" \"{}\"", 
			obj->formID, bForced, Config::GetEffectEnabled(), Config::GetAlternativeClothEnabled()));		
	}

	std::string num2hex(int num)
	{
		std::stringstream ss;
		ss << std::hex << num;
		return ss.str();
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
