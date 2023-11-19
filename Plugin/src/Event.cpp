#include "Event.h"

namespace Events
{
	REL::Relocation<__int64 (*)(double, char*, ...)> ExecuteCommand{ REL::ID(166307) };  // From Console-Command-Runner-SF

	RE::BSEventNotifyControl EventHandlerForMenu::ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		Utility::Notification(fmt::format("Menu: name:{}, opening:{}", a_event.menuName.c_str(), a_event.opening));
		//for future release
		if (a_event.menuName == "MainMenu" and !a_event.opening) {
			mainMenuClosed = true;
		}
		if (a_event.menuName == "PauseMenu" and a_event.opening) {
			Config::ReadIni();
			//SetupForms();
		}
		if (a_event.menuName == "LoadingMenu" and !a_event.opening) {
			if (mainMenuClosed) {
				StrippingArmor::MakeKeywordMapIfNeeded();
				NeedReset = true;
			}
		}
		if (a_event.menuName == "DialogueMenu" and a_event.opening) {
			//DialogueTarget = RE::PlayerCharacter::GetSingleton()->crosshairRef;
			bool openstate = RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu");
			Utility::Notification(fmt::format("Menu: name:{} openstate:{}", a_event.menuName.c_str(), openstate));
			//Utility::Notification(fmt::format("Menu: openstate:{}", openstate));
		}
		if (a_event.menuName == "DialogueMenu" and a_event.opening == false) {
			//DialogueTarget = nullptr;
			bool openstate = RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu");
			//Utility::Notification(fmt::format("Menu: openstate:{}", openstate));
			Utility::Notification(fmt::format("Menu: name:{} openstate:{}", a_event.menuName.c_str(), openstate));
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventHandlerForContainer::ProcessEvent(const RE::TESContainerChangedEvent& a_event, RE::BSTEventSource<RE::TESContainerChangedEvent>*)
	{
		//for future release
		return RE::BSEventNotifyControl::kContinue;
	}
}