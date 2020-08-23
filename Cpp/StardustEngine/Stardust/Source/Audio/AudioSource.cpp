#include <openAl/al.h>
#include <openAl/alc.h>
#include <openAl/sndfile.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "AudioSource.h"


AudioSource::AudioSource()
{
	alGenSources(1, &_id);
	alSourcef(_id, AL_GAIN, 1.0f);
	alSourcef(_id, AL_PITCH, 1.0f);
	alSource3f(_id, AL_POSITION, 0.0f, 0.0f, 0.0f);

	setReferenceDistance(3.0f);
	setRollOfFactor(0.8f);
}

AudioSource::AudioSource(int buffer)
{
	this->_buffer = buffer;
	alGenSources(1, &_id);
	alSourcei(_id, AL_BUFFER, buffer);

	alSourcef(_id, AL_GAIN, 1.0f);
	alSourcef(_id, AL_PITCH, 1.0f);
	alSource3f(_id, AL_POSITION, 0.0f, 0.0f, 0.0f);
}

void AudioSource::setPosition(float x, float y, float z)
{
	alSource3f(_id, AL_POSITION, x, y, z);
}

void AudioSource::setVelocity(float x, float y, float z)
{
	alSource3f(_id, AL_VELOCITY, x, y, z);
}

void AudioSource::setPitch(float pitch)
{
	alSourcef(_id, AL_PITCH, pitch);
}

void AudioSource::setGain(float gain)
{
	alSourcef(_id, AL_GAIN, gain);
}

ALuint AudioSource::getId()
{
	return _id;
}

void AudioSource::setBuffer(int newBuffer)
{
	alSourcei(_id, AL_BUFFER, newBuffer);
}

ALint AudioSource::getBuffer()
{
	ALint buf;
	alGetSourcei(_id, AL_BUFFER, &buf);
	return buf;
}



float AudioSource::getGain()
{
	float value;
	alGetSourcef(_id, AL_GAIN, &value);
	return value;
}

glm::vec3 AudioSource::getPosition()
{
	glm::vec3 value;
	ALfloat valueX;
	ALfloat valueY;
	ALfloat valueZ;
	alGetSource3f(_id, AL_POSITION, &valueX, &valueY, &valueZ);
	value = glm::vec3(valueX, valueY, valueZ);
	return value;
}


glm::vec3 AudioSource::getDistanceFromListener()
{
	glm::vec3 distance;
	ALfloat dListenX;
	ALfloat dListenY;
	ALfloat dListenZ;
	alGetListener3f(AL_POSITION, &dListenX, &dListenY, &dListenZ);



	distance = glm::vec3(abs(dListenX - getPosition().x), abs(dListenY - getPosition().y), abs(dListenY - getPosition().z));
	return distance;
}


void AudioSource::setRollOfFactor(ALfloat value)
{
	alSourcef(_id, AL_ROLLOFF_FACTOR, value);
}

void AudioSource::setReferenceDistance(ALfloat value)
{
	alSourcef(_id, AL_REFERENCE_DISTANCE, value);
}

void AudioSource::setMaxDistance(ALfloat value)
{
	alSourcef(_id, AL_MAX_DISTANCE, value);
}