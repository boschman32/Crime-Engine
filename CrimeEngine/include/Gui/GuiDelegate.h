#pragma once
#include <vector>
#include <utility>

//Modified version of:
//https://blog.molecular-matters.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/
//Changed: naming, made it for void, added support for multiple listeners, added action helper function.

namespace ce
{
	using InstancePtr = void*;
	using InternalFunction = void(*)(InstancePtr);
	using GuiAction = std::pair<InstancePtr, InternalFunction>;

	class GuiDelegate
	{
	public:
		GuiDelegate() = default;
		~GuiDelegate() = default;

		//Create a GuiAction.
		template <class C, void (C::*F)()>
		static inline GuiAction CreateGuiAction(C* a_instance)
		{
			return GuiAction{ a_instance, &CreateInternalFunction<C, F> };
		}

		//Add listener: Class, pointer to ClassFunction, pointer to Instance;
		//AddListener<MyClass, &MyClass::MyFunction>(&myClassInstance);
		template <class C, void (C::*F)()>
		void AddListener(C* a_instance)
		{
			GuiAction action{ a_instance, &CreateInternalFunction<C, F> };
			AddListener(action);
		}

		void AddListener(GuiAction& a_action)
		{
			m_listeners.push_back(a_action);
		}

		//Remove listener: Class, pointer to ClassFunction, pointer to Instance;
		//RemoveListener<MyClass, &MyClass::MyFunction>(&myClassInstance);
		template <class C, void (C::*F)()>
		void RemoveListener(C* a_instance)
		{
			GuiAction action{ a_instance, &CreateInternalFunction<C, F> };
			RemoveListener(action);
		}

		void RemoveListener(GuiAction& a_action)
		{
			m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), a_action), m_listeners.end());
		}

		void Invoke() const
		{
			for (const auto& a : m_listeners)
			{
				a.second(a.first);
			}
		}

	private:
		//Turns a member function into an internal function.
		template <class C, void (C::*F)()>
		static inline void CreateInternalFunction(InstancePtr a_instance)
		{
			//Cast the instance pointer back into the original class instance.
			return (static_cast<C*>(a_instance)->*F)();
		}

		std::vector<GuiAction> m_listeners = std::vector<GuiAction>();
	};
}