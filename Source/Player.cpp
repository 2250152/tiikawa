#include"Player.h"
#include"System/Input.h"
#include<imgui.h>
#include"Camera.h"
#include"EnemyManager.h"
#include"Collision.h"
#include"ProjectileStraight.h"
#include"ProjectileHoming.h"
#include"System/Audio.h"

//コンストラクタ ←最初に呼び出される関数のこと
//Player::Player()
//{
//	//model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
//	//モデルが大きいのでスケーリング
//	//scale.x = scale.y = scale.z = 0.01f;
//
//	//ヒットエフェクト読み込み
//	hitEffect = new Effect("Data/Effect/Hit.efk");
//}
//デストラクタ
//Player::~Player()
//{
//	//delete model;
//
//	delete hitEffect;
//}

void Player::Initialize()
{
	model = new Model("Data/Model/gal/gal.mdl");//Mr.Incredible

	//ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/Hit.efk");

	//ヒットSE読み込み
	hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;
}

//終了化
void Player::Finalize()
{
	delete model;
	delete hitEffect;
	delete hitSE; 
}

//更新処理
void Player::Update(float elapsedTime)   //←elapsedTimeは1フレームかな
{

	//======================================================
	////進行ベクトル取得                                   
	//DirectX::XMFLOAT3 moveVec = GetMoveVec();      　　  

	////移動処理　　　　　　　　　　　　　　　　　　　　　 
	//float moveSpeed = this->moveSpeed * elapsedTime;
	//position.x += moveVec.x * moveSpeed;
	//position.z += moveVec.z * moveSpeed;
	// 　
	// 関数化した===========================================
	// 　↓
	
	//移動入力処理
	InputMove(elapsedTime);

	//速度処理更新
	UpdateVelocity(elapsedTime);//velocityは日本語で"速度"です。

	//弾丸更新処理
	projectileManager.Update(elapsedTime);//playerのとこで弾丸の更新処理の命令を出している。playerが放つから。

	//ジャンプ入力処理
	InputJump();

	//弾丸入力処理
	InputProjectile();

	//プレイヤーと敵との衝突処理
	CollisionPlayerVsEnemies();

	//弾丸と敵の衝突処理
	CollisionProjectilesVsEnemies();

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform();



}

//描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//これはplayerそのものの描画処理
	renderer->Render(rc, transform, model, ShaderId::Lambert);

	//弾丸描画処理
	projectileManager.Render(rc, renderer);
}

//==========================================================================================================================================================================================

//デバッグプリミティブ描画
void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスの関数呼び出し
	Character::RenderDebugPrimitive(rc, renderer);

	//弾丸デバッグプリミティブ描画
	projectileManager.RenderDebugPrimitive(rc, renderer);
}

//デバッグ用GUI描画　　←デバッグ表示は最後どうせ消すので分かりやすいように普通は一番上とかに書いとけばいい
void Player::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		//トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//位置
			ImGui::InputFloat3("Position", &position.x);
			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//スケール
			ImGui::InputFloat3("Scale", &scale.x);
		}
	}
	ImGui::End();
}


//#include"Camera.h"をインクルードしたか？
//スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//移動ベクトルはXZ平面に水平なベクトルになるようにする

	//カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);//←ピタゴラスの定理
	if (cameraRightLength > 0.0f)//長さがあったら
	{
		//単位ベクトル化　　　　←pdfに書いています、見ましょう
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength= sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		//単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//スティックの水平入力値をカメラ右方向に反映し、
	//スティックの垂直入力値をカメラ前方向に反映し、
	//進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;

}

//移動処理
//void Player::Move(float elapsedTime, float vx, float vz, float speed)
//{
//	speed *= elapsedTime;
//	position.x += vx * speed;
//	position.z += vz * speed;
//}

////旋回処理
//void Player::Turn(float elapsedTime, float vx, float vz, float speed)
//{
//	speed *= elapsedTime;
//
//	//進行方向ベクトルがゼロベクトルの場合は処理する必要なし　　　　　←進行ベクトルは長さ
//	float lengh = sqrtf(vx * vx + vz * vz);
//	if (lengh < 0.001f)return;//←本当は if(len==0) とやりたいけどちょっきり0にならない
//
//	//進行ベクトルを単位ベクトル化
//	vx /= lengh;
//	vz /= lengh;
//
//	//自身の回転値から前方向を求める
//	float frontX = sinf(angle.y);
//	float frontZ = cosf(angle.y);
//
//
//	//回転角を求めるため、2つの単位ベクトルのない席を計算する
//	float dot = (frontX * vx) + (frontZ * vz);
//
//	//内積値は-1.0～1.0で表現されており、2つの単位ベクトルの角度
//	//が小さいほど1.0に近づくという性質を利用して回転速度を調整する
//	float rot = 1.0f - dot;
//	if (rot > speed)rot = speed;
//
//
//
//	//左右判定を行うために2つの単位ベクトルの外積を計算する
//	float cross = (frontZ * vx) - (frontX * vz);               //←前方向に進行方向をかけるって言ってた
//
//
//	//2Dの外積値が正の場合か負の場合によって左右判定が行える
//	//左右判定を行うことによって左右回転を選択する
//	if (cross < 0.0f)
//	{
//		/*angle.y -= speed;*/
//		angle.y -= rot;          //内積値から求められたspeedを使う
//	}
//	else
//	{
//		/*angle.y += speed;*/
//		angle.y += rot;
//	}
//
//
//}

//移動入力処理
void Player::InputMove(float elapsedTime)
{
	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//移動処理
	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

//着地した時に呼ばれる
void Player::OnLanding()
{
	jumpCount = 0;
}

//プレイヤーとエネミーとの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0;i < enemyCount;++i)
	{

		Enemy* enemy = enemyManager.GetEnemy(i);

		//衝突処理
		DirectX::XMFLOAT3 outPosition;

		//if (Collision::IntersectSphereVsSphere(
		//	position, radius,
		//	enemy->GetPosition(),            //GetPositionとかを覚えてだって。やり方など使い方を覚えろってことか？
		//	enemy->GetRadius(),              //GetRadiusとかを覚えてだって
		//	outPosition))
		//{
		//	//押し出し後の位置設定
		//	enemy->SetPosition(outPosition); //SetPositionとかを覚えてだって
		//}
		if (Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition))
		{
			//敵の真上付近に当たったかを判定
			DirectX::HXMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::HXMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::HXMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::HXMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);

			//上から踏んづけた場合は小ジャンプする
			if (normal.y > 0.8)
			{
				//小ジャンプする
				Jump(jumpSpeed * 0.5f);
			}
			else
			{
				//押し出し後の位置設定
				enemy->SetPosition(outPosition);
			}
		}

	}

}

////ジャンプ処理
//void Player::Jump(float speed)
//{
//	//上方向の力を設定
//	velocity.y = speed;
//}

////速力処理更新
//void Player::UpdateVelocity(float elapsedTime)
//{
//	//重力処理
//	velocity.y += gravity * elapsedTime;  //重力を強くしていかないとダメ//elapsedTimeはgravityをの値を安定させるためにあるのかな？
//
//	//移動処理
//	position.y += velocity.y * elapsedTime;
//
//	//地面判定
//	if (position.y < 0.0f)//ここ絶体小なりにしないといけないらしい。
//	{
//		position.y = 0.0f;
//		velocity.y = 0.0f;
//	}
//
//}

//ジャンプ入力処理
void Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A) {

		//ジャンプ回数制限
		if (jumpCount < jumpLimit) {
			//ジャンプ
			++jumpCount;
			Jump(jumpSpeed);
		}
	}
}

//弾丸入力処理
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//直接弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{

		//※プログラムでsin,cosを使いたかったらfを付ける必要がある。sinf(),cosf()

		//前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);     //キャラクターって Y軸 を中心に回転するから angle.y
		dir.y = 0.0f;
		dir.z = cosf(angle.y);     //キャラクターって Y軸 を中心に回転するから angle.y

		//発射位置（プレイヤーの腰あたり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5;//腰あたりという指定があるので height * 0.5 が必要。
		pos.z = position.z;

		//発射
		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
		//projectileManager.Register(projectile);

	}

	//追尾弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		//前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		//発射位置（プレイヤーの腰あたり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		//ターゲット（デフォルトではプレイヤーの前方）
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		//一番近くの敵をターゲットにする
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0;i < enemyCount;++i)
		{
			Enemy* enemy = EnemyManager::Instance().GetEnemy( i );
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			DirectX::XMVECTOR D = DirectX::XMVector3Length(V);//←単位ベクトルにする
			float d;//2点間の距離
			DirectX::XMStoreFloat(&d, D);
			if (d < dist)
			{
				dist = d;
				target = enemy->GetPosition();
				target.y += enemy->GetHeight() * 0.5;
			}
		}

		//発射
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target); //Launch は日本語で開始するです。

	}
}

void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0;i < projectileCount;++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0;j < enemyCount;++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				//ダメージを与える
				if (enemy->ApplyDamage(1, 0.5f))
				{
					//吹き飛ばす
					{
						DirectX::XMFLOAT3 impulse;
						const float power = 10.0f;
						const DirectX::XMFLOAT3& e = enemy->GetPosition();
						const DirectX::XMFLOAT3& p = projectile->GetPosition();
						float vx = e.x - p.x;
						float vz = e.z - p.z;
						float lengthXZ = sqrtf(vx * vx + vz * vz);
						vx /= lengthXZ;
						vz /= lengthXZ;

						impulse.x = vx * power;
						impulse.y = power * 0.5f;
						impulse.z = vz * power;

						enemy->AddImpulse(impulse);
					}

					//ヒットエフェクト再生
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					//ヒットSE再生
					{
						hitSE->Play(false);
					}

					//弾丸破棄
					projectile->Destroy();
				}
			}
		}
	}
}