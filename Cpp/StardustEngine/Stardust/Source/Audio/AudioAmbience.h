#include <openAl/al.h>
#include <openAl/alc.h>
#include <openAl/sndfile.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "AudioSource.h"

constexpr auto BEGIN_PATH = "Assets/Sounds/";

#define NB_BUFFERS_MAX 32
#define NB_SOURCES_MAX 32

using namespace std;

class AudioAmbience
{
public:

	AudioAmbience();


	ALuint* GetBuffers();
	ALuint* GetIdSources();
	ALuint GetBuffer(int id);
	ALuint GetIdSource(int id);
	AudioSource GetASource(ALuint id_source);

	ALuint LoadSound(const std::string& Filename);

	void PlaySource(ALuint id_source);
	void PlayAllSources();

	bool CheckDistance(ALuint id_Source);

	void PlaySourceAuto(ALuint id_source);

	void PlayAllSourcesAuto();

	void PlayAmbiantSource();

	void PlayStepSource();


	void PlayAllBuffers(ALuint id_source, vector<ALuint> tab_buffers);

	ALuint CreateSilence(double time);


	void ProceduralSequence(ALuint id_source);

	void TransformSource_GAIN(ALuint id_source, float modif);
	void TransformSource_PITCH(ALuint id_source, float modif);
	void TransformSource_POSITION(ALuint id_source, float x, float y, float z);

	void SetDistanceModel(ALuint id_source, ALfloat rollOf, ALfloat referenceDistance, ALfloat maxDistance);

	void FreeSource(ALuint id_source);

	void AddBuffer(string file);
	void AddMultipleBuffers(string file);
	void AddAmbiantBuffer(string file);
	void AddStepBuffer(string file);
	ALuint GetAmbiantBuffer();
	void AddSource();

	void LinkBufferSource(ALuint buffer, ALuint source);

	void LinkQueueBuffersSource(ALuint * buffers, ALuint id_source, ALsizei size);

	void LinkAmbiantBufferSource();

	void LinkStepBufferSource();

	void LinkAutoBufferSource(ALuint buffer);

	ALuint GetBufferByFile(string name);

	void ShutdownOpenAL();
	void cleanAll();

private:
	ALuint* _buffers = new ALuint[NB_BUFFERS_MAX];
	std::map<ALuint, AudioSource> _sources;
	ALuint* _id_sources = new ALuint[NB_SOURCES_MAX];
	ALuint _nb_sources;
	ALuint _nb_buffers;
	std::map<string, ALuint> _id_buffers;
	ALuint _procedural_counter;

	ALfloat _step_pitch_diff;
};

