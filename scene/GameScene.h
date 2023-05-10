#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugText.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	// BG
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;

	// ビュープロジェクション（共通）
	ViewProjection viewProjection_;

	// ステージ
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_[20];

	// プレイヤー
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;
	Sprite* spriteLife_[3];
	int playerTimer_ = 0;

	// 弾（ビーム）
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = nullptr;
	WorldTransform worldTransformBeam_[10];
	int beamFlag_[10];

	// 敵
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_[10];
	int isEnemyAlive_[10];
	float enemyJump_[10];

	// タイトル（スプライト）
	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;

	uint32_t textureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;
	int gameTimer_ = 0;

    // ゲームオーバー
	uint32_t textureHandleGameOver_ = 0;
	Sprite* spriteGameOver_ = nullptr;
	int gameOver_ = false;


	//デバッグ
	DebugText* debugText_ = nullptr;
	int gameScore_ = 0;
	int playerLife_ = 3;

	// シーン
	int sceneMode_ = 1;

	// スコア
	uint32_t textureHandleNumber_ = 0;
	Sprite* spriteNumber_[5];

	/// <summary>
	/// デストラクタ
	/// </summary>
    ~GameScene();	

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void GamePlayUpdate();      // ゲームプレイ更新
	void GamePlayDraw3D();      // ゲームプレイ3D表示
	void GamePlayDraw2DBack();  // ゲームプレイ背景2D表示
	void GamePlayDraw2DNear();  // ゲームプレイ近景2D表示


	// 初期化
	void GamePlayStart(); //変数の初期化など

	// ステージ
	void StageUpdate();

	// プレイヤー
	void PlayerUpdate();

	// ビーム
	void BeamUpdate();    // 更新
	void BeamMove();      // 移動
	void BeamBron();      // 発生
	int beamTimer = 0;

	// 敵
	void EnemyUpdate();   // 更新
	void EnemyMove();     // 移動
	void EnemyBron();     // 発生
	void EnemyJump();
	float enemySpeed_[10];

	// 衝突判定
	void Collision();
	void CollisionPlayerEnemy(); // プレイヤーと敵
	void CollisionBeamEnemy(); // ビームと敵

	// シーン切り替え
	void TitleUpdate();       // タイトル更新
	void TitleDraw2DNear();   // タイトル2D
	void GameOverDraw2DNear(); // ゲームオーバー
	void GameOverUpdate(); // ゲームオーバー更新

	// サウンド
	uint32_t soundDateHandleTitleBGM_ = 0;
	uint32_t soundDateHandleGamePlayBGM_ = 0;
	uint32_t soundDateHandleGameOverBGM_ = 0;
	uint32_t soundDateHandleEnemyHitSE_ = 0;
	uint32_t soundDateHandlePlayerHitSE_ = 0;
	uint32_t voiceHandleBGM_ = 0;

	// スコア数値
	void DrawScore();



private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
