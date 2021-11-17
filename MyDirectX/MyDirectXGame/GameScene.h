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
/// ゲームシーン
/// </summary>
class GameScene
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private: // 静的メンバ変数
	static const int debugTextTexNumber = 0;

public: // メンバ関数

	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon *dxCommon, Input *input, Audio *audio);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon *dxCommon = nullptr;
	Input *input = nullptr;
	Audio *audio = nullptr;
	DebugText debugText;

	/// <summary>
	/// ゲームシーン用
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
	//シーン変数
	int sceneNo = 0;
	//カメラ変数
	float angle = 0.0f;
	//自機の弾変数
	bool BulletFlag[20];
	float shotTimer;
	float maxshotTimer;
	float frame;
	float maxframe;
	float x;
	float PI = 3.141592;
	//スロー変数
	float slowValue = 1.0f;
	bool isSlow = 0;
	//エネミー弾変数
	bool EnemyBulletFlag[20];
	float EnemyBulletFrame;
	float EnemyBulletMaxframe;
	bool enemyMoveFlag;

	bool hit[20];
};

