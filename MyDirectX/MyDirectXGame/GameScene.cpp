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
	maxframe = 50;
	EnemyBulletFrame = 0;
	EnemyBulletMaxframe = 100;
	shotTimer = 120;
	maxshotTimer = 300;

	enemyMoveFlag = 0;
	enemyFrame = 0;
	enemyMaxFrame = 100;
}

GameScene::~GameScene()
{
	safe_delete(spriteBG);
	safe_delete(title);
	safe_delete(clear);
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
	if (!Sprite::LoadTexture(2, L"Resources/title.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(3, L"Resources/clear.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(4, L"Resources/gameover.png")) {
		assert(0);
		return;
	}
	//// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	title = Sprite::Create(2, { 0.0f,0.0f });
	clear = Sprite::Create(3, { 0.0f,0.0f });
	gameover = Sprite::Create(4, { 0.0f,0.0f });
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
	audio->PlayBGM("Resources/Alarm01.wav", true);
	audio->PlaySE("Resources/Alarm01.wav", false);
	audio->StopBGM();
}

void GameScene::Update()
{
	//カメラの位置取得--------------------------------
	XMFLOAT3 cameraTarget = Object3d::GetTarget();
	XMFLOAT3 cameraEye = Object3d::GetEye();
	//マウスカーソル位置取得---------------------------
	POINT mousePos;
	GetCursorPos(&mousePos);
	//弾の位置取得-----------------------------------
	XMFLOAT3 position[20];
	for (int i = 0; i < 20; i++)
	{
		position[i] = playerObj[i]->GetPosition();
	}
	//敵の弾取得-------------------------------------
	XMFLOAT3 enemyBulletPosition[20];
	for (int i = 0; i < 20; i++)
	{
		enemyBulletPosition[i] = EnemyBullet[i]->GetPosition();
	}

	//スロー処理--------------------------------
	if (sceneNo == 0)
	{
		debugText.Print("title", 20, 20, 1.5f);
		if (input->TriggerKey(DIK_SPACE))
		{
			sceneNo = 1;
		}
	}
	else if (sceneNo == 1)
	{
		maxshotTimer++;

		if (input->TriggerKey(DIK_SPACE) && isSlow == 0)
		{
			isSlow = 1;
		}
		else if (input->TriggerKey(DIK_SPACE) && isSlow == 1)
		{
			isSlow = 0;
		}
		if (isSlow == 1 && slowValue > 0.0625)
		{
			slowValue -= 0.03125;
		}
		else if (isSlow == 0 && slowValue < 1.0)
		{
			slowValue += 0.03125;
		}

		//ボスをターゲットにしたカメラ回転--------------------------------------
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
			if (input->PushKey(DIK_D)) { angle -= 5.0f * slowValue; }
			else if (input->PushKey(DIK_A)) { angle += 5.0f * slowValue; }

		}

		//弾の移動-----------------------------------------------------------------
		for (int i = 0; i < 20; i++)
		{
			if (input->TriggerMouse(0)) {
				if (BulletFlag[i] == false && shotTimer <= maxshotTimer) {
					BulletFlag[i] = true;
					frame = 0;
					position[i].x = f.x;
					position[i].z = f.z + 300;
					shotTimer = 0;
				}
			}

			if (BulletFlag[i] == true) {
				if (frame >= 0 && frame <= maxframe) {
					x = static_cast<float>(frame) / static_cast<float>(maxframe);
					position[i].x = f.x + ((position2.x - 5.48) - f.x) * (sin(x * PI / 2));
					position[i].z = f.z + ((position2.z - 8.8) - f.z) * (sin(x * PI / 2));
				}
			}
			if (frame > maxframe) {
				BulletFlag[i] = false;
			}
		}

		//敵の弾----------------------------------------------------------------
		float AngleX = f.x - position2.x;
		float AngleZ = f.z - position2.z;
		float Angle = atan2(AngleX, AngleZ);
		//レーザー--------------------------------------------------------------
		if (Angle < 2.5 && Angle < -1.9)
		{
			enemyMoveFlag = 1;
		}

		if (enemyMoveFlag == 1)
		{
			for (int i = 0; i < 20; i++)
			{

				if (EnemyBulletFlag[i] == false && EnemyBulletFrame >= EnemyBulletMaxframe)
				{
					EnemyBulletFlag[i] = true;
					enemyBulletPosition[i].z = position2.z;
					enemyBulletPosition[i].x = position2.x;
					enemyFrame = 0;
					EnemyBulletFrame = 0;
				}

				if (EnemyBulletFlag[i] == true && enemyFrame >= 0 && enemyFrame <= enemyMaxFrame)
				{
					enemyX = static_cast<float>(enemyFrame) / static_cast<float>(enemyMaxFrame);
					enemyBulletPosition[i].x = position2.x + (f.x - position2.x) * (sin(enemyX * PI / 2));
					enemyBulletPosition[i].z = position2.z + (f.z - position2.z) * (sin(enemyX * PI / 2));
				}
				if (enemyFrame >= enemyMaxFrame)
				{
					EnemyBulletFlag[i] = false;
					enemyMoveFlag = 0;
				}
			}
		}

		//カメラY軸に対する首振り---------------------------
		float mouseAngle = ((1080 - mousePos.y) - 540) * 4;
		cameraTarget.y = XMConvertToRadians(mouseAngle);
		Object3d::SetTarget(cameraTarget);

		//ポジションをセット-----------------
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
		//タイマー-------------------------
		EnemyBulletFrame++;
		shotTimer++;
		frame++;
		enemyFrame++;
		//デバッグテキスト-------------------
		char str[256];
		sprintf_s(str, "%f  position %f %f, flag = %d %f", enemyFrame, f.z, f.x, enemyMoveFlag, Angle);
		debugText.Print(str, 20, 20, 1.5f);

		float r = 1;
		for (int i = 0; i < 20; i++)
		{
			hit[i] = Collision::ChenkSphere2Sphere(position[i].x, position[i].y, position[i].z, position2.x - 5.48f, position2.y + 10.0f, position2.z - 8.8f, 1.0f, 1.0f);
			if (hit[i] == true)
			{
				debugText.Print("EyeTarget:SPACE Q LCONTROL E", 20, 120 + (3 * i), 1.5f);
				//sceneNo = 2;
			}
		}
	}
	else if (sceneNo == 2)
	{
	debugText.Print("Clear", 20, 20, 1.5f);
	if (input->TriggerKey(DIK_SPACE))
	{
		sceneNo = 4;
	}
	}
	else if (sceneNo == 3)//初期化用
	{
	debugText.Print("gameover", 20, 20, 1.5f);
	if (input->TriggerKey(DIK_SPACE))
	{
		sceneNo = 4;
	}
}
	else if (sceneNo == 4)//初期化用
	{
	debugText.Print("gameover", 20, 20, 1.5f);
	sceneNo = 0;
	}
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	if (sceneNo == 0)
	{
		spriteBG->Draw();
	}
	else if (sceneNo == 1)
	{

	}
	else if (sceneNo == 2)
	{
		spriteBG->Draw();
	}
	else if (sceneNo == 3)
	{
		spriteBG->Draw();
	}
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
		if (EnemyBulletFlag[i] == true && enemyMoveFlag == 1) {
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
	if (sceneNo == 0)
	{
		title->Draw();
	}
	else if (sceneNo == 1)
	{

	}
	else if (sceneNo == 2)
	{
		clear->Draw();
	}
	else if (sceneNo == 3)
	{
		gameover->Draw();
	}
	///// <summary>
	///// ここに前景スプライトの描画処理を追加できる
	///// </summary>

	// デバッグテキストの描画
	debugText.DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}
