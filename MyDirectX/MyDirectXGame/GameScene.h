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
	Sprite* title[12] = { nullptr };
	Sprite *clear = nullptr;
	Sprite *gameover = nullptr;
	Sprite *warningMark = nullptr;
	//弾-------------------------------
	Object3d* playerObj = nullptr ;
	Model *playerModel = nullptr;
	//敵の弾-------------------------------
	static const int EnemyBulletNum = 60;
	Object3d* EnemyBullet[EnemyBulletNum] = { nullptr };
	Model* EnemyBulletModel = nullptr;
	Object3d* EnemyBullet2 = { nullptr };

	//ボス-------------------------------
	Object3d *playerObj2 = nullptr;
	Model *playerModel2 = nullptr;
	//背景-------------------------------
	Object3d *skydomeObj = nullptr;
	Model *skydomeModel = nullptr;
	//プレスの頭
	Object3d *pressObj = nullptr;
	Model *pressModel = nullptr;
public:
	//シーン変数
	int sceneNo = 0;
	float nowTime = 0;
	float endTime = 1.0;
	float timeRate = 0;
	bool isChange = false;
	//カメラ変数
	float angle = 0.0f;
	//自機の弾変数
	bool BulletFlag;
	float frame;
	float maxframe;
	float x;
	const float PI = 3.141592;
	//スロー変数
	float slowValue = 1.0f;
	bool isSlow = 0;
	//エネミー弾変数
	bool EnemyBulletFlag[EnemyBulletNum];
	bool EnemyBulletFlag2;
	float EnemyBulletFrame;
	float EnemyBulletMaxframe;
	float EnemyBulletFrame2;
	float EnemyBulletMaxframe2;
	//レーザー
	int enemyMoveFlag;
	float enemyFrame;
	float enemyMaxFrame;
	float enemyX;
	//弾幕
	float bullAngle[EnemyBulletNum];
	float EnemybullTimer;
	//当たり判定
	bool laserHit = 0;
	bool barrageHit = 0;
	bool pressHit = 0;
	bool playerBulletHit = 0;
	int bossHP = 30;

	int active;

	//シェイク
	bool shakeFlag;
	float shakeX;
	float shakeY;
	float shakeZ;
	float shakeCount;

	//プレス
	int playerStopCount = 0;
	bool nowPressAttack = 0;
	int pressCount = 0;

	//
	float lFrame;
	float maxlFrame;
	float lAngleY;

	//タイトル
	float animationTimer;
	int animationCount;

};

