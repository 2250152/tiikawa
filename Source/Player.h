#pragma once

#include"System/ModelRenderer.h"
#include"Character.h"
#include"Effect.h"
#include"System/AudioSource.h"
#include"System/Input.h"
#include"Block.h"

//プライヤー
class Player :public Character
{
public:
	void SetStandingGroup(Group* group) { standingGroup = group; }
	Group* GetStandingGroup() const { return standingGroup; }
private:
	Player() {};
	~Player() override {};
public:
	// インスタンス取得
	static Player& Instance()
	{
		static Player instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 終了化
	void Finalize();

	//デバッグ用GUI描画
	void DrawDebugGUI();

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)override;

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//ジャンプ入力処理
	void InputJump(float elapsedTime);

	

protected:
	//着地した時に呼ばれる
	void OnLanding()override;

private:
	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	//移動入力処理
	void InputMove(float elapsedTime);

	//重力処理
	void ApplyLocalGravity(float elapsedTime);

	//プレイヤーをブロックの面にくっつける処理
	void StickToBlockFace();

	//接地処理
	void Grounding();

	//レイキャストでhitPositionがブロックの面の中心から一番近い位置を探してその近い距離を返す関数
	void SearchClosestHitPositionToFaceCenter(DirectX::XMFLOAT3 HitPos, Block* block)
	{
		float minDistance = FLT_MAX;
		DirectX::XMFLOAT3 faceCenters[6] = {
			block->GetTopCenter(),
			block->GetBottomCenter(),
			block->GetFrontCenter(),
			block->GetBackCenter(),
			block->GetRightCenter(),
			block->GetLeftCenter()
		};

		for (const auto& faceCenter : faceCenters)
		{
			float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&HitPos), DirectX::XMLoadFloat3(&faceCenter))));
			if (distance < minDistance)
			{
				minDistance = distance;
				closestHitPositionToFaceCenter = faceCenter;
			}
		}
	}


	//debug用の回転
	//void DebugTurn(float elapsedTime) {

	//	GamePad& gamePad = Input::Instance().GetGamePad();
	//	if(gamePad.GetButtonDown() & GamePad::BTN_Y)// "V"キーを押したら
	//	//Z軸を徐々に回転
	//	angle.z += DirectX::XMConvertToRadians(91); //91度回転
	//	
	//}


	//アニメーション=================================================
	enum class State
	{
		IDLE, //通常
		JumpHipDrop, //ジャンプヒップドロップ
	};
	State state = State::IDLE;
	int animationIndex = -1;
	float animationSeconds = 0.0f;
	bool animationLoop = false;
	bool animationPlaying = false;
	float animationBlendSecondsLength = 0.2f;// 0.2秒で切り替える
	void PlayAnimation(const char* name, bool loop);
	// アニメーション再生
	void PlayAnimation(int index, bool loop);
	// アニメーション更新処理
	void UpdateAnimation(float elapsedTime);
	//===============================================================

private:
	Model* model = nullptr;
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
	float jumpSpeed = 10.0f;
	int jumpCount = 0;
	int jumpLimit = 1;
	DirectX::XMFLOAT3 closestHitPositionToFaceCenter;//レイキャストでhitPositionがブロックの面の中心から一番近い位置
	

	Effect*  hitEffect = nullptr;

	AudioSource* hitSE = nullptr;

	Group* standingGroup = nullptr; // 現在乗っているグループ
	bool isHitLocal = false; // この関数内での当たり判定結果

	float JumpTime = 0.0f;

};