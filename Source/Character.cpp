//#include "Character.h"
//
////行列更新処理
//void Character::UpdateTransform()
//{
//	//スケール行列を作成
//	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x,scale.y,scale.z);
//
//	//回転行列を作成
//	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z);
//	// 変更点：オイラー角の代わりにクォータニオンから行列を作る
//	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(rotationQuat);
//
//	//位置行列を作成
//	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
//
//	//3つの行列を組み合わせ、ワールド行列を作成
//	DirectX::XMMATRIX W = S * R * T;
//	//計算したワールド行列を取り出す
//	DirectX::XMStoreFloat4x4(&transform, W);
//}
//
////デバッグプリミティブ描画
//void Character::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
//{
//	//衝突判定用のデバッグk\球を描画
//	/*renderer->RenderSphere(rc, position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));*/
//
//	//衝突判定用のデバッグ円柱を描画
//	renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
//}
//
////移動処理
//void Character::Move(float elapsedTime, float vx, float vz, float speed)
//{
//	/*speed *= elapsedTime;
//	position.x += vx * speed;
//	position.z += vz * speed;*/
//
//	//移動方向ベクトルを設定
//	moveVecX = vx;
//	moveVecZ = vz;
//
//	//最大速度設定
//	maxMoveSpeed = speed;
//}
//
////旋回処理
//void Character::Turn(float elapsedTime, float vx, float vz, float speed)
//{
//	speed *= elapsedTime;
//
//	//進行方向ベクトルがゼロベクトルの場合は処理する必要なし　
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
//	float cross = (frontZ * vx) - (frontX * vz);        
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
//}
//
////ジャンプ処理
//void Character::Jump(float speed)
//{
//	//上方向の力を設定
//	//velocity.y = speed;
//
//	//playerのup(上方向のベクトルを算出)
//	DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);
//	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
//	DirectX::XMVECTOR localUp = DirectX::XMVector3TransformNormal(up, R);
//	localUp = DirectX::XMVector3Normalize(localUp);
//
//	//ジャンプ速度を加える
//	DirectX::XMVECTOR vel = DirectX::XMLoadFloat3(&velocity);
//	vel = DirectX::XMVectorAdd(vel, DirectX::XMVectorScale(localUp, speed));
//	DirectX::XMStoreFloat3(&velocity, vel);
//
//
//}
//
////速力処理更新
//void Character::UpdateVelocity(float elapsedTime) //Velocity...速度
//{
//	//垂直速力更新処理
//	//UpdateVerticalVelocity(elapsedTime);
//	//水平速力更新処理
//	//UpdateHorizonzalVelocity(elapsedTime);
//	//垂直移動更新処理
//	//UpdateVerticalMove(elapsedTime);
//	//水平移動更新処理
//	//UpdateHorizontalMove(elapsedTime);
//
//	//垂直 & 水平
//	UpdateVHmove(elapsedTime);
//}
//
//bool Character::ApplyDamage(int damage,float invincibleTime)
//{
//	//ダメージが0の場合は変更健康状態を変更する必要がない
//	if (damage == 0)return false;
//
//	//死亡している場合は健康状態を変更しない
//	if (health <= 0)return false;
//
//	//無敵時間中はダメージを与えない
//	if (invincibleTimer > 0.0f)return false;
//
//	//無敵時間設定
//	invincibleTimer = invincibleTime;
//
//	//ダメージ処理
//	health -= damage;
//
//	//死亡通知
//	if (health <= 0)
//	{
//		OnDead();
//	}
//	//ダメージ通知
//	else
//	{
//		OnDamaged();
//	}
//
//	//健康状態が変更した場合はtrueを返す
//	return true;
//
//}
//
////無敵時間更新
//void Character::UpdateInvincibleTimer(float elapsedTime)
//{
//	if (invincibleTimer > 0.0f)
//	{
//		invincibleTimer -= elapsedTime;
//	}
//}
//
////垂直速力更新処理
//void Character::UpdateVerticalVelocity(float elapsedTime)
//{
//	//重力処理
//	//velocity.y += gravity * elapsedTime;
//}
//
//void Character::UpdateVerticalMove(float elapsedTime)
//{
//	//移動処理
//	position.y += velocity.y * elapsedTime;
//
//	//地面判定
//	if (position.y < 0.0f)
//	{
//		position.y = 0.0f;
//
//		//着地した
//		if (!isGround)
//		{
//			OnLanding();
//		}
//		isGround = true;
//		velocity.y = 0.0f;
//	}
//	else
//	{
//		isGround = false;
//	}
//}
//
//void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
//{
//	//速力に力を加える
//	velocity.x += impulse.x;
//	velocity.y += impulse.y;
//	velocity.z += impulse.z;
//
//}
//
////水平速力更新処理
//void Character::UpdateHorizonzalVelocity(float elapsedTime)
//{
//	//XZ平面の速力を減速する
//	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
//
//	if (length > 0.0f)
//	{
//		//摩擦力
//		float friction = this->friction * elapsedTime;
//
//		//空中にいるときは摩擦力を減らす。
//		if (!isGround)friction *= airControl;
//
//		//摩擦による横方向の減速処理
//		if (length > friction)
//		{
//			//単位ベクトル化
//			float vx = velocity.x / length;
//			float vz = velocity.z / length;
//			//減速処理
//			velocity.x -= vx * friction;
//			velocity.z -= vz * friction;
//		}
//		//横方向の速力が摩擦力以下になったので速力を無効化
//		else
//		{
//			velocity.x = 0.0f;
//			velocity.z = 0.0f;
//		}
//	}
//
//	//XZ平面の速力を加速する
//	if (length <= maxMoveSpeed)
//	{
//		//移動ベクトルがゼロベクトルでないなら加速する
//		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
//		if (moveVecLength > 0.0f)
//		{
//			//加速力
//			float acceleration = this->acceleration * elapsedTime;
//
//			//空中にいるときは加速力を減らす
//			if (!isGround)acceleration *= airControl;
//
//			//移動ベクトルによる加速処理
//			velocity.x += moveVecX * acceleration;
//			velocity.z += moveVecZ * acceleration;
//
//			//最大速度制限
//			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
//			if (length > maxMoveSpeed)
//			{
//				float vx = velocity.x / length;
//				float vz = velocity.z / length;
//
//				velocity.x = vx * maxMoveSpeed;
//				velocity.z = vz * maxMoveSpeed;
//			}
//		}
//	}
//	//移動ベクトルをリセット
//	moveVecX = 0.0f;
//	moveVecZ = 0.0f;
//}
//
////水平移動更新処理
//void Character::UpdateHorizontalMove(float elapsedTime)
//{
//	//移動処理
//	position.x += velocity.x * elapsedTime;
//	position.z += velocity.z * elapsedTime;
//
//	//ためし
//	//position.y += velocity.y * elapsedTime;
//}
//
//void Character::UpdateVHmove(float elapsedTime)
//{
//	//移動処理
//	position.x += velocity.x * elapsedTime;
//	position.y += velocity.y * elapsedTime;
//	position.z += velocity.z * elapsedTime;
//
//    #ifdef _DEBUG
//	//何の意図があったの？これのせいでisGroundeが常にfalseになってたんだけど?
//	// 殺すぞ
//	//地面判定
//	//if (position.y < 0.0f)
//	//{
//	//	position.y = 0.0f;
//
//	//	//着地した
//	//	if (!isGround)
//	//	{
//	//		OnLanding();
//	//	}
//	//	isGround = true;
//	//	velocity.y = 0.0f;
//	//}
//	//else
//	//{
//	//	isGround = false;
//	//}
//    #endif
//
//}



#include "Character.h"
#include <cmath>

//行列更新処理
void Character::UpdateTransform()
{
	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	// 【変更点】オイラー角の代わりにクォータニオンから行列を作る
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(rotationQuat);

	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

//デバッグプリミティブ描画
void Character::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//衝突判定用のデバッグ円柱を描画
	renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

//移動処理
void Character::Move(float elapsedTime, float vx, float vz, float speed)
{
	//移動方向ベクトルを設定
	moveVecX = vx;
	moveVecZ = vz;

	//最大速度設定
	maxMoveSpeed = speed;
}

// 【クォータニオン版に完全刷新】旋回処理
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	// 進行方向ベクトルがゼロベクトルの場合は処理しない
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f) return;

	// 入力されたワールド移動方向ベクトル（XZ平面上）
	DirectX::XMVECTOR targetMoveDir = DirectX::XMVectorSet(vx / length, 0.0f, vz / length, 0.0f);

	// 現在のキャラクターの回転行列
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(rotationQuat);

	// キャラクターの現在のローカル軸を取得
	DirectX::XMVECTOR localUp = R.r[1];      // Y軸（頭の方向。どの面を歩いているかで変わる）
	DirectX::XMVECTOR localForward = R.r[2]; // Z軸（現在の正面方向）

	// 現在立っている面（localUp）に対して、目標の移動方向を投影してローカルな進行方向を再計算
	// これにより、壁や天井を歩いていても、その面に応じた正しい「前」に向かって旋回します
	DirectX::XMVECTOR projTargetDir = DirectX::XMVector3Normalize(
		DirectX::XMVectorSubtract(targetMoveDir, DirectX::XMVectorScale(localUp, DirectX::XMVectorGetX(DirectX::XMVector3Dot(targetMoveDir, localUp))))
	);

	// 現在の正面と目標方向の内積・外積から回転角度と向きを計算
	float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(localForward, projTargetDir));
	dot = fmaxf(-1.0f, fminf(1.0f, dot)); // クランプ処理

	float angleToTarget = acosf(dot);
	if (angleToTarget < 0.001f) return; // 既にほぼそっちを向いているなら終了

	// 1フレームの最大回転量
	float rotStep = speed * elapsedTime;
	if (angleToTarget > rotStep) angleToTarget = rotStep;

	// localUp を軸としてどちらに回転するかを外積で判定
	DirectX::XMVECTOR cross = DirectX::XMVector3Cross(localForward, projTargetDir);
	float direction = DirectX::XMVectorGetX(DirectX::XMVector3Dot(cross, localUp));

	if (direction < 0.0f) {
		angleToTarget = -angleToTarget;
	}

	// 「現在の頭の向き（localUp）」を軸にして回転するクォータニオンを作成し、現在の回転に乗算する
	DirectX::XMVECTOR turnQuat = DirectX::XMQuaternionRotationAxis(localUp, angleToTarget);
	rotationQuat = DirectX::XMQuaternionMultiply(rotationQuat, turnQuat);
	rotationQuat = DirectX::XMQuaternionNormalize(rotationQuat);
}

//ジャンプ処理
void Character::Jump(float speed)
{
	// 【変更点】rotationQuatから現在のローカルUp(上方向)を正確に求めて、その方向にジャンプさせる
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(rotationQuat);
	DirectX::XMVECTOR localUp = DirectX::XMVector3TransformNormal(up, R);
	localUp = DirectX::XMVector3Normalize(localUp);

	// ジャンプ速度を加える
	DirectX::XMVECTOR vel = DirectX::XMLoadFloat3(&velocity);
	vel = DirectX::XMVectorAdd(vel, DirectX::XMVectorScale(localUp, speed));
	DirectX::XMStoreFloat3(&velocity, vel);
}

//速力処理更新
void Character::UpdateVelocity(float elapsedTime)
{
	// 垂直 & 水平移動の更新
	UpdateVHmove(elapsedTime);
}

bool Character::ApplyDamage(int damage, float invincibleTime)
{
	if (damage == 0) return false;
	if (health <= 0) return false;
	if (invincibleTimer > 0.0f) return false;

	invincibleTimer = invincibleTime;
	health -= damage;

	if (health <= 0) {
		OnDead();
	}
	else {
		OnDamaged();
	}

	return true;
}

//無敵時間更新
void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f) {
		invincibleTimer -= elapsedTime;
	}
}

//垂直速力更新処理
void Character::UpdateVerticalVelocity(float elapsedTime)
{
}

void Character::UpdateVerticalMove(float elapsedTime)
{
}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

//水平速力更新処理
void Character::UpdateHorizonzalVelocity(float elapsedTime)
{
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

	if (length > 0.0f) {
		float friction = this->friction * elapsedTime;
		if (!isGround) friction *= airControl;

		if (length > friction) {
			float vx = velocity.x / length;
			float vz = velocity.z / length;
			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}
		else {
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	if (length <= maxMoveSpeed) {
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f) {
			float acceleration = this->acceleration * elapsedTime;
			if (!isGround) acceleration *= airControl;

			velocity.x += moveVecX * acceleration;
			velocity.z += moveVecZ * acceleration;

			float currentLength = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (currentLength > maxMoveSpeed) {
				velocity.x = (velocity.x / currentLength) * maxMoveSpeed;
				velocity.z = (velocity.z / currentLength) * maxMoveSpeed;
			}
		}
	}
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

//水平移動更新処理
void Character::UpdateHorizontalMove(float elapsedTime)
{
	position.x += velocity.x * elapsedTime;
	position.z += velocity.z * elapsedTime;
}

// 垂直 & 水平移動処理
void Character::UpdateVHmove(float elapsedTime)
{
	// 実際の移動処理
	position.x += velocity.x * elapsedTime;
	position.y += velocity.y * elapsedTime;
	position.z += velocity.z * elapsedTime;

	// バグの原因になっていた #ifdef _DEBUG 内の position.y < 0.0f による
	// 強制接地判定（isGroundのバグ）は完全に削除しました。
}