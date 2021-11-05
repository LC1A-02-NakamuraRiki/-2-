#include "Audio.h"
#include <fstream>
#include <cassert>

#pragma comment(lib,"xaudio2.lib")

bool Audio::Initialize()
{
    HRESULT result;

    result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

    result = xAudio2->CreateMasteringVoice(&masterVoice);

    return true;
}

void Audio::PlayWave(const char *filename)
{
	HRESULT result;

	std::ifstream file;

	file.open(filename, std::ios_base::binary);

	RiffHeader riff;
	file.read((char *)&riff, sizeof(riff));

	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}

	//if (strncmp(riff.type, "WAVE", 4) != 0)
	//{
	//	assert(0);
	//}


	FormatChunk format = {};
	file.read((char *)&format, sizeof(format));


	//assert(format.chunk.size <= sizeof(format.fmt));
	//file.read((char *)&format.fmt, format.chunk.size);

	Chunk data;
	file.read((char *)&data, sizeof(data));

	char *pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();

	//if (strncmp(data.id, "JUNK", 4) == 0)
	//{
	//	file.seekg(data.size, std::ios_base::cur);

	//	file.read((char *)&data, sizeof(data));
	//}

	//if (strncmp(data.id, "data", 4) != 0)
	//{
	//	assert(0);
	//}

	//SoundData soundData = {};

	WAVEFORMATEX wfex{};
	// 波形フォーマットの設定
	memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;


	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice *pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex,0,2.0f,&voiceCallback);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE *)pBuffer;
	buf.pContext = pBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = data.size;

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	
	result = pSourceVoice->Start();
}
