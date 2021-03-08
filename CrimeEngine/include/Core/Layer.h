#pragma once
#include <string>

namespace ce
{
	class Layer
	{
	public:
		Layer(std::string a_debugName = "DefaultLayer");
		virtual ~Layer() = default;

		virtual void OnAttach() { }
		virtual void OnDetach() { }
        virtual void OnPlay() { }
		virtual void OnPause() { }
        virtual void OnEditorStop() { }
		
		virtual void OnUpdate(float) { }
        virtual void OnImGuiRender() { }
        virtual void OnEvent() { };

		const std::string& GetDebugName() const { return m_debugName;  }
		void SetEnabled(bool a_enabled) { m_isEnabled = a_enabled; }
		bool IsEnabled() const { return m_isEnabled; }
	protected:
		std::string m_debugName;
		bool m_isEnabled { true };
	};
}
