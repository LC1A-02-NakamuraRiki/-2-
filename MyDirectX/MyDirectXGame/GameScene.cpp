#include "GameScene.h"
#include <cassert>

using namespace DirectX;

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	safe_delete(spriteBG);
	safe_delete(playerModel);
	safe_delete(playerObj);

	safe_delete(playerModel2);
	safe_delete(playerObj2);
}

void GameScene::Initialize(DirectXCommon *dxCommon, Input *input, Audio *audio)
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
	//// �w�i�X�v���C�g����
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	// 3D�I�u�W�F�N�g����

	
	playerModel = playerModel->CreateFromObject("largeCarL");
	playerObj = Object3d::Create();
	playerObj->LinkModel(playerModel);
	playerObj->SetPosition({-5.0f, 0.0f, -20.0f });
	playerObj->SetScale({ 1.0f,1.0f,1.0f });
	playerObj->Update();

	playerModel = playerModel2->CreateFromObject("untitled");
	playerObj2 = Object3d::Create();
	playerObj2->LinkModel(playerModel);
	playerObj2->SetPosition({ 0.0f, 0.0f, 0.0f });
	playerObj2->SetScale({ 1.0f,1.0f,1.0f });
	playerObj2->Update();
	//�T�E���h�Đ�
	//audio->PlayWave("Resources/Alarm01.wav");
}

void GameScene::Update()
{
	debugText.Print("ObjectMove:ArrowKey", 20, 20, 1.5f);
	debugText.Print("EyeMove:W A S D", 20, 50, 1.5f);
	debugText.Print("EyeTarget:SPACE Q LCONTROL E", 20, 80, 1.5f);

	XMFLOAT3 cameraTarget = Object3d::GetTarget();
	XMFLOAT3 cameraEye = Object3d::GetEye();
	
	POINT mousePos;
	GetCursorPos(&mousePos);

	// �I�u�W�F�N�g�ړ�
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
	{
		// ���݂̍��W���擾
		XMFLOAT3 position = playerObj->GetPosition();

		// �ړ���̍��W���v�Z
		if (input->PushKey(DIK_UP)) { position.y += 1.0f; }
		else if (input->PushKey(DIK_DOWN)) { position.y -= 1.0f; }
		if (input->PushKey(DIK_RIGHT)) { position.x += 1.0f; }
		else if (input->PushKey(DIK_LEFT)) { position.x -= 1.0f; }

		// ���W�̕ύX�𔽉f
		playerObj->SetPosition(position);
	}

	XMFLOAT3 position2 = playerObj2->GetPosition();

	// �J�����ړ�
	if (input->PushKey(DIK_D) || input->PushKey(DIK_A))
	{
		if (input->PushKey(DIK_D)) { angle += 5.0f; }
		else if (input->PushKey(DIK_A)) { angle -= 5.0f; }

		XMVECTOR v0 = { 0, 0, -50, 0 };
		//angle���W�A������y���܂��ɉ�]�B���a��-100
		XMMATRIX rotM = XMMatrixRotationY(XMConvertToRadians(angle));
		XMVECTOR v = XMVector3TransformNormal(v0, rotM);
		XMVECTOR bossTarget = { position2.x,  position2.y,  position2.z };
		XMVECTOR v3 = bossTarget + v;
		XMFLOAT3 f = { v3.m128_f32[0], v3.m128_f32[1], v3.m128_f32[2] };
		cameraTarget = { bossTarget.m128_f32[0], bossTarget.m128_f32[1], bossTarget.m128_f32[2] };
		cameraEye = f;

		Object3d::SetTarget(cameraTarget);
		Object3d::SetEye(cameraEye);
	}

	// �J�����ړ�
	if (input->PushKey(DIK_Q) || input->PushKey(DIK_E) || input->PushKey(DIK_LCONTROL) || input->PushKey(DIK_SPACE))
	{
		if (input->PushKey(DIK_SPACE)) { Object3d::CameraMoveEyeVector({ 0.0f,+1.0f,0.0f }); }
		else if (input->PushKey(DIK_LCONTROL)) { Object3d::CameraMoveEyeVector({ 0.0f,-1.0f,0.0f }); }
		if (input->PushKey(DIK_E)) { Object3d::CameraMoveEyeVector({ +1.0f,0.0f,0.0f }); }
		else if (input->PushKey(DIK_Q)) { Object3d::CameraMoveEyeVector({ -1.0f,0.0f,0.0f }); }
	}

	playerObj->Update();
	playerObj2->Update();

}

void GameScene::Draw()
{
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList *cmdList = dxCommon->GetCommandList();

#pragma region �w�i�X�v���C�g�`��
	// �w�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);
	// �w�i�X�v���C�g�`��
	spriteBG->Draw();

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
	playerObj->Draw();
	playerObj2->Draw();
	/// <summary>
	/// ������3D�I�u�W�F�N�g�̕`�揈����ǉ��ł���
	/// </summary>

	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();
#pragma endregion

#pragma region �O�i�X�v���C�g�`��
	//// �O�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);

	///// <summary>
	///// �����ɑO�i�X�v���C�g�̕`�揈����ǉ��ł���
	///// </summary>

	// �f�o�b�O�e�L�X�g�̕`��
	debugText.DrawAll(cmdList);

	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
#pragma endregion
}
