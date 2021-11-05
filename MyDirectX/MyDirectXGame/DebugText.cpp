#include "DebugText.h"

DebugText::DebugText()
{
}

DebugText::~DebugText()
{
	for (int i = 0; i < _countof(spriteDatas); i++) {
		delete spriteDatas[i];
	}
}

void DebugText::Initialize(UINT texnumber)
{
	for (int i = 0; i < _countof(spriteDatas); i++)
	{
		spriteDatas[i] = Sprite::Create(texnumber, { 0,0 });
	}
}

void DebugText::Print(const std::string &text, float x, float y, float size = 1.0f)
{
	//全ての文字について
	for (int i = 0; i < text.size(); i++)
	{
		//最大文字数超過
		if (spriteIndex >= maxCharCount)
		{
			break;
		}

		//1文字取り出す(ASCIIコードでしか成り立たない)
		const unsigned char &character = text[i];

		//ASCIIコードの2段分飛ばした番号を計算
		int fontIndex = character - 32;
		if (character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		//座標計算
		spriteDatas[spriteIndex]->SetPosition({ x + fontWidth * size * i, y });
		spriteDatas[spriteIndex]->SetTextureRect({ (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight }, { (float)fontWidth, (float)fontHeight });
		spriteDatas[spriteIndex]->SetSize({ fontWidth * size, fontHeight * size });

		// 文字を１つ進める
		spriteIndex++;
	}
}

void DebugText::DrawAll(ID3D12GraphicsCommandList *cmdList)
{
	//全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++)
	{
		//スプライト描画
		spriteDatas[i]->Draw();
	}

	spriteIndex = 0;
}
