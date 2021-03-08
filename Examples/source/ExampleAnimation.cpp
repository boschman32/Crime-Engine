#include "expch.h"
#include "ExampleAnimation.h"

#include "Utils/ServiceManager.h"

#include "Core/ResourceManager.h"
#include "Core/Input/InputManager.h"
#include "Gfx/RenderService.h"
#include "Gfx/Drawables/Sprite.h"
#include "Gfx/Texture.h"
#include "Gfx/Camera2D.h"

#include "Gfx/Animator2D.h"
#include "Utils/Timer.h"

void ce::examples::ExampleAnimation::RunExample()
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

	auto tex = rs->LoadResource<Texture>("data/textures/thief.png");
	//Create a quad to draw.
	Sprite sprite;
	sprite.SetColor(Vector4(1, 1, 1, 1));
	sprite.SetPosition(Vector3(0, 0, 0.f));
	sprite.SetScale(Vector2(5, 5));
	sprite.SetTexture(*tex);

	cr::Animator2D anim; // Create Animator
	anim.AddTexture(*tex, Vector2(128, 128)); // Add Texture and specify the frame size
	anim.CreateCycle("Idle",0, 17, 0.05f); // set a cycle from frame 0 till frame 17 with speed of 0.05 second
	anim.CreateCycle("Idle2", 18, 35, 0.05f); // set a cycle from frame 18 till frame 35 with speed of 0.05 second

	anim.SetCycle("Idle"); // Set the current Active Cycle

	//Color to clear the screen with.
	float c[] = { 0.29f,0.59f,0.82f,0 };

	while (true)
	{
		//Calls to add in the game loop.
		//Clear the screen.
		gs->Clear(c);

		//Peak the window and handle the message.
		gs->Peak();

		cam->SetOrthographicProjection(gs->GetSize());

		anim.Update(); //Update the Animator
		anim.AnimateSprite(sprite); //Animate the sprite

		if (InputManager::IsKeyDown(Keys::A))
		{
			CE_CORE_INFO("A press");
			anim.SetCycle("Idle"); // Set the current Active Cycle
		}
		if (InputManager::IsKeyDown(Keys::D))
		{
			CE_CORE_INFO("D press");	
			anim.SetCycle("Idle2"); // Set the current Active Cycle
		}

		//Prepare draw data.
		//Add our drawable to the draw list (every frame).
		sprite.Draw(*rl);

		//Update render things like creating a new frame for ImGui.
		gs->PreRender();

		//Render.
		gs->Render();
	}
}
