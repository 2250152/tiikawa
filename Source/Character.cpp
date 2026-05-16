
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