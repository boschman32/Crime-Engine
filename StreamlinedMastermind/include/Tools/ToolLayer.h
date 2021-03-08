#pragma once
#include "Core/Layer.h"
#include "Utils/Editor/IEditorWindow.h"
 
class ToolLayer : public ce::Layer
{
public:

	ToolLayer();
	~ToolLayer() override = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float a_deltaTime) override;
	void OnImGuiRender() override;

private:

	std::vector<std::unique_ptr<ce::editor::IEditorWindow>> m_toolWindows;
	static void SetImGuiStyle();
};