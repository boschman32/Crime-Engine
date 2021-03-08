#pragma once

#include "Core/Math.h"
#include "Core/Resource.h"

#include "Utils/Timer.h"


namespace ce
{
	class Texture;
	class Sprite;
}

namespace cr
{

	struct FrameData
	{
		ce::ResourceId m_textureId;
		ce::Vector4 m_frameRect;
	};

	struct AnimCycleData 
	{
		std::string m_cycleName;
		unsigned int m_startFrame;
		unsigned int m_endFrame;

		float m_cycleTimer;
	};

	class Animator2D
	{
	public:
		Animator2D() = default;
		~Animator2D();

		void AddTexture(ce::Texture&);
		void AddTexture(ce::Texture&, ce::Vector2);

		void CreateCycle(std::string, unsigned int, unsigned int, float);

		void SetFrame(unsigned int);
		void SetCycle(std::string);
		void AnimateSprite(ce::Sprite&);

		void Update();
	private:
		unsigned int m_activeCycle;
		bool m_cycleAnimation;
		unsigned int m_lastFrame;
		unsigned int m_activeFrame;
		ce::Timer<float>  m_timer;

		std::vector<FrameData> m_frames;
		std::vector<AnimCycleData> m_cycles;
	};
}