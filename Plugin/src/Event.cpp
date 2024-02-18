#include "Event.h"

namespace Events
{
	REL::Relocation<__int64 (*)(double, char*, ...)> ExecuteCommand{ REL::ID(166307) };  // From Console-Command-Runner-SF

	RE::BSEventNotifyControl EventHandlerForMenu::ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		Debug(format("Menu: name:{}, opening:{}", a_event.menuName.c_str(), a_event.opening));
		//for future release
		if (a_event.menuName == "MainMenu" and a_event.opening) {
			mainMenuOpened = true;
		}
		if (mainMenuOpened && a_event.menuName == "MainMenu" and a_event.opening == false) {
			mainMenuClosed = true;
		}
		if (a_event.menuName == "PauseMenu" and a_event.opening) {
			Config::ReadIni();
		}
		if (a_event.menuName == "LoadingMenu" and !a_event.opening) {
			if (mainMenuClosed) {
				StrippingArmorCommon::MakeKeywordMapIfNeeded();
				NeedReset = true;
			}
		}

		if (a_event.menuName == "DialogueMenu" and a_event.opening) {
			bool openstate = RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu");
			Debug(format("Menu: name:{} openstate:{}", a_event.menuName.c_str(), openstate));
		}
		if (a_event.menuName == "DialogueMenu" and a_event.opening == false) {
			bool openstate = RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu");
			Debug(format("Menu: name:{} openstate:{}", a_event.menuName.c_str(), openstate));
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventHandlerForContainer::ProcessEvent(const RE::TESContainerChangedEvent& a_event, RE::BSTEventSource<RE::TESContainerChangedEvent>*)
	{
		//for future release
		return RE::BSEventNotifyControl::kContinue;
	}
}