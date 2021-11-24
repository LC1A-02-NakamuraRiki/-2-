#pragma once

#include "SafeDelete.h"
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include "Input.h"
#include "Sprite.h"
#include "Object3d.h"
#include "DebugText.h"
#include "Audio.h"
#include "Model.h"
#include "Collision.h"

/// <summary>
/// �Q�[���V�[��
/// </summary>
class GameScene
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private: // �ÓI�����o�ϐ�
	static const int debugTextTexNumber = 0;

public: // �����o�֐�

	/// <summary>
	/// �R���X�g�N���^
	/// </summary>
	GameScene();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~GameScene();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(DirectXCommon *dxCommon, Input *input, Audio *audio);

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

private: // �����o�ϐ�
	DirectXCommon *dxCommon = nullptr;
	Input *input = nullptr;
	Audio *audio = nullptr;
	DebugText debugText;

	/// <summary>
	/// �Q�[���V�[���p
	/// </summary>
	Sprite *spriteBG = nullptr;
	Sprite* title[12] = { nullptr };
	Sprite *clear = nullptr;
	Sprite *gameover = nullptr;
	Sprite *warningMark = nullptr;
	//�e-------------------------------
	Object3d* playerObj = nullptr ;
	Model *playerModel = nullptr;
	//�G�̒e-------------------------------
	static const int EnemyBulletNum = 60;
	Object3d* EnemyBullet[EnemyBulletNum] = { nullptr };
	Model* EnemyBulletModel = nullptr;
	Object3d* EnemyBullet2 = { nullptr };

	//�{�X-------------------------------
	Object3d *playerObj2 = nullptr;
	Model *playerModel2 = nullptr;
	//�w�i-------------------------------
	Object3d *skydomeObj = nullptr;
	Model *skydomeModel = nullptr;
	//�v���X�̓�
	Object3d *pressObj = nullptr;
	Model *pressModel = nullptr;
public:
	//�V�[���ϐ�
	int sceneNo = 0;
	float nowTime = 0;
	float endTime = 1.0;
	float timeRate = 0;
	bool isChange = false;
	//�J�����ϐ�
	float angle = 0.0f;
	//���@�̒e�ϐ�
	bool BulletFlag;
	float frame;
	float maxframe;
	float x;
	const float PI = 3.141592;
	//�X���[�ϐ�
	float slowValue = 1.0f;
	bool isSlow = 0;
	//�G�l�~�[�e�ϐ�
	bool EnemyBulletFlag[EnemyBulletNum];
	bool EnemyBulletFlag2;
	float EnemyBulletFrame;
	float EnemyBulletMaxframe;
	float EnemyBulletFrame2;
	float EnemyBulletMaxframe2;
	//���[�U�[
	int enemyMoveFlag;
	float enemyFrame;
	float enemyMaxFrame;
	float enemyX;
	//�e��
	float bullAngle[EnemyBulletNum];
	float EnemybullTimer;
	//�����蔻��
	bool laserHit = 0;
	bool barrageHit = 0;
	bool pressHit = 0;
	bool playerBulletHit = 0;
	int bossHP = 30;

	int active;

	//�V�F�C�N
	bool shakeFlag;
	float shakeX;
	float shakeY;
	float shakeZ;
	float shakeCount;

	//�v���X
	int playerStopCount = 0;
	bool nowPressAttack = 0;
	int pressCount = 0;

	//
	float lFrame;
	float maxlFrame;
	float lAngleY;

	//�^�C�g��
	float animationTimer;
	int animationCount;

};

