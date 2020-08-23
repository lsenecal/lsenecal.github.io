#include <openAl/al.h>
#include <openAl/alc.h>
#include <openAl/sndfile.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "AudioAmbience.h"

using namespace std;

class AudioMaster
{
public :
	AudioMaster();

	void GetDevices(std::vector<std::string>& Devices);
	bool InitOpenAL(const char* DeviceName = NULL);


	void SetListenerPos(float x, float y, float z);

	void SetListenerOri(ALfloat x1, ALfloat y1, ALfloat z1, ALfloat x2, ALfloat y2, ALfloat z2);


	void SetForestAmbience();


	void PlayAmbience();

	void Update(ALfloat pos_x, ALfloat pos_y, ALfloat pos_z, ALfloat front_x, ALfloat front_y, ALfloat front_z, ALfloat up_x, ALfloat up_y, ALfloat up_z);


	void ClearAll();

private:
	AudioAmbience _CurrentAmbience;
	float _pos_x;
	float _pos_y;
	float _pos_z;
	float _radius;
};
