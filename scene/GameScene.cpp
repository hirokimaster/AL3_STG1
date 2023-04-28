#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "MathUtilityForText.h"
#include "time.h"

//コンストラクタ
GameScene::GameScene() {}


//デストラクタ
GameScene::~GameScene() { 
	
	delete modelStage_;
	delete modelPlayer_;
	delete modelBeam_;
	delete modelEnemy_;
	delete spriteTitle_;
	delete spriteEnter_;
}


//初期化
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// BG
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	// ビュープロジェクションの初期化
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	// ステージ
	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();

	// ステージの位置を変更
	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};
	
	//変換行列を更新
	worldTransformStage_.matWorld_ = MakeAffineMatrix(
	    worldTransformStage_.scale_,
		worldTransformStage_.rotation_,
	worldTransformStage_.translation_);

	// 変換行列を定数バッファに転送
	worldTransformStage_.TransferMatrix();	

	// プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	// 弾（ビーム）
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].scale_ = {0.3f, 0.3f, 0.3f};
		worldTransformBeam_[i].Initialize();

	}
	
	// 敵
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].translation_.z = 40;
		worldTransformEnemy_[i].Initialize();

	}
	
	srand((unsigned int)time(NULL));

	// タイトル（2Dスプライト)
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});

	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 500});

	// ゲームオーバー
	textureHandleGameOver_ = TextureManager::Load("gameover.png");
	spriteGameOver_ = Sprite::Create(textureHandleGameOver_, {0, 0});

	//デバッグ
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();

}


// ゲームプレイ前の初期化
void GameScene::GamePlayStart() {

	// 敵
    for (int i = 0; i < 10; i++) {
		 worldTransformEnemy_[i].translation_.z = 40;
    }

	// スコア、ライフ
	gameScore_ = 0;
    playerLife_ = 3;
	// ゲームオーバーフラグ
	gameOver_ = false;
	// プレイヤー

	// 行列変換を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);

	// 変換行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();

	worldTransformPlayer_.translation_.x = 0;



}


// ゲームプレイ更新
void GameScene::GamePlayUpdate() { 
	PlayerUpdate();
	EnemyUpdate();
	BeamUpdate();
	Collision();

	// 
	if (playerLife_ <= 0) {
		gameOver_ = true;
	}

	if (gameOver_) {
		sceneMode_ = 2;
	}

}


void GameScene::GamePlayDraw3D() {
	// ステージ
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);

	// プレイヤー表示
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);

	// ビーム表示
	for (int i = 0; i < 10; i++) {
		if (beamFlag_) {

			modelBeam_->Draw(worldTransformBeam_[i], viewProjection_, textureHandleBeam_);
		}
	}
	

	// 敵表示
	for (int i = 0; i < 10; i++) {
		if (isEnemyAlive_) {

			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		}
	
	}
	
}

void GameScene::GamePlayDraw2DBack() {
	// BG
	spriteBG_->Draw();

}

void GameScene::GamePlayDraw2DNear() {
	// ゲームスコア
	char str[100];
	sprintf_s(str, "SCORE %d", gameScore_);
	debugText_->Print(str, 200, 10, 2);
	// プレイヤーライフ
	sprintf_s(str, "LIFE %d", playerLife_);
	debugText_->Print(str, 1000, 10, 2);

}


/*--------------------------------------------------
 プレイヤー
----------------------------------------------------*/

// プレイヤー更新
void GameScene::PlayerUpdate() {

	// 行列変換を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);

	// 変換行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();

	//移動処理
	//右
	if (input_->PushKey(DIK_D)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	
	}

	// 左
	if (input_->PushKey(DIK_A)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}


	//移動制限
	//右
	if (worldTransformPlayer_.translation_.x > 4.0f) {
		worldTransformPlayer_.translation_.x = 4.0f;
	}

	//左
	if (worldTransformPlayer_.translation_.x < -4.0f) {
		worldTransformPlayer_.translation_.x = -4.0f;
	}


}

/*---------------------------------------------------
 ビーム
-----------------------------------------------------*/

// ビーム更新
void GameScene::BeamUpdate() {

	for (int i = 0; i < 10; i++) {

		// 行列変換を更新
		worldTransformBeam_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformBeam_[i].scale_, worldTransformBeam_[i].rotation_,
		    worldTransformBeam_[i].translation_);

		// 変換行列を定数バッファに転送
		worldTransformBeam_[i].TransferMatrix();
	
	}
	

	BeamMove();
	BeamBron();


}

// ビーム移動
void GameScene::BeamMove() {

	for (int i = 0; i < 10; i++) {
		if (beamFlag_) {

			// 回転
			worldTransformBeam_[i].rotation_.x += 0.1f;

			// 移動
			worldTransformBeam_[i].translation_.z += 1.0f;

			// z座標が40超えたら消す
			if (worldTransformBeam_[i].translation_.z >= 40) {
				beamFlag_[i] = false;
			}

			if (beamFlag_[i] == false) {
				worldTransformBeam_[i].translation_.x = -200;
			}
		}
	}
	
}


// ビーム発射
void GameScene::BeamBron() {

	if (beamTimer == 0) {
		// 発射
		for (int i = 0; i < 10; i++) {
			if (input_->PushKey(DIK_SPACE) && beamFlag_[i] == false) {
				beamFlag_[i] = true;
				worldTransformBeam_[i].translation_.z = worldTransformPlayer_.translation_.z;
				worldTransformBeam_[i].translation_.x = worldTransformPlayer_.translation_.x;
				beamTimer = 1;
				break;
			}
		}
	} else {
		beamTimer++;
		if (beamTimer > 10) {
			beamTimer = 0;
		}
	
	}
	
	
}

/*------------------------------------------------
 敵
--------------------------------------------------*/

//敵更新
void GameScene::EnemyUpdate() {

	// 行列変換を更新
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformEnemy_[i].scale_, worldTransformEnemy_[i].rotation_,
		    worldTransformEnemy_[i].translation_);

		// 変換行列を定数バッファに転送
		worldTransformEnemy_[i].TransferMatrix();
	
	}
	

	EnemyMove();
	EnemyBron();

}

// 敵移動
void GameScene::EnemyMove() {

	for (int i = 0; i < 10; i++) {
		// 回転
		worldTransformEnemy_[i].rotation_.x += 0.1f;

		// 移動
		if (isEnemyAlive_) {

			worldTransformEnemy_[i].translation_.z -= 0.1f;
			worldTransformEnemy_[i].translation_.x += enemySpeed_[i];
		}

		if (worldTransformEnemy_[i].translation_.x > 4) {
			enemySpeed_[i] = -0.1f;
		}

		if (worldTransformEnemy_[i].translation_.x < -4) {
			enemySpeed_[i] = 0.1f;
		}


		// 端に行ったら消す
		if (worldTransformEnemy_[i].translation_.z <= -5) {
			isEnemyAlive_[i] = false;
		}

	
	}
	
}

// 敵発生
void GameScene::EnemyBron() {

	// 乱数でx座標の指定
	int randX = rand() % 80;
	float randX2 = (float)randX / 10 - 4;
	

	// 発生
	for (int i = 0; i < 10; i++) {
		if (isEnemyAlive_[i] == false) {
			isEnemyAlive_[i] = true;
			worldTransformEnemy_[i].translation_.z = 40;
			worldTransformEnemy_[i].translation_.x = randX2;
			// スピード
			if (rand() % 2 == 0) {
				enemySpeed_[i] = 0.1f;

			} else {
				enemySpeed_[i] = -0.1f;
			}
			break;
		}
		
			
	}
	
	

}

/*----------------------------------------------
 衝突判定
-------------------------------------------------*/

// 衝突判定更新
void GameScene::Collision() {
	// プレイヤーと敵
	CollisionPlayerEnemy();

	// ビームと敵
	CollisionBeamEnemy();
}

// 衝突判定（プレイヤーと敵）

void GameScene::CollisionPlayerEnemy() {

	//敵がいたら
	for (int i = 0; i < 10; i++) {
		if (isEnemyAlive_) {
			float dx =
			    abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
			float dz =
			    abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);
		
			
			// 当たったら
			if (dx < 1 && dz < 1) {
				// 消える
				isEnemyAlive_[i] = false;
				playerLife_ = playerLife_ - 1;
			}
		}
	
	}
	


}

void GameScene::CollisionBeamEnemy() {

	for (int i = 0; i < 10; i++) {

		if (isEnemyAlive_[i]) {

			for (int j = 0; j < 10; j++) {

				if (beamFlag_[j]) {

					float dx = abs(worldTransformBeam_[j].translation_.x -
					        worldTransformEnemy_[i].translation_.x);
					float dz = abs(worldTransformBeam_[j].translation_.z -
					        worldTransformEnemy_[i].translation_.z);

					// 当たったら
					if (dx < 1 && dz < 1) {
						// 消える
						isEnemyAlive_[i] = false;
						beamFlag_[j] = false;
						gameScore_ = gameScore_ + 1;
					}
				}
			
			
			}
		}

		if (beamFlag_[i] == false) {
			worldTransformBeam_[i].translation_.x = -200;
		}
	
	}


}
 

/*------------------------------------------------
 タイトル
---------------------------------------------------*/

// ゲームオーバー更新
void GameScene::GameOverUpdate() {
	gameTimer_ += 1;

	if (input_->TriggerKey(DIK_RETURN)) {
		GamePlayStart();
		sceneMode_ = 1;
	}
}


// タイトル更新
void GameScene::TitleUpdate() { 
	gameTimer_ += 1;

	// エンターキー押したとき　(タイトルからプレイ）
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneMode_ = 0;

	}

}

// タイトル表示
void GameScene::TitleDraw2DNear() {
	// タイトル表示
	spriteTitle_->Draw();

	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
	

}

void GameScene::GameOverDraw2DNear() {
	// ゲームオーバー
	spriteGameOver_->Draw();
	
}


// 更新
void GameScene::Update() { 
	// シーン切り替え
	switch (sceneMode_) {

	case 0:
		GamePlayUpdate(); // ゲームプレイ
		break;

	case 1:
		TitleUpdate(); // タイトル更新
		break;

    case 2:
		if (gameOver_) {
			GameOverUpdate();
		}
		break;
	}

}


// 表示
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>

	// シーン切り替え
	switch (sceneMode_) {

	case 0:
		GamePlayDraw2DBack(); // ゲームプレイ2D表示
		break;

    case 1:
		TitleDraw2DNear(); // タイトル表示
		break;

    case 2:
		if (gameOver_) {
			GamePlayDraw2DBack();
			GameOverDraw2DNear(); // ゲームオーバー
		}
		break;

	}


	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	
	switch (sceneMode_) {

	case 0:
		GamePlayDraw3D();
		break;

	case 2:
		if (gameOver_) {
			GamePlayDraw3D();
		}
	}
	

	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

	
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	
	switch (sceneMode_) {

	case 0:
		GamePlayDraw2DNear();
		break;

	case 2:

		if (gameTimer_ % 40 >= 20) {
			spriteEnter_->Draw();
		}
		GamePlayDraw2DNear();

	}
	
	debugText_->DrawAll();

	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
