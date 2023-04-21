#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "MathUtilityForText.h"

//コンストラクタ
GameScene::GameScene() {}



//デストラクタ
GameScene::~GameScene() {}


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
	worldTransformBeam_.scale_ = {0.3f, 0.3f, 0.3f};
	worldTransformBeam_.Initialize();

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

	// 行列変換を更新
	worldTransformBeam_.matWorld_ = MakeAffineMatrix(
	    worldTransformBeam_.scale_, worldTransformBeam_.rotation_,
	    worldTransformBeam_.translation_);

	// 変換行列を定数バッファに転送
	worldTransformBeam_.TransferMatrix();

	BeamMove();
	BeamBron();


}

// ビーム移動
void GameScene::BeamMove() {

	if (beamFlag_) {

		// 回転
		worldTransformBeam_.rotation_.x += 0.1f;

		// 移動
		worldTransformBeam_.translation_.z += 1.0f;

		// z座標が40超えたら消す
		if (worldTransformBeam_.translation_.z > 40) {
			beamFlag_ = false;
		}

	}

	
	
}

void GameScene::BeamBron() {

	// 発射
	if (input_->TriggerKey(DIK_SPACE) && beamFlag_ == false) {
		beamFlag_ = true;
		worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;
		worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
	}


}


// 更新
void GameScene::Update() {

	// プレイヤー
	PlayerUpdate();

	//　ビーム
	BeamUpdate();


}


// 表示
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	
	// BG
	spriteBG_->Draw();


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
	
	// ステージ
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);

	// プレイヤー表示
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);

	// ビーム表示
	if (beamFlag_) {

		modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
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
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
