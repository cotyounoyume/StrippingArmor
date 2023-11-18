#pragma once
#include "Event.h"
#include "Config.h"

namespace Events
{
	REL::Relocation<__int64 (*)(double, char*, ...)> ExecuteCommand{ REL::ID(166307) };  // From Console-Command-Runner-SF

	RE::BSEventNotifyControl EventHandlerForMenu::ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		Utility::Notification(fmt::format("Menu: name:{}, opening:{}", a_event.menuName.c_str(), a_event.opening));
		//for future release
		if (a_event.menuName == "PauseMenu" and a_event.opening) {
			Config::ReadIni();
			SetupForms();
		}
		if (a_event.menuName == "LoadingMenu" and a_event.opening) {
			NeedReset = true;
		}
		if (a_event.menuName == "DialogueMenu" and a_event.opening) {
			DialogueTarget = RE::PlayerCharacter::GetSingleton()->crosshairRef;
		}
		if (a_event.menuName == "DialogueMenu" and a_event.opening == false) {
			DialogueTarget = nullptr;
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventHandlerForContainer::ProcessEvent(const RE::TESContainerChangedEvent& a_event, RE::BSTEventSource<RE::TESContainerChangedEvent>*)
	{
		//for future release
		return RE::BSEventNotifyControl::kContinue;
	}

	void SetupForms()
	{
		RE::TESForm* form = RE::TESForm::LookupByID(0x6dead);
		//auto         id = RE::PlayerCharacter::GetSingleton()->GetFormID();
		//RE::TESForm* form = RE::TESForm::LookupByID(id);
		if (!form) {
			Utility::Notification(fmt::format("form not found."));
		} else {
			Utility::Notification(fmt::format("form:{} is {}", Utility::num2hex(form->GetFormID()), form->GetFormEditorID()));
		}

		RE::TESForm* form2 = RE::TESForm::LookupByEditorID("DontStripThis");
		//auto         id = RE::PlayerCharacter::GetSingleton()->GetFormID();
		//RE::TESForm* form = RE::TESForm::LookupByID(id);
		if (!form) {
			Utility::Notification(fmt::format("form2 not found."));
		} else {
			Utility::Notification(fmt::format("form2:{} is {}", Utility::num2hex(form->GetFormID()), form->GetFormEditorID()));
			if (form2->Is<RE::BGSKeyword>()) {
				Utility::Notification(fmt::format("form2:{} is {}, and This is BGSKeyword", Utility::num2hex(form2->GetFormID()), form2->GetFormEditorID()));
				auto form3 = dynamic_cast<RE::BGSKeyword*>(form2);
				if (form3) {
					Utility::Notification(fmt::format("form3:{}", form3->GetFormEditorID()));
				}
			}
		}
	}
}