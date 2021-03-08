#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <thread>
#include <iostream>
#include <vector>

class Audio
{
public:
	int SetupSoundSystem();

	void CreateSound(const char* a_path, bool a_loop);
	void PlaySounds();

	void SetVolume(float a_volume) const;

	void Update();
private:
	FMOD_RESULT m_result;
	FMOD::System* m_soundSystem = nullptr;
	FMOD::ChannelGroup* m_channelGroup = nullptr;
	std::vector<FMOD::Sound*> m_allSounds;
	FMOD::Channel* m_channel = nullptr;

	bool SucceededOrWarn(const std::string& a_message, FMOD_RESULT a_result);
};
