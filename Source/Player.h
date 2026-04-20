#pragma once

#include"System/ModelRenderer.h"
#include"Character.h"
#include"ProjectileManager.h"
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

	//ジャンプ処理
	//void Jump(float speed);　←リファクタリングするため。だからCharacter.hに移す。キャラクターという概念のやつ全てに継承してほしいから

	//速力処理更新
	//void UpdateVelocity(float elapsedTime);　←リファクタリングするため。だからCharacter,hに移す。

	//ジャンプ入力処理
	void InputJump();

	//弾丸入力処理
	void InputProjectile();

protected:
	//着地した時に呼ばれる
	void OnLanding()override;

private:
	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	//移動処理
	//void Move(float elapsedTime, float vx, float vz, float speed);　←リファクタリングするため。だからCharacter.hに移す。

	//移動入力処理
	void InputMove(float elapsedTime);

	//旋回処理
	//void Turn(float elapsedTime, float vx, float vz, float speed);　←リファクタリングするため。だからCharacter.hに移す。

	//プレイヤーとエネミーとの衝突処理
	void CollisionPlayerVsEnemies();

	//弾丸と敵の衝突処理
	void CollisionProjectilesVsEnemies();

private:
	Model* model = nullptr;
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
	float jumpSpeed = 12.0f;
	//float gravity = -30.0f;   ←リファクタリングするため。だからCharacter.hに移す。
	//DirectX::XMFLOAT3 velocity = { 0,0,0 };  ←リファクタリングするため。だからCharacter.hに移す。

	int jumpCount = 0;
	int jumpLimit = 2;

	ProjectileManager projectileManager;

	Effect*  hitEffect = nullptr;

	AudioSource* hitSE = nullptr;

};