#include "cepch.h"
#include "Gfx/Animator2D.h"

#include "Gfx/Texture.h"
#include "Gfx/Drawables/Sprite.h"
cr::Animator2D::~Animator2D()
{
}

void cr::Animator2D::AddTexture(ce::Texture& a_texture)
{
	FrameData newFrame;
	newFrame.m_textureId = a_texture.GetID();
	newFrame.m_frameRect = ce::Vector4(0,0, a_texture.GetTextureData().m_width, a_texture.GetTextureData().m_height);
	m_frames.push_back(newFrame);
	m_activeFrame = 0;
}

void cr::Animator2D::AddTexture(ce::Texture& a_texture, ce::Vector2 a_frameSize)
{
	FrameData newFrame;
	newFrame.m_textureId = a_texture.GetID();

	int w = a_texture.GetTextureData().m_width;
	int h = a_texture.GetTextureData().m_height;

	int xFrames = w / static_cast<int>(a_frameSize.x);
	int yFrame = h / static_cast<int>(a_frameSize.y);

	for (int sY = 0; sY < yFrame; ++sY)
	{
		for (int sX = 0; sX < xFrames; ++sX)
		{
			newFrame.m_frameRect = ce::Vector4(a_frameSize.x * static_cast<float>(sX), a_frameSize.y * static_cast<float>(sY),
												a_frameSize.x , a_frameSize.y);
			m_frames.push_back(newFrame);
		}
	}
}

void cr::Animator2D::CreateCycle(std::string a_cycleName,unsigned int a_frameS, unsigned int a_frameE, float a_cycleSpeed)
{
	AnimCycleData cycle;

	cycle.m_cycleName = a_cycleName;
	cycle.m_startFrame = a_frameS;
	cycle.m_endFrame = a_frameE;
	cycle.m_cycleTimer = a_cycleSpeed;

	m_cycles.push_back(cycle);
}

void cr::Animator2D::SetFrame(unsigned int a_frame)
{
	m_cycleAnimation = false;
	m_activeFrame = a_frame;
}

void cr::Animator2D::SetCycle(std::string a_cycleName)
{
	m_cycleAnimation = true;
	unsigned int cId = 0;

	for (auto c : m_cycles)
	{
		if (c.m_cycleName == a_cycleName)
		{
			break;
		}
		cId++;
	}
	m_activeCycle = cId;
	m_timer.Start(m_cycles[cId].m_cycleTimer);
	m_activeFrame = m_cycles[cId].m_startFrame;
}

void cr::Animator2D::AnimateSprite(ce::Sprite& a_sprite)
{
	a_sprite.SetRect(m_frames[m_activeFrame].m_frameRect);
}

void cr::Animator2D::Update()
{
	if (!m_cycleAnimation)
	{
		return;
	}


	if (m_timer.IsFinished())
	{
		m_timer.Start(m_cycles[m_activeCycle].m_cycleTimer);
		m_activeFrame++;
		if (m_activeFrame > m_cycles[m_activeCycle].m_endFrame)
		{
			m_activeFrame = m_cycles[m_activeCycle].m_startFrame;
		}
	}
}
