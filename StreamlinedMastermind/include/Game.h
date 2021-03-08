#pragma once
#include "Core/Application.h"

class Game final : public ce::Application
{
public:
	Game();
	~Game() override;

	void OnInitialized() override;
	void OnClose() override;
};

