#include "GameScene.h"
#include <cassert>

using namespace DirectX;

GameScene::GameScene()
{
	for (int i = 0; i < 20; i++)
	{
		BulletFlag[i] = false;
	}

	for (int i = 0; i < 20; i++)
	{
		EnemyBulletFlag[i] = false;
	}

	frame = 0;
	maxframe = 200;
	EnemyBulletFrame = 0;
	EnemyBulletMaxframe = 100;
	shotTimer = 120;
	maxshotTimer = 120;
}

GameScene::~GameScene()
{
	safe_delete(spriteBG);
	safe_delete(playerModel);
	safe_delete(EnemyBulletModel);
	for (int i = 0; i < 20; i++)
	{
		safe_delete(playerObj[i]);
	}

	for (int i = 0; i < 20; i++)
	{
		safe_delete(EnemyBullet[i]);
	}

	safe_delete(playerModel2);
	safe_delete(playerObj2);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	// デバッグテキスト用テクスチャ読み込み
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png")) {
		assert(0);
		return;
	}
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	if (!Sprite::LoadTexture(1, L"Resources/background.png")) {
		assert(0);
		return;
	}
	//// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	// 3Dオブジェクト生成


	playerModel = playerModel->CreateFromObject("bullet");
	for (int i = 0; i < 20; i++)
	{
		playerObj[i] = Object3d::Create();
		playerObj[i]->LinkModel(playerModel);
		playerObj[i]->SetPosition({ -5.0f, 0.0f, -100.0f });
		playerObj[i]->SetScale({ 0.5f,0.5f,0.5f });
		playerObj[i]->Update();
	}

	EnemyBulletModel = EnemyBulletModel->CreateFromObject("bullet");
	for (int i = 0; i < 20; i++)
	{
		EnemyBullet[i] = Object3d::Create();
		EnemyBullet[i]->LinkModel(EnemyBulletModel);
		EnemyBullet[i]->SetPosition({ -5.0f, 0.0f, 0.0f });
		EnemyBullet[i]->SetScale({ 0.5f,0.5f,0.5f });
		EnemyBullet[i]->Update();
	}

	playerModel = playerModel2->CreateFromObject("temp");
	playerObj2 = Object3d::Create();
	playerObj2->LinkModel(playerModel);
	playerObj2->SetPosition({ +5.48f, -10.0f, +8.8f });
	playerObj2->SetScale({ 4.0f,4.0f,4.0f });
	playerObj2->Update();

	skydomeModel = skydomeModel->CreateFromObject("skydome");
	skydomeObj = Object3d::Create();
	skydomeObj->LinkModel(skydomeModel);
	skydomeObj->SetScale({ 5.0f,5.0f,5.0f });
	skydomeObj->Update();
	//サウンド再生
	//audio->PlayWave("Resources/Alarm01.wav");
}

void GameScene::Update()
{
	maxshotTimer++;
	//debugText.Print("ObjectMove:ArrowKey", 20, 20, 1.5f);
	//debugText.Print("EyeMove:W A S D", 20, 50, 1.5f);
	//debugText.Print("EyeTarget:SPACE Q LCONTROL E", 20, 80, 1.5f);

	if (input->PushKey(DIK_1) || input->PushKey(DIK_2))
	{
		if (input->PushKey(DIK_1))
		{
			isSlow = 1;
		}
		if (input->PushKey(DIK_2))
		{
			isSlow = 0;
		}
	}
	if (isSlow == 1 && slowValue > 0.0625)
	{
		slowValue -= 0.03125;
	} else if (isSlow == 0 && slowValue < 1.0)
	{
		slowValue += 0.03125;
	}

	XMFLOAT3 cameraTarget = Object3d::GetTarget();
	XMFLOAT3 cameraEye = Object3d::GetEye();

	POINT mousePos;
	GetCursorPos(&mousePos);

	XMFLOAT3 position[20];
	for (int i = 0; i < 20; i++)
	{
		position[i] = playerObj[i]->GetPosition();
	}

	

	XMFLOAT3 position2 = playerObj2->GetPosition();
	XMVECTOR v0 = { 0, 0, -50, 0 };
	//angleラジアンだけy軸まわりに回転。半径は-100
	XMMATRIX rotM = XMMatrixRotationY(XMConvertToRadians(angle));
	XMVECTOR v = XMVector3TransformNormal(v0, rotM);
	XMVECTOR bossTarget = { position2.x - 5.48,  position2.y + 10,  position2.z - 8.8 };
	XMVECTOR v3 = bossTarget + v;
	XMFLOAT3 f = { v3.m128_f32[0], v3.m128_f32[1], v3.m128_f32[2] };
	cameraTarget = { bossTarget.m128_f32[0], bossTarget.m128_f32[1], bossTarget.m128_f32[2] };
	cameraEye = f;

	Object3d::SetTarget(cameraTarget);
	Object3d::SetEye(cameraEye);

	// カメラ移動
	if (input->PushKey(DIK_D) || input->PushKey(DIK_A))
	{
		if (input->PushKey(DIK_D)) { angle += 5.0f * slowValue; } else if (input->PushKey(DIK_A)) { angle -= 5.0f * slowValue; }

	}

	for (int i = 0; i < 20; i++)
	{

		if (input->PushKey(DIK_SPACE)) {
			if (BulletFlag[i] == false && shotTimer <= maxshotTimer) {
				BulletFlag[i] = true;
				frame = 0;
				position[i].x = f.x;
				position[i].z = f.z;
				shotTimer = 0;
			}
		}

		if (BulletFlag[i] == true) {
			shotTimer++;
			if (frame >= 0 && frame <= maxframe) {
				frame++;
				x = static_cast<float>(frame) / static_cast<float>(maxframe);
				position[i].x = f.x + ((position2.x - 5.48) - f.x) * (sin(x * PI / 2));
				position[i].z = f.z + ((position2.z - 8.8) - f.z) * (sin(x * PI / 2));

			}
		}
		if (frame > maxframe) {
			BulletFlag[i] = false;
		}
	}

	XMFLOAT3 enemyBulletPosition[20];
	for (int i = 0; i < 20; i++)
	{
		enemyBulletPosition[i] = EnemyBullet[i]->GetPosition();
	}
	
	
	for (int i = 0; i < 20; i++)
	{
		
		if (EnemyBulletFlag[i] == false && EnemyBulletFrame >= EnemyBulletMaxframe)
		{
			EnemyBulletFlag[i] = true;
			enemyBulletPosition[i].z = position2.z;
		}

		if (EnemyBulletFlag[i] == true)
		{
			enemyBulletPosition[i].z += 0.5f;
			if (enemyBulletPosition[i].z >= 300.0f)
			{
				EnemyBulletFlag[i] = false;
				EnemyBulletFrame = 0;
				enemyBulletPosition[i].z = 0;
			}
		}
	}


	// カメラ移動
	//if (input->PushKey(DIK_Q) || input->PushKey(DIK_E) || input->PushKey(DIK_LCONTROL) || input->PushKey(DIK_SPACE))
	//{
	//	if (input->PushKey(DIK_SPACE)) { Object3d::CameraMoveEyeVector({ 0.0f,+1.0f,0.0f }); }
	//	else if (input->PushKey(DIK_LCONTROL)) { Object3d::CameraMoveEyeVector({ 0.0f,-1.0f,0.0f }); }
	//	if (input->PushKey(DIK_E)) { Object3d::CameraMoveEyeVector({ +1.0f,0.0f,0.0f }); }
	//	else if (input->PushKey(DIK_Q)) { Object3d::CameraMoveEyeVector({ -1.0f,0.0f,0.0f }); }
	//}

	float mouseAngle = ((1080 - mousePos.y) - 540) * 4;
	cameraTarget.y = XMConvertToRadians(mouseAngle);
	Object3d::SetTarget(cameraTarget);

	for (int i = 0; i < 20; i++)
	{
		playerObj[i]->SetPosition(position[i]);
		playerObj[i]->Update();
	}
	for (int i = 0; i < 20; i++)
	{
		EnemyBullet[i]->SetPosition(enemyBulletPosition[i]);
		EnemyBullet[i]->Update();
	}
	playerObj2->Update();
	skydomeObj->Update();
	char str[256];
	sprintf_s(str, "%f, %f, %f  position %f", frame, shotTimer, EnemyBulletFrame, enemyBulletPosition[0].z);
	debugText.Print(str, 20, 20, 1.5f);
	EnemyBulletFrame++;
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	spriteBG->Draw();

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(cmdList);

	// 3Dオブクジェクトの描画
	skydomeObj->Draw();
	for (int i = 0; i < 20; i++)
	{
		if (BulletFlag[i] == true) {
			playerObj[i]->Draw();
		}
	}

	for (int i = 0; i < 20; i++)
	{
		if (EnemyBulletFlag[i] == true) {
			EnemyBullet[i]->Draw();
		}
	}

	playerObj2->Draw();
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	//// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	///// <summary>
	///// ここに前景スプライトの描画処理を追加できる
	///// </summary>

	// デバッグテキストの描画
	debugText.DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}
