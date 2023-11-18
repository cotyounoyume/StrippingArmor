#pragma once
#include "Utility.h"
#include "PCH.h"

template <class T>
class ISingleton
{
public:
	static T* GetSingleton()
	{
		static T singleton;
		return std::addressof(singleton);
	}

protected:
	ISingleton() = default;
	~ISingleton() = default;

	ISingleton(const ISingleton&) = delete;
	ISingleton(ISingleton&&) = delete;
	ISingleton& operator=(const ISingleton&) = delete;
	ISingleton& operator=(ISingleton&&) = delete;
};

struct TESContainerChangedEvent
{
	std::uint32_t sourceContainerFormID;  // 00
	std::uint32_t targetContainerFormID;  // 04
	std::uint32_t itemFormID;             // 08
	std::uint32_t count;                  // 0C
										  //...
};

namespace Events
{
	inline bool NeedReset = false;
	inline RE::TESObjectREFR* DialogueTarget;
	class EventHandlerForMenu : public ISingleton<EventHandlerForMenu>, public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;
	};

	class EventHandlerForContainer : public ISingleton<EventHandlerForContainer>, public RE::BSTEventSink<RE::TESContainerChangedEvent>
	{
		RE::BSEventNotifyControl ProcessEvent(const RE::TESContainerChangedEvent& a_event, RE::BSTEventSource<RE::TESContainerChangedEvent>* a_eventSource) override;
	};

	void SetupForms();
}  // namespace Events