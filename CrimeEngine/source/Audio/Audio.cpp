#include "cepch.h"
#include "Audio/Audio.h" 

bool Audio::SucceededOrWarn(const std::string& a_message, FMOD_RESULT a_result)
{
	if (a_result != FMOD_OK) 
	{
		CE_CORE_ERROR("{0} : {1} {2}", a_message, a_result, FMOD_ErrorString(a_result));
		return false;
	}
	return true;
}

int Audio::SetupSoundSystem()
{
	// Create the main system object.
	m_result = FMOD::System_Create(&m_soundSystem);
	if (!SucceededOrWarn("FMOD: Failed to create system object", m_result))
	{
		return 1;
	}

	// Initialize FMOD.
	m_result = m_soundSystem->init(512, FMOD_INIT_NORMAL, nullptr);
	if (!SucceededOrWarn("FMOD: Failed to initialise system object", m_result))
	{
		return 1;
	}

	// Create the channel group.
	m_result = m_soundSystem->createChannelGroup("inGameSoundEffects", &m_channelGroup);
	if (!SucceededOrWarn("FMOD: Failed to create in-game sound effects channel group", m_result))
	{
		return 1;
	}

	return 0;
}

void Audio::CreateSound(const char* a_path, bool a_loop)
{
	FMOD::Sound* s = nullptr;

	if (a_loop) 
	{
		m_soundSystem->createSound(a_path, FMOD_LOOP_NORMAL, nullptr, &s);
	}
	else 
	{
		m_soundSystem->createSound(a_path, FMOD_DEFAULT, nullptr, &s);
	}
	m_allSounds.push_back(s);
}

void Audio::PlaySounds()
{
	for (auto s : m_allSounds)
	{
		m_result = m_soundSystem->playSound(s, nullptr, false, &m_channel);
	}
}

void Audio::SetVolume(float a_volume) const
{
	m_channel->setVolume(a_volume);
}

void Audio::Update()
{
	//Play sound
	bool isPlaying = true;
	m_channel->isPlaying(&isPlaying);

	if (!isPlaying) 
	{
		for (auto s : m_allSounds)
		{
			s->release();
		}
		m_channelGroup->release();
		m_soundSystem->release();
	}
}
