#pragma once
#include "Core/Layer.h"
#include "EditorConsole.h"
#include "IEditorWindow.h"

namespace ce
{
	namespace editor
	{
		class EditorLayer : public Layer
		{
		public:
			EditorLayer();
			~EditorLayer() override = default;

			static EditorLayer& Get() { return *ms_instance; }
			
			void OnAttach() override;
			void OnDetach() override;
			void OnUpdate(float a_deltaTime) override;
			
	        void OnImGuiRender() override;

			template<typename T, typename = typename std::enable_if<std::is_base_of<IEditorWindow, T>::value, T>::type>
			T* GetWindow(const std::string& a_windowName);
			
		private:
			static EditorLayer* ms_instance;
			
			std::unordered_map<std::string, std::unique_ptr<IEditorWindow>> m_editorWindows;

			static void SetImGuiStyle();

			std::unique_ptr<EditorConsole> m_console;
		};

		template <typename T, typename>
		T* EditorLayer::GetWindow(const std::string& a_windowName)
		{
			const auto window = m_editorWindows.find(a_windowName);

			if(window != m_editorWindows.end())
			{
				return dynamic_cast<T*>(window->second.get());
			}
			return nullptr;
		}

	}
}
