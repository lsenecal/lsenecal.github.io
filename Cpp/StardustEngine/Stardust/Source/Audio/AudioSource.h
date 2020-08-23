#include <openAl/al.h>
#include <openAl/alc.h>
#include <openAl/sndfile.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

using namespace std;

class AudioSource
{
public:
	AudioSource();
	AudioSource(int buffer);
	void setPosition(float x, float y, float z);
	void setVelocity(float x, float y, float z);
	void setPitch(float pitch);
	void setGain(float gain);
	ALuint getId();

	void setBuffer(int newBuffer);

	ALint getBuffer();

	ALfloat getGain();

	glm::vec3 getPosition();

	glm::vec3 getDistanceFromListener();

	void setRollOfFactor(ALfloat value);

	void setReferenceDistance(ALfloat value);

	void setMaxDistance(ALfloat value);

private:
	int _buffer;
	ALuint _id;
};
