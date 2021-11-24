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

void Audio::PlayBGM(const char *filename, bool LOOP)
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

	FormatChunk format = {};
	file.read((char *)&format, sizeof(format));

	Chunk data;
	file.read((char *)&data, sizeof(data));

	char *pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();

	WAVEFORMATEX wfex{};
	// 波形フォーマットの設定
	memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;


	//波形フォーマットを元にSourceVoiceの生成
	result = xAudio2->CreateSourceVoice(&pBgmSourceVoice, &wfex, 0, 2.0f, &voiceCallback);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE *)pBuffer;
	buf.pContext = pBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = data.size;
	if (LOOP == TRUE)
	{
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	//波形データの再生
	result = pBgmSourceVoice->SubmitSourceBuffer(&buf);

	result = pBgmSourceVoice->Start();
}

void Audio::PlaySE(const char *filename, bool LOOP)
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

	FormatChunk format = {};
	file.read((char *)&format, sizeof(format));

	Chunk data;
	file.read((char *)&data, sizeof(data));

	char *pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();

	WAVEFORMATEX wfex{};
	// 波形フォーマットの設定
	memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;


	//波形フォーマットを元にSourceVoiceの生成
	result = xAudio2->CreateSourceVoice(&pSeSourceVoice, &wfex, 0, 2.0f, &voiceCallback);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE *)pBuffer;
	buf.pContext = pBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = data.size;
	if (LOOP == TRUE)
	{
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	//波形データの再生
	result = pSeSourceVoice->SubmitSourceBuffer(&buf);

	result = pSeSourceVoice->Start();
}

void Audio::StopBGM()
{
	pBgmSourceVoice->Stop();
}
