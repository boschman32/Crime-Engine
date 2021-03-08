#include "expch.h"
#include "ExampleRender.h"
#include "Core/Core.h"

#include "Utils/ServiceManager.h"

#include "Core/ResourceManager.h"
#include "Core/Input/InputManager.h"
#include "Gfx/RenderService.h"
#include "Gfx/Drawables/Sprite.h"
#include "Gfx/Texture.h"
#include "Gfx/Camera2D.h"

namespace ce
{
	namespace examples
	{
		void ExampleRender::RunExample()
		{
			//Setup rendering.
			//If not exist create a render service.
			ServiceManager::MakeService<InputManager>();
			ServiceManager::MakeService<ResourceManager>();

			if (!ServiceManager::HasService<RenderService>())
			{
				ServiceManager::MakeService<RenderService>();
			}
			auto* gs = ServiceManager::GetService<RenderService>();
			auto* rs = ServiceManager::MakeService<ResourceManager>();
			auto* rl = gs->GetRenderLayer();

			auto cam = dynamic_cast<ce::Camera2D*>(Camera::GetMainCamera());

			//TODO: Move this to game?
			cam->SetPosition(glm::vec3(0, 0, -1));
			cam->SetBounds(glm::vec2(16, 9));
			cam->SetRenderDistance(glm::vec2(0.1f, 1000.0f));
			cam->SetOrthographicProjection(gs->GetSize());

			cam->LookDir(glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));

			auto tex = rs->LoadResource<Texture>("data/textures/Checkboard.dds");
			//Create a quad to draw.
			Sprite sprite;
			sprite.SetColor(Vector4(1, 0, 0, 1));
			sprite.SetPosition(Vector3(0, 0, 0.f));
			sprite.SetScale(Vector2(1, 1));
			sprite.SetTexture(*tex);

			//Color to clear the screen with.
			float c[] = { 0,0,0,0 };

			while (true)
			{
				//Calls to add in the game loop.
				//Clear the screen.
				gs->Clear(c);

				//Peak the window and handle the message.
				gs->Peak();

				cam->SetOrthographicProjection(gs->GetSize());
				//Prepare draw data.
				//Add our drawable to the draw list (every frame).
				sprite.Draw(*rl);

				//Update render things like creating a new frame for ImGui.
				gs->PreRender();

				//Render.
				gs->Render();
			}
		}
	}
}