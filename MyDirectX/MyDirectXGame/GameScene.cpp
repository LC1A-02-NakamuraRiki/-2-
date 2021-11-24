#include "GameScene.h"
#include <cassert>
#include<time.h>

using namespace DirectX;

GameScene::GameScene()
{

	BulletFlag = false;

	for (int i = 0; i < EnemyBulletNum; i++)
	{
		EnemyBulletFlag[i] = false;
		EnemyBulletFlag2 = true;
	}

	frame = 0;
	maxframe = 50;
	EnemyBulletFrame = 0;
	EnemyBulletMaxframe = 100;
	EnemyBulletFrame2 = 0;
	EnemyBulletMaxframe2 = 100;

	EnemybullTimer = 120;
	srand(time(NULL));

	enemyMoveFlag = 0;
	enemyFrame = 0;
	enemyMaxFrame = 100;

	bosssHP = 30;

	active = 0;

	shakeCount = 0;
	shakeX = 0.0f;
	shakeY = 0.0f;
	shakeZ = 0.0f;
	shakeFlag = false;

	lFrame = 0;
	maxlFrame = 50;

	animationTimer = 0;
	animationCount = 0;

	clearTimer = 0;
	clearCount = 0;
	playersHP = 5;
}

GameScene::~GameScene()
{
	safe_delete(spriteBG);
	for (int i = 0; i < 12; i++) {
		safe_delete(title[i]);
	}
	for (int i = 0; i < 9; i++) {
		safe_delete(clear[i]);
	}
	safe_delete(warningMark);
	safe_delete(slowUi);
	safe_delete(playerModel);
	safe_delete(EnemyBulletModel);
	for (int i = 0; i < 20; i++)
	{
		safe_delete(playerObj);
	}

	for (int i = 0; i < EnemyBulletNum; i++)
	{
		safe_delete(EnemyBullet[i]);
		safe_delete(EnemyBullet2);
	}

	safe_delete(playerModel2);
	safe_delete(playerObj2);
	safe_delete(pressObj);
	safe_delete(pressModel);
	safe_delete(playerDroneObj);
	safe_delete(playerDroneModel);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// nullptr�`�F�b�N
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	// �f�o�b�O�e�L�X�g�p�e�N�X�`���ǂݍ���
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png")) {
		assert(0);
		return;
	}
	// �f�o�b�O�e�L�X�g������
	debugText.Initialize(debugTextTexNumber);

	// �e�N�X�`���ǂݍ���
	if (!Sprite::LoadTexture(1, L"Resources/background.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(2, L"Resources/gameover.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(3, L"Resources/warningMark.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(4, L"Resources/title1.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(5, L"Resources/title2.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(6, L"Resources/title3.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(7, L"Resources/title4.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(8, L"Resources/title5.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(9, L"Resources/title6.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(10, L"Resources/title7.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(11, L"Resources/title8.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(12, L"Resources/title9.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(13, L"Resources/title10.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(14, L"Resources/title11.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(15, L"Resources/title12.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(16, L"Resources/clear1.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(17, L"Resources/clear2.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(18, L"Resources/clear3.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(19, L"Resources/clear4.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(20, L"Resources/clear5.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(21, L"Resources/clear6.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(22, L"Resources/clear7.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(23, L"Resources/clear8.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(24, L"Resources/clear9.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(25, L"Resources/slowUI.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(26, L"Resources/playerHP.png")) {
		assert(0);
		return;
	}
	if (!Sprite::LoadTexture(27, L"Resources/enemyHP.png")) {
		assert(0);
		return;
	}
	//// �w�i�X�v���C�g����
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	for (int i = 0; i < 12; i++) {
		title[i] = Sprite::Create(4 + i, { 0.0f,0.0f });
	}
	for (int i = 0; i < 9; i++) {
		clear[i] = Sprite::Create(16 +i, { 0.0f,0.0f });
	}
	gameover = Sprite::Create(2, { 0.0f,0.0f });
	warningMark = Sprite::Create(3, { 0.0f,0.0f });

	slowUi = Sprite::Create(25, { 0.0f,0.0f });
	slowUi->SetSize({ 512.0f,16.0f });
	slowUi->SetPosition({ 32.0f,656.0f });

	playerHP = Sprite::Create(26, { 0.0f,0.0f });
	playerHP->SetSize({ 256.0f,16.0f });
	playerHP->SetPosition({ 32.0f,688.0f });

	enemyHP = Sprite::Create(27, { 0.0f,0.0f });
	enemyHP->SetSize({ 1216.0f,32.0f });
	enemyHP->SetPosition({ 32.0f,32.0f });

	// 3D�I�u�W�F�N�g����


	playerModel = playerModel->CreateFromObject("bullet");


	playerObj = Object3d::Create();
	playerObj->LinkModel(playerModel);
	playerObj->SetPosition({ -5.0f, 0.0f, -100.0f });
	playerObj->SetScale({ 0.5f,0.5f,0.5f });
	playerObj->Update();

	playerDroneModel = playerDroneModel->CreateFromObject("player");
	playerDroneObj = Object3d::Create();
	playerDroneObj->LinkModel(playerDroneModel);
	playerDroneObj->SetPosition({ 0.0f, -10.0f, -55.0f });
	playerDroneObj->SetScale({ 5.0f,5.0f,5.0f });
	playerDroneObj->Update();

	EnemyBulletModel = EnemyBulletModel->CreateFromObject("enemyBullet");
	EnemyBulletModel2 = EnemyBulletModel2->CreateFromObject("lazer");
	for (int i = 0; i < EnemyBulletNum; i++)
	{
		EnemyBullet[i] = Object3d::Create();
		EnemyBullet[i]->LinkModel(EnemyBulletModel);
		EnemyBullet[i]->SetPosition({ -5.0f, -2.0f, 0.0f });
		EnemyBullet[i]->SetScale({ 0.5f,0.5f,0.5f });
		EnemyBullet[i]->Update();

		
	}

	EnemyBullet2 = Object3d::Create();
	EnemyBullet2->LinkModel(EnemyBulletModel2);
	EnemyBullet2->SetPosition({ 0.0f, -5.0f, 0.0f });
	EnemyBullet2->SetScale({ 5.0f,5.0f,5.0f });
	EnemyBullet2->Update();

	playerModel = playerModel2->CreateFromObject("temprobot");
	playerObj2 = Object3d::Create();
	playerObj2->LinkModel(playerModel);
	playerObj2->SetRotation({ 0.0f,0.0f,0.0f });
	playerObj2->SetPosition({ 0.0f, -4.0f, 0.0f });
	playerObj2->SetScale({ 5.0f,5.0f,5.0f });
	playerObj2->Update();

	pressModel = pressModel->CreateFromObject("temprobot");
	pressObj = Object3d::Create();
	pressObj->LinkModel(pressModel);
	pressObj->SetRotation({ 0.0f,0.0f,0.0f });
	pressObj->SetPosition({ 0.0f, -4.0f, 0.0f });
	pressObj->SetScale({ 5.0f,5.0f,5.0f });
	pressObj->Update();

	skydomeModel = skydomeModel->CreateFromObject("skydome");
	skydomeObj = Object3d::Create();
	skydomeObj->LinkModel(skydomeModel);
	skydomeObj->SetScale({ 5.0f,5.0f,5.0f });
	skydomeObj->Update();
	//�T�E���h�Đ�
	//audio->PlayWave("Resources/Alarm01.wav");
	audio->PlayBGM("Resources/Title.wav", true);
	//audio->PlaySE("Resources/Alarm01.wav", false);
}

void GameScene::Update()
{

	//�J�����̈ʒu�擾--------------------------------
	XMFLOAT3 cameraTarget = Object3d::GetTarget();
	XMFLOAT3 cameraEye = Object3d::GetEye();
	//�}�E�X�J�[�\���ʒu�擾---------------------------
	POINT mousePos;
	GetCursorPos(&mousePos);
	//�e�̈ʒu�擾-----------------------------------
	XMFLOAT3 position;

	position = playerObj->GetPosition();

	//�G�̒e�擾-------------------------------------
	XMFLOAT3 enemyBulletPosition[EnemyBulletNum];
	XMFLOAT3 enemyBulletPosition2;
	for (int i = 0; i < EnemyBulletNum; i++)
	{
		enemyBulletPosition[i] = EnemyBullet[i]->GetPosition();
		enemyBulletPosition2 = EnemyBullet2->GetPosition();
	}


	//�X���[����--------------------------------
	if (sceneNo == 0) {
		if (input->TriggerKey(DIK_SPACE)) {
			isChange = 1;

			Object3d::SetEye(cameraEye);
			Object3d::SetTarget(cameraTarget);
		}

		if (isChange == true) {
			nowTime += 0.01;
			timeRate = min(nowTime / endTime, 1);

			cameraEye.x = 75 * (1.0f - (timeRate * timeRate) * (3 - (2 * timeRate))) + 0 * (timeRate * timeRate) * (3 - (2 * timeRate));
			cameraEye.y = 30 * (1.0f - (timeRate * timeRate) * (3 - (2 * timeRate))) + 0 * (timeRate * timeRate) * (3 - (2 * timeRate));
			cameraEye.z = -50 * (1.0f - (timeRate * timeRate) * (3 - (2 * timeRate))) + -50 * (timeRate * timeRate) * (3 - (2 * timeRate));
			cameraTarget.x = -90 * (1.0f - (timeRate * timeRate) * (3 - (2 * timeRate))) + 0 * (timeRate * timeRate) * (3 - (2 * timeRate));
			cameraTarget.y = 40 * (1.0f - (timeRate * timeRate) * (3 - (2 * timeRate))) + 0 * (timeRate * timeRate) * (3 - (2 * timeRate));

			Object3d::SetEye(cameraEye);
			Object3d::SetTarget(cameraTarget);
		}
		if (timeRate == 1) {
			sceneNo = 1;
			isChange = false;
			nowTime = 0;
			timeRate = 0;
			audio->StopBGM();
			audio->PlayBGM("Resources/bgm.wav", true);
		}

		playerObj2->Update();
		skydomeObj->Update();
		playerObj->Update();
		for (int i = 0; i < EnemyBulletNum; i++)
		{
			EnemyBullet[i]->Update();
			EnemyBullet2->Update();
		}
		//�A�j���[�V����
		animationTimer++;
		if (animationTimer >= 5)
		{
			animationCount++;
			animationTimer = 0;
			if (animationCount == 11)
			{
				animationCount = 0;
			}
		}
		playerDroneObj->Update();
	} else if (sceneNo == 1)
	{
		
		playerStopCount++;

		if (input->PushKey(DIK_SPACE) && slowCount > 0 && delay == 0)
		{
			isSlow = 1;
		}
		else
		{
			isSlow = 0;
		}
		if (isSlow == 1 && slowValue > 0.25)
		{
			slowValue -= 0.25;
		}
		else if (isSlow == 0 && slowValue < 1.0)
		{
			slowValue += 0.25;
		}

		//������
		if (isSlow == 1)
		{
			slowCount -= 1;
		}
		//����
		if (isSlow == 0 && slowCount <= 99)
		{
			slowCount += 1;
		}

		if (slowCount < 1)
		{
			isSlow = 0;
			delay = 1;
		}

		if (delay == 1)
		{
			slowDelay++;
		}

		if (slowDelay > 30)
		{
			delay = 0;
			slowDelay = 0;
		}
		slowUi->SetSize({ 512.0f * (slowCount / 100),16.0f});
		playerHP->SetSize({ 256.0f * (playersHP / 5),16.0f});
		enemyHP->SetSize({ 1216.0f * (bosssHP / 30) ,32.0f });
		//�{�X���^�[�Q�b�g�ɂ����J������]--------------------------------------
		XMFLOAT3 position2 = playerObj2->GetPosition();
		XMVECTOR v0 = { 0, 0, -50, 0 };
		//angle���W�A������y���܂��ɉ�]�B���a��-100
		XMMATRIX rotM = XMMatrixRotationY(XMConvertToRadians(angle));
		XMVECTOR v = XMVector3TransformNormal(v0, rotM);
		XMVECTOR bossTarget = { 0,0,0 };
		XMVECTOR v3 = bossTarget + v;
		XMFLOAT3 f = { v3.m128_f32[0], v3.m128_f32[1], v3.m128_f32[2] };
		cameraTarget = { bossTarget.m128_f32[0], bossTarget.m128_f32[1], bossTarget.m128_f32[2] };
		cameraEye = f;

		Object3d::SetTarget(cameraTarget);
		Object3d::SetEye(cameraEye);

		// �J�����ړ�
		if (input->PushKey(DIK_D) || input->PushKey(DIK_A))
		{
			playerStopCount = 0;
			if (input->PushKey(DIK_D)) { angle -= 2.0f * slowValue; } else if (input->PushKey(DIK_A)) { angle += 2.0f * slowValue; }

		}

		if (playerStopCount > 80)
		{
			nowPressAttack = 1;
		}

		//�e�̈ړ�-----------------------------------------------------------------

		if (input->TriggerMouse(0)) {
			if (BulletFlag == false) {
				BulletFlag = true;
				frame = 0;
				position.x = f.x;
				position.z = f.z + 300;
				audio->PlaySE("Resources/player_shoot.wav", false);
			}
		}

		if (BulletFlag == true) {
			if (frame >= 0 && frame <= maxframe) {
				x = static_cast<float>(frame) / static_cast<float>(maxframe);
				position.x = f.x + ((position2.x) - f.x) * (sin(x * PI / 2));
				position.z = f.z + ((position2.z) - f.z) * (sin(x * PI / 2));
			}
		}
		if (frame > maxframe) {
			BulletFlag = false;
		}

		/*---------------------�G�̒e----------------------*/
		float AngleX = position2.x - f.x;
		float AngleZ = position2.z - f.z;
		float Angle = atan2(AngleX, AngleZ);
		switch (active)
		{
		case 0:
			//���[�U�[--------------------------------------------------------------

			if (Angle < 1.0 && Angle > -1.0)
			{
				EnemyBulletFlag2 = true;
			} else {
				shakeFlag = true;
				EnemyBulletFlag2 = false;
				lFrame = 0;
				if (shakeCount >= 20)
				{
					active = 1;
				}
			}
			for (int i = 0; i < EnemyBulletNum; i++)
			{

				if (EnemyBulletFlag2 == false)
				{
					EnemyBulletFlag2 = true;
					enemyBulletPosition2.z = 0;
					enemyBulletPosition2.x = 0;
					lFrame = 0;
				}

				if (EnemyBulletFlag2 == true && lFrame == maxlFrame)
				{
					/*if (angle <= 0)
					{
						lAngleY -= 5.0f;
					} else {
						lAngleY += 5.0f;
					}*/
					
					EnemyBullet2->SetRotation({ 0,lAngleY,0 });
					lFrame = 0;
				}
				if (lAngleY == angle)
				{
					EnemyBulletFlag2 = false;
				}
			}
			break;

		case 1:

			//�e��--------------------------------------------
			for (int i = 0; i < EnemyBulletNum; i++)
			{
				if (EnemybullTimer <= 0)
				{
					if (EnemyBulletFlag[i] == false && EnemyBulletFrame >= EnemyBulletMaxframe)
					{
						EnemyBulletFlag[i] = true;
						enemyBulletPosition[i].z = position2.z;
						enemyBulletPosition[i].x = position2.x;
						bullAngle[i] = rand() + 1000 / 10000.f * (PI * 2);

					}
				}
				
				if (EnemyBulletFlag[i] == true)
				{
					if (EnemyBulletFrame == 120 && EnemyBulletFlag[i] == true)
					{
						audio->PlaySE("Resources/boss_SE_02.wav", false);
					}
					
					enemyBulletPosition[i].x += cos(bullAngle[i]) * 0.5 * slowValue;
					enemyBulletPosition[i].z += sin(bullAngle[i]) * 0.5 * slowValue;

					if (enemyBulletPosition[i].z > 50.0f || enemyBulletPosition[i].z < -50.0f
						|| enemyBulletPosition[i].x > 50.0f || enemyBulletPosition[i].x < -50.0f)
					{
						EnemyBulletFlag[i] = false;
						EnemyBulletFrame = 0;
						EnemybullTimer = 120;
						enemyBulletPosition[i].z = 0;
						enemyBulletPosition[i].x = 0;
					}
				}
				if (Angle <= 1.0 && Angle >= -1.0)
				{
					active = 0;
					lFrame = 0;
				}

			}
			break;
		case3:

			break;
		default:
			break;
		}

		lazerCount++;
		if (lazerCount == 60)
		{
			audio->PlaySE("Resources/boss_SE_01.wav", false);
		}
		if (lazerCount >= 120)
		{
			lazerCount = 0;
		}
		//�h��-------------------------------------------
		if (shakeCount > 20)
		{
			shakeCount = 0;
			shakeFlag = false;
			position2 = { 0,-4,0 };
		}
		if (shakeFlag == false)
		{
			shakeX = 0.0f;
			shakeY = 0.0f;
			shakeZ = 0.0f;
		} else
		{
			shakeX = rand() % 5 - 2.5;
			shakeY = rand() % 5 - 2.5;

			position2.x += shakeX;
			position2.z += shakeY;
			shakeCount++;
		}

		//�J����Y���ɑ΂����U��---------------------------
		float mouseAngle = ((1080 - mousePos.y) - 540) * 8;
		cameraTarget.y = XMConvertToRadians(mouseAngle);
		Object3d::SetTarget(cameraTarget);

		//�|�W�V�������Z�b�g-----------------

		playerObj->SetPosition(position);
		playerObj->Update();

		for (int i = 0; i < EnemyBulletNum; i++)
		{
			EnemyBullet[i]->SetPosition(enemyBulletPosition[i]);
			EnemyBullet[i]->Update();
			EnemyBullet2->SetPosition(enemyBulletPosition2);
			EnemyBullet2->Update();
		}
		playerObj2->SetPosition(position2);
		playerObj2->Update();
		skydomeObj->Update();
		//�^�C�}�[-------------------------
		EnemyBulletFrame += 1 * slowValue;
		EnemyBulletFrame2 += 1 * slowValue;

		frame += 1 * slowValue;
		enemyFrame += 1 * slowValue;
		EnemybullTimer -= 1 * slowValue;
		lFrame += 1 * slowValue;

		//�f�o�b�O�e�L�X�g-------------------
		/*char str[256];
		sprintf_s(str, "%f  position %f %f, flag = %d %f HP = %f", EnemyBulletFrame, angle, lAngleY, enemyMoveFlag, shakeCount, bosssHP);
		debugText.Print(str, 20, 20, 1.5f);*/


		if (nowPressAttack == 1)
		{
			XMFLOAT3 pressPos = pressObj->GetPosition();
			XMFLOAT3 bossRotation = pressObj->GetRotation();
			pressCount += 1 * slowValue;

			if (pressCount <= 1)
			{
				pressPos.y = 30;
			}
			if (pressCount == 40)
			{
				audio->PlaySE("Resources/face_crash.wav", false);
			}
			if (pressCount <= 40)
			{
				pressPos.x = cameraEye.x;
				pressPos.z = cameraEye.z;
				bossRotation.y += 5 * slowValue;
			} else if (pressCount >= 50)
			{
				if (pressPos.y >= -8)
				{
					pressPos.y -= 2 * slowValue;
				}
			}
			if (pressCount >= 90)
			{
				nowPressAttack = 0;
				bossRotation.y = 0;
				pressPos.y = 30;
				pressCount = 0;
			}
			pressObj->SetRotation(bossRotation);
			pressObj->SetPosition(pressPos);
			pressObj->Update();
		}

		playerBulletHit = Collision::ChenkSphere2Sphere(position.x, position.y, position.z, position2.x, position2.y, position2.z, 1.0f, 10.0f);
		if (playerBulletHit == true && BulletFlag == true)
		{
			playerBulletHit = false;
			BulletFlag = false;
			frame = 0;
			bosssHP -= 1;
			audio->PlaySE("Resources/boss_damage.wav", false);
			//debugText.Print("EyeTarget:SPACE Q LCONTROL E", 20, 120 + 3, 1.5f);
		}

		for (int i = 0; i < EnemyBulletNum; i++)
		{
			laserHit = Collision::ChenkSphere2Sphere(cameraEye.x, cameraEye.y, cameraEye.z, 0, 0, -50, 2.0f, 1.0f);
			if (laserHit == true && lazerCount >= 60)
			{
				playersHP -= 0.001;
				//laserHit = false;
				//debugText.Print("EyeTarget:SPACE Q LCONTROL E", 20, 120 + 3, 1.5f);
				audio->PlaySE("Resources/player_damage.wav", false);
			}

			barrageHit = Collision::ChenkSphere2Sphere(cameraEye.x, cameraEye.y, cameraEye.z, enemyBulletPosition[i].x, enemyBulletPosition[i].y, enemyBulletPosition[i].z, 1.2f, 1.2f);
			if (barrageHit == true && EnemyBulletFlag[i] == true)
			{
				playersHP--;
				barrageHit = false;
				EnemyBulletFlag[i] = false;
				//debugText.Print("EyeTarget:SPACE Q LCONTROL E", 20, 120 + 3, 1.5f);
				audio->PlaySE("Resources/player_damage.wav", false);
			}
		}
		XMFLOAT3 pressPos = pressObj->GetPosition();
		XMFLOAT3 bossRotation = pressObj->GetRotation();
		pressHit = Collision::ChenkSphere2Sphere(cameraEye.x, cameraEye.y - 3, cameraEye.z, pressPos.x, pressPos.y, pressPos.z, 1.5f, 1.5f);
		if (pressHit == true && nowPressAttack == true)
		{
			pressHit = false;
			nowPressAttack = 0;
			bossRotation.y = 0;
			pressPos.y = 30;
			pressCount = 0;
			playerStopCount = 0;
			playersHP--;
			//debugText.Print("EyeTarget:SPACE Q LCONTROL E", 20, 120 + 3, 1.5f);
			audio->PlaySE("Resources/player_damage.wav", false);
		}
		if (bosssHP <= 0)
		{
			sceneNo = 2;
			audio->StopBGM();
			audio->PlayBGM("Resources/clear.wav", true);
		}
		if (playersHP <= 0)
		{
			sceneNo = 3;
			audio->StopBGM();
			audio->PlayBGM("Resources/gameover.wav", true);
		}

	} else if (sceneNo == 2)
	{
		//debugText.Print("Clear", 20, 20, 1.5f);
		clearTimer++;
		if (clearTimer >= 5)
		{
			clearCount++;
			clearTimer = 0;
			if (clearCount == 8)
			{
				clearCount = 0;
			}
		}
		if (input->TriggerKey(DIK_SPACE))
		{
			sceneNo = 4;
			audio->StopBGM();
			audio->PlayBGM("Resources/title.wav", true);

		}
	} else if (sceneNo == 3)//�������p
	{
		//debugText.Print("gameover", 20, 20, 1.5f);
		if (input->TriggerKey(DIK_SPACE))
		{
			audio->StopBGM();
			audio->PlayBGM("Resources/title.wav", true);
			sceneNo = 4;
		}
	} else if (sceneNo == 4)//�������p
	{
		//debugText.Print("gameover", 20, 20, 1.5f);
		sceneNo = 0;

		BulletFlag = false;

		for (int i = 0; i < EnemyBulletNum; i++)
		{
			EnemyBulletFlag[i] = false;
			EnemyBulletFlag2 = true;
		}

		frame = 0;
		maxframe = 50;
		EnemyBulletFrame = 0;
		EnemyBulletMaxframe = 100;
		EnemyBulletFrame2 = 0;
		EnemyBulletMaxframe2 = 100;

		EnemybullTimer = 120;
		srand(time(NULL));

		enemyMoveFlag = 0;
		enemyFrame = 0;
		enemyMaxFrame = 100;

		bosssHP = 30;

		active = 0;

		shakeCount = 0;
		shakeX = 0.0f;
		shakeY = 0.0f;
		shakeZ = 0.0f;
		shakeFlag = false;

		lFrame = 0;
		maxlFrame = 50;

		animationTimer = 0;
		animationCount = 0;

		clearTimer = 0;
		clearCount = 0;

		angle = 0;

		playerObj->SetPosition({ -5.0f, 0.0f, -100.0f });
		playerObj->SetScale({ 0.5f,0.5f,0.5f });
		playerObj->Update();



		for (int i = 0; i < EnemyBulletNum; i++)
		{

			EnemyBullet[i]->SetPosition({ -5.0f, 0.0f, 0.0f });
			EnemyBullet[i]->SetScale({ 0.5f,0.5f,0.5f });
			EnemyBullet[i]->Update();



			
		}

		EnemyBullet2->SetPosition({ 0.0f, -5.0f, 0.0f });
		EnemyBullet2->SetScale({ 5.0f,5.0f,5.0f });
		EnemyBullet2->Update();


		playerObj2->SetRotation({ 0.0f,0.0f,0.0f });
		playerObj2->SetPosition({ 0.0f, -4.0f, 0.0f });
		playerObj2->SetScale({ 5.0f,5.0f,5.0f });
		playerObj2->Update();




		pressObj->SetRotation({ 0.0f,0.0f,0.0f });
		pressObj->SetPosition({ 0.0f, -4.0f, 0.0f });
		pressObj->SetScale({ 5.0f,5.0f,5.0f });
		pressObj->Update();




		skydomeObj->SetScale({ 5.0f,5.0f,5.0f });
		skydomeObj->Update();
		playersHP = 5;
		cameraEye.x = 75;
		cameraEye.y = 30;
		cameraEye.z = -50;
		cameraTarget.x = -90;
		cameraTarget.y = 40;

		Object3d::SetEye(cameraEye);
		Object3d::SetTarget(cameraTarget);
		lazerCount = 0;
	}

}

void GameScene::Draw()
{
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList *cmdList = dxCommon->GetCommandList();

#pragma region �w�i�X�v���C�g�`��
	// �w�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);
	// �w�i�X�v���C�g�`��
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
	/// �����ɔw�i�X�v���C�g�̕`�揈����ǉ��ł���
	/// </summary>

	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
	// �[�x�o�b�t�@�N���A
	dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3D�I�u�W�F�N�g�`��
	// 3D�I�u�W�F�N�g�`��O����
	Object3d::PreDraw(cmdList);

	// 3D�I�u�N�W�F�N�g�̕`��
	skydomeObj->Draw();

	if (BulletFlag == true) {
		playerObj->Draw();

	}
	if (sceneNo == 0)
	{
		playerDroneObj->Draw();
	}
	if (sceneNo == 1)
	{
		for (int i = 0; i < EnemyBulletNum; i++)
		{
			if (EnemyBulletFlag[i] == true && active == 1) {
				EnemyBullet[i]->Draw();
			}
			/*if (EnemyBulletFlag2 == true && active == 0) {
				
			}*/
		}
		if (lazerCount >= 60)
		{
			EnemyBullet2->Draw();
		}
	}

	if (nowPressAttack == 1)
	{
		pressObj->Draw();
	}
	if (bosssHP != 0) {
		playerObj2->Draw();
	}
	/// <summary>
	/// ������3D�I�u�W�F�N�g�̕`�揈����ǉ��ł���
	/// </summary>

	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();
#pragma endregion

#pragma region �O�i�X�v���C�g�`��
	//// �O�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);
	if (sceneNo == 0)
	{
		title[animationCount]->Draw();

	} else if (sceneNo == 1)
	{
		if (slowCount > 0)
		{
			slowUi->Draw();
		}
		playerHP->Draw();
		enemyHP->Draw();
		if (nowPressAttack == 1)
		{
			warningMark->Draw();
		}
	} else if (sceneNo == 2)
	{
		clear[clearCount]->Draw();
	} else if (sceneNo == 3)
	{
		gameover->Draw();
	}
	///// <summary>
	///// �����ɑO�i�X�v���C�g�̕`�揈����ǉ��ł���
	///// </summary>

	// �f�o�b�O�e�L�X�g�̕`��
	debugText.DrawAll(cmdList);

	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
#pragma endregion
}
