#pragma once

#include"System/ModelRenderer.h"
#include"Character.h"
#include"Effect.h"
#include"System/AudioSource.h"

//プライヤー
class Player :public Character
{
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
	void InputJump();

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

private:
	Model* model = nullptr;
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
	float jumpSpeed = 12.0f;

	int jumpCount = 0;
	int jumpLimit = 2;

	

	Effect*  hitEffect = nullptr;

	AudioSource* hitSE = nullptr;

};