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
	Sprite *title = nullptr;
	Sprite *clear = nullptr;
	Sprite *gameover = nullptr;

	Object3d* playerObj[20] = { nullptr };
	Model *playerModel = nullptr;

	Object3d* EnemyBullet[20] = { nullptr };
	Model* EnemyBulletModel = nullptr;

	Object3d *playerObj2 = nullptr;
	Model *playerModel2 = nullptr;

	Object3d *skydomeObj = nullptr;
	Model *skydomeModel = nullptr;
public:
	//�V�[���ϐ�
	int sceneNo = 0;
	//�J�����ϐ�
	float angle = 0.0f;
	//���@�̒e�ϐ�
	bool BulletFlag[20];
	float shotTimer;
	float maxshotTimer;
	float frame;
	float maxframe;
	float x;
	float PI = 3.141592;
	//�X���[�ϐ�
	float slowValue = 1.0f;
	bool isSlow = 0;
	//�G�l�~�[�e�ϐ�
	bool EnemyBulletFlag[20];
	float EnemyBulletFrame;
	float EnemyBulletMaxframe;
	bool enemyMoveFlag;

	bool hit[20];
};

