#include <openAl/al.h>
#include <openAl/alc.h>
#include <openAl/sndfile.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "AudioMaster.h"


//Builder
AudioMaster::AudioMaster()
{
	std::vector<std::string> Devices;
	GetDevices(Devices);
	InitOpenAL(Devices[0].c_str());


	_CurrentAmbience = AudioAmbience(); 
	this->_radius = 0.0f;
	this->_pos_x = 0.0f;
	this->_pos_y = 0.0f;
	this->_pos_z = 0.0f;

	alListener3f(AL_POSITION, _pos_x, _pos_y, _pos_z);

	alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
}


//Return all devices
void AudioMaster::GetDevices(std::vector<std::string>& Devices)
{
	Devices.clear();
	const ALCchar* DeviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

	if (DeviceList)
	{
		while (strlen(DeviceList) > 0)
		{
			Devices.push_back(DeviceList);
			DeviceList += strlen(DeviceList) + 1;
		}
	}
	else
	{
		std::cerr << "ERROR - Can't get the devices list" << std::endl;
	}
}

//Initiate OpenAL
bool AudioMaster::InitOpenAL(const char* DeviceName)
{
	ALCdevice* Device = alcOpenDevice(DeviceName);
	if (!Device)
	{
		std::cerr << "ERROR - Can't open the default device" << std::endl;
		return false;
	}

	ALCcontext* Context = alcCreateContext(Device, NULL);
	if (!Context)
	{
		std::cerr << "ERROR - Can't create an audio context" << std::endl;
		return false;
	}

	if (!alcMakeContextCurrent(Context))
	{
		std::cerr << "ERROR - Can't activate the audio context" << std::endl;
		return false;
	}
	return true;
}


void AudioMaster::SetListenerPos(float x, float y, float z)
{
	alListener3f(AL_POSITION, x, y, z);
}

void AudioMaster::SetListenerOri(ALfloat x1, ALfloat y1, ALfloat z1, ALfloat x2, ALfloat y2, ALfloat z2)
{
	ALfloat ori[6];
	ori[0] = x1;
	ori[1] = y1;
	ori[2] = z1;
	ori[3] = x2;
	ori[4] = y2;
	ori[5] = z2;
	alListenerfv(AL_ORIENTATION, ori);
}


void AudioMaster::SetForestAmbience()
{
	AudioAmbience am = AudioAmbience();
	am.AddMultipleBuffers("oiseaux");
	am.AddAmbiantBuffer("vent_doux.wav");
	am.AddStepBuffer("step1.wav");
	am.AddSource();
	am.AddSource();
	am.AddSource();
	am.AddSource();
	am.AddSource();
	am.AddSource();
	am.AddSource();
	am.AddSource();
	am.AddSource();
	am.AddSource();


	am.LinkAmbiantBufferSource();

	am.TransformSource_GAIN(am.GetIdSource(1), 0.7f);
	am.LinkStepBufferSource();

	am.TransformSource_POSITION(am.GetIdSource(2), 27.0f, 24.0f, 7.0f);
	am.ProceduralSequence(am.GetIdSource(2));

	am.TransformSource_POSITION(am.GetIdSource(3), 31.0f, 24.0f, 82.0f);
	am.ProceduralSequence(am.GetIdSource(3));

	am.TransformSource_POSITION(am.GetIdSource(4), 87.0f, 18.0f, 60.0f);
	am.ProceduralSequence(am.GetIdSource(4));

	am.TransformSource_POSITION(am.GetIdSource(5), 113.0f, 25.0f, 58.0f);
	am.ProceduralSequence(am.GetIdSource(5));

	am.TransformSource_POSITION(am.GetIdSource(6), 90.0f, 25.0f, 117.0f);
	am.ProceduralSequence(am.GetIdSource(6));

	am.TransformSource_POSITION(am.GetIdSource(7), 31.0f, 23.0f, 148.0f);
	am.ProceduralSequence(am.GetIdSource(7));


	_CurrentAmbience = am;
}

void AudioMaster::PlayAmbience()
{
	_CurrentAmbience.PlayAllSourcesAuto();
}


void AudioMaster::Update(ALfloat pos_x, ALfloat pos_y, ALfloat pos_z, ALfloat front_x, ALfloat front_y, ALfloat front_z, ALfloat up_x, ALfloat up_y, ALfloat up_z)
{
	alListener3f(AL_POSITION, pos_x, pos_y, pos_z);
	float ori[6];
	ori[0] = front_x;
	ori[1] = front_y;
	ori[2] = front_z;
	ori[3] = up_x;
	ori[4] = up_y;
	ori[5] = up_z;
	alListenerfv(AL_ORIENTATION, ori);

	PlayAmbience();
}



void AudioMaster::ClearAll()
{
	_CurrentAmbience.cleanAll();
}
