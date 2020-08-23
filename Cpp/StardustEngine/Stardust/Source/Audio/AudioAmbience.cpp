#include <openAl/al.h>
#include <openAl/alc.h>
#include <openAl/sndfile.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <openAl/dirent.h>
#include <time.h>
#include <algorithm>

#include "AudioAmbience.h"

using namespace std;

//Builder
AudioAmbience::AudioAmbience()
{
	_nb_sources = 0;
	_nb_buffers = 2;
	_procedural_counter = 1;
	_step_pitch_diff = 0.2f;

	ALfloat x_pos, y_pos, z_pos;
	alGetListener3f(AL_POSITION, &x_pos, &y_pos, &z_pos);
}


//Return all buffers
ALuint* AudioAmbience::GetBuffers()
{
	return _buffers;
}

//Return a specific buffer
ALuint AudioAmbience::GetBuffer(int id)
{
	return _buffers[id];
}

//Return a buffer by the name of the file
ALuint AudioAmbience::GetBufferByFile(string file)
{
	ALuint thebuffer;
	try
	{
		thebuffer = _id_buffers.at(file);
	}
	catch (const std::out_of_range& e)
	{
		std::cerr << "ERROR - Buffer doesn't exit" << std::endl;
		return NULL;
	}
	return GetBuffer(thebuffer);
}


//Return all id of sources
ALuint* AudioAmbience::GetIdSources()
{
	return _id_sources;
}

//Return a specific id of a source
ALuint AudioAmbience::GetIdSource(int id)
{
	return _id_sources[id];
}

//Return a specific Source
AudioSource AudioAmbience::GetASource(ALuint id_source)
{
	return _sources.at(id_source);
}



//Add a buffer
void AudioAmbience::AddBuffer(string file)
{
	string path = BEGIN_PATH + file;
	ALuint Buffer = LoadSound(path);
	if (Buffer == 0)
	{
		std::cerr << "ERROR - File Sound not found" << std::endl;
		return;
	}
	else
	{
		if (_nb_buffers < NB_BUFFERS_MAX)
		{
			_buffers[_nb_buffers] = Buffer;
			_id_buffers.insert(pair<string, ALuint>(file, _nb_buffers));

			_nb_buffers++;
		}
		else
		{
			std::cerr << "ERROR - Limit of buffers reached" << std::endl;
			return;
		}
	}
}

//Add many buffers from a repository
void AudioAmbience::AddMultipleBuffers(string rep_name)
{
	string path = BEGIN_PATH + rep_name;

	DIR* rep = opendir(path.c_str());

	vector<string> files_name;

	if (rep != NULL)
	{
		struct dirent * ent;

		while ((ent = readdir(rep)) != NULL)
		{
			if (ent->d_type == DT_REG)
			{
				string file = rep_name + "/" + ent->d_name;
				files_name.push_back(file);
			}
		}

		closedir(rep);
		srand(time(NULL));

		while (_nb_buffers < NB_BUFFERS_MAX && files_name.size() > 0)
		{
			int taille = files_name.size();
			int selector = rand() % files_name.size();
			AddBuffer(files_name[selector]);
			files_name.erase(files_name.begin() + selector);
			files_name.resize(taille - 1);
		}
	}
}

//Add or replace the ambiant buffer, buffer 0
void AudioAmbience::AddAmbiantBuffer(string file)
{
	string path = BEGIN_PATH + file;
	ALuint Buffer = LoadSound(path);
	if (Buffer == 0)
	{
		std::cerr << "ERROR - Sound file not found" << std::endl;
		return;
	}
	else
	{
		_buffers[0] = Buffer;
	}
}


//Add or replace the ambiant buffer, buffer 0
void AudioAmbience::AddStepBuffer(string file)
{
	string path = BEGIN_PATH + file;
	ALuint Buffer = LoadSound(path);
	if (Buffer == 0)
	{
		std::cerr << "ERROR - Sound file not found" << std::endl;
		return;
	}
	else
	{
		_buffers[1] = Buffer;
	}
}

//Get ambiant buffer
ALuint AudioAmbience::GetAmbiantBuffer()
{
	return _buffers[0];
}

//Add a source
void AudioAmbience::AddSource()
{
	if (_nb_sources < NB_SOURCES_MAX)
	{
		AudioSource s = AudioSource();
		_sources.insert(pair<ALuint, AudioSource>(s.getId(), s));
		_id_sources[_nb_sources] = s.getId();
		_nb_sources++;
	}
	else
	{
		std::cerr << "ERROR - Limit of sources reached" << std::endl;
		return;
	}
}


//Link a buffer to a source 
void AudioAmbience::LinkBufferSource(ALuint buffer, ALuint id_source)
{
	_sources.at(id_source).setBuffer(buffer);
}

//Link an array of buffer to a source
void AudioAmbience::LinkQueueBuffersSource(ALuint* buffers, ALuint id_source, ALsizei size)
{
	alSourceQueueBuffers(id_source, size, buffers);

}

//Link the ambiant buffer to the ambiant source
void AudioAmbience::LinkAmbiantBufferSource()
{
	_sources.at(GetIdSource(0)).setBuffer(GetBuffer(0));
}

//Link the ambiant buffer to the ambiant source
void AudioAmbience::LinkStepBufferSource()
{
	_sources.at(GetIdSource(1)).setBuffer(GetBuffer(1));
}

//Link a buffer to the first free source
void AudioAmbience::LinkAutoBufferSource(ALuint buffer)
{
	int i = 1;
	int verif;
	alGetSourcei(_id_sources[i], AL_BUFFER, &verif);
	while (verif != 0)
	{
		i++;
		alGetSourcei(_id_sources[i], AL_BUFFER, &verif);
	}
	_sources.at(GetIdSource(1)).setBuffer(buffer);
}




//Load a sound into a buffer
ALuint AudioAmbience::LoadSound(const std::string& Filename)
{
	SF_INFO FileInfos;
	SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);
	if (!File)
	{
		std::cerr << "ERROR - Can't open the audio file" << std::endl;
		return 0;
	}

	ALsizei NbSamples = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
	ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);

	std::vector<ALshort> Samples(NbSamples);
	if (sf_read_short(File, &Samples[0], NbSamples) < NbSamples)
	{
		std::cerr << "ERROR - Can't read samples stored in the audio file" << std::endl;
		return 0;
	}

	sf_close(File);

	ALenum Format;
	switch (FileInfos.channels)
	{
	case 1: Format = AL_FORMAT_MONO16;   break;
	case 2: Format = AL_FORMAT_STEREO16; break;
	default:
		std::cerr << "ERROR - Audio Format unsupported (more than 2 channel)" << std::endl;
		return 0;
	}

	ALuint Buffer;
	alGenBuffers(1, &Buffer);

	alBufferData(Buffer, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);

	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "ERROR - Can't fil the OpenAL buffer with the samples from the audio file" << std::endl;
		return 0;
	}

	return Buffer;
}

//Play a sound from a source
void AudioAmbience::PlaySource(ALuint id_source)
{
	ALint Status;
	alGetSourcei(id_source, AL_SOURCE_STATE, &Status);
	if (Status != AL_PLAYING)
	{
		alSourcePlay(id_source);
	}
}

//Play all the sources 
void AudioAmbience::PlayAllSources()
{
	alSourcePlayv(_nb_sources, _id_sources);
}

//Return true if the listener is close enough, false otherwise
bool AudioAmbience::CheckDistance(ALuint id_source)
{
	float limit = 50.0f;
	float temp = GetASource(id_source).getDistanceFromListener().x * GetASource(id_source).getDistanceFromListener().x +
		GetASource(id_source).getDistanceFromListener().y * GetASource(id_source).getDistanceFromListener().y +
		GetASource(id_source).getDistanceFromListener().z * GetASource(id_source).getDistanceFromListener().z;
	float norme = sqrt(temp);
	return norme < limit;
}


//Play a source with the generation function and check the distance
void AudioAmbience::PlaySourceAuto(ALuint id_source)
{
	ALint Status;
	alGetSourcei(id_source, AL_SOURCE_STATE, &Status);
	if (Status != AL_PLAYING && CheckDistance(id_source))
	{
		ProceduralSequence(id_source);
	}
	else if (!CheckDistance(id_source))
	{
		FreeSource(id_source);
	}
}


//Play all sources with the generation function and the distance checking
void AudioAmbience::PlayAllSourcesAuto()
{
	for (int i = 0; i < _nb_sources; i++)
	{
		if (i == 0) PlayAmbiantSource();
		else if (i == 1) PlayStepSource();
		else PlaySourceAuto(_id_sources[i]);
	}
}


//Play and Update the position of the ambiant source
void AudioAmbience::PlayAmbiantSource()
{
	ALfloat x_pos, y_pos, z_pos;
	alGetListener3f(AL_POSITION, &x_pos, &y_pos, &z_pos);
	_sources[_id_sources[0]].setPosition(x_pos, y_pos + 1.0f, z_pos);
	PlaySource(_id_sources[0]);
}

//Play and Update the position of the stepsource
void AudioAmbience::PlayStepSource()
{
	ALfloat x_pos, y_pos, z_pos;
	alGetListener3f(AL_POSITION, &x_pos, &y_pos, &z_pos);
	ALfloat x_step, y_step, z_step;
	alGetSource3f(_id_sources[1], AL_POSITION, &x_step, &y_step, &z_step);
	ALfloat distance = max(abs(x_pos - x_step), abs(z_pos - z_step));
	if (distance > 1.0f)
	{
		if (_step_pitch_diff == 0.2f) _step_pitch_diff = 0.0f;
		else _step_pitch_diff = 0.2f;
		ALfloat pitch = 0.9f + _step_pitch_diff;
		TransformSource_PITCH(_id_sources[1], pitch);
		_sources[_id_sources[1]].setPosition(x_pos, y_pos - 1.0f, z_pos);
		PlaySource(_id_sources[1]);
	}
}


//Play multiples buffers on a source
void AudioAmbience::PlayAllBuffers(ALuint id_source, vector<ALuint> tab_buffers)
{
	ALint Status;
	for (ALuint i = 0; i < tab_buffers.size(); i++)
	{
		LinkBufferSource(tab_buffers[i], id_source);
		PlaySource(id_source);
		alGetSourcei(id_source, AL_SOURCE_STATE, &Status);
		if (Status == AL_PLAYING)
		{
			i = i - 1;
		}
	}
}


//Make a silent buffer
ALuint AudioAmbience::CreateSilence(double time)
{
	int sampleRate = 48000;
	double sTime = time;     // How long to maintain silence.
	int sampleCount = int(sTime*sampleRate);
	int byteCount = sampleCount * sizeof(short);
	short* silence = (short*)malloc(byteCount);
	memset(silence, 0, byteCount);

	ALuint silenceBuffer;
	alGenBuffers(1, &silenceBuffer);
	alBufferData(silenceBuffer, AL_FORMAT_MONO16, silence, byteCount, sampleRate);

	delete silence;
	return silenceBuffer;
}


//Create a procedural sequence with sounds and silence between
void AudioAmbience::ProceduralSequence(ALuint id_source)
{
	srand(time(NULL));
	int taille = (((rand() * _procedural_counter) % _nb_buffers) + 1);

	ALuint* tab_buffers = new ALuint[NB_BUFFERS_MAX];
	int beg_sil = (((rand() * _procedural_counter))) % 2;

	for (int i = 0; i < taille; i++)
	{
		if (beg_sil == 0) {
			int num_buf = ((rand() * _procedural_counter) % (_nb_buffers - 2)) + 2;
			tab_buffers[i] = GetBuffer(num_buf);
			beg_sil = 1;
			_procedural_counter++;
		}
		else
		{
			double silent = ((rand() * _procedural_counter) % 10) + 1;
			tab_buffers[i] = CreateSilence(silent);
			beg_sil = 0;
			_procedural_counter++;
		}
		_procedural_counter++;
	}

	if (_procedural_counter > 1000) _procedural_counter = 0;

	LinkQueueBuffersSource(tab_buffers, id_source, taille);
	alSourcePlay(id_source);
	alDeleteBuffers(NB_BUFFERS_MAX, tab_buffers);
	delete[] tab_buffers;
}

//Transform the gain of a source
void AudioAmbience::TransformSource_GAIN(ALuint id_source, float modif)
{
	_sources.at(id_source).setGain(modif);
}

//Transform the pitch of a source
void AudioAmbience::TransformSource_PITCH(ALuint id_source, float modif)
{
	_sources.at(id_source).setPitch(modif);
}

//Transform the position x y z of a source
void AudioAmbience::TransformSource_POSITION(ALuint id_source, float x, float y, float z)
{
	_sources.at(id_source).setPosition(x, y, z);
}

//Set the roll of factor, the reference distance et the distance max of the distance model of a source
void AudioAmbience::SetDistanceModel(ALuint id_source, ALfloat rollOf, ALfloat referenceDistance, ALfloat maxDistance)
{
	_sources.at(id_source).setRollOfFactor(rollOf);
	_sources.at(id_source).setReferenceDistance(referenceDistance);
	_sources.at(id_source).setMaxDistance(maxDistance);
}

//Release 
void AudioAmbience::FreeSource(ALuint id_source)
{
	alSourceStop(id_source);
	GetASource(id_source).setBuffer(0);
}


//Shutdown openAl
void AudioAmbience::ShutdownOpenAL()
{
	ALCcontext* Context = alcGetCurrentContext();
	ALCdevice*  Device = alcGetContextsDevice(Context);

	alcMakeContextCurrent(NULL);

	alcDestroyContext(Context);

	alcCloseDevice(Device);
}

//Shutdown openAL, delete buffers and sources
void AudioAmbience::cleanAll()
{
	alDeleteBuffers(NB_BUFFERS_MAX, _buffers);

	int i = 0;
	while (_id_sources[i] != NULL)
	{
		alSourcei(_id_sources[i], AL_BUFFER, 0);
		alDeleteSources(1, &_id_sources[i]);
		i++;
	}

	delete[] _buffers;
	delete[] _id_sources;

	ShutdownOpenAL();
}



