#include"Player.h"
#include"System/Input.h"
#include<imgui.h>
#include"Camera.h"
#include"System/Audio.h"
#include <DirectXCollision.h>
#include <System/Graphics.h>
#include"Collision.h"
#include"BlockManager.h"
#include"Group.h"


void Player::Initialize()
{
	model = new Model("Data/Model/player/player.mdl");

	//ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/Hit.efk");

	//ヒットSE読み込み
	hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.0005f;

	state = State::IDLE;
	PlayAnimation("IDLE", true);

	//クォータニオン初期化、初期化時に単位クォータニオンを代入
	rotationQuat = DirectX::XMQuaternionIdentity();
}

//終了化
void Player::Finalize()
{
	delete model;
	delete hitEffect;
	delete hitSE; 
}

//更新処理
void Player::Update(float elapsedTime)  
{
	
	//移動入力処理
	/*if (isGround)
	InputMove(elapsedTime);*/


	//プレイヤーをブロックの面に配置する処理
	StickToBlockFace();
	
	//ジャンプ入力処理
	InputJump(elapsedTime);
	//重力処理
	ApplyLocalGravity(elapsedTime);
	//接地処理
	Grounding();
	//速度処理更新
	UpdateVelocity(elapsedTime);

	
	
#ifdef _DEBUG 

	//debug用の旋回処理(Z軸が中心の回転)
	//DebugTurn(elapsedTime);

#endif

	GamePad& gamePad = Input::Instance().GetGamePad();
	//Mouse& mouse = Input::Instance().GetMouse();
	switch (state)
	{
	case State::IDLE:
	
		if (gamePad.GetButtonDown() & GamePad::BTN_A)
		{
			state = State::JumpHipDrop;
			PlayAnimation("hip drop", false);
		}
		break;

	case State::JumpHipDrop:

		if (!isGround)
		{
			//ジャンプヒップドロップ中の処理
		}
		else
		{
			state = State::IDLE;
			PlayAnimation("IDLE", true);
		}
		break;
	}
	



	//オブジェクト行列を更新
	UpdateTransform();

	// アニメーション更新処理
	UpdateAnimation(elapsedTime);
	////モデル行列更新
	//model->UpdateTransform(); //←UpdateAnimationの中に入っている


}

//描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//これはplayerそのものの描画処理
	renderer->Render(rc, transform, model, ShaderId::Lambert);

}

//デバッグプリミティブ描画
void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスの関数呼び出し
	Character::RenderDebugPrimitive(rc, renderer);

	
}

//デバッグ用GUI描画
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

			//回転 (オイラー角のやり方)
			/*DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);*/

			//クォータニオン表示に変更
			// オイラー角変数を廃止したから、内部クォータニオンの値をそのまま表示します
			//比例的に手動書き換えしたい場合用のダミー（読み取り専用か、簡易的なオイラー角表示にしても良いですがバグの原因になるため省略）
			DirectX::XMFLOAT4 q;
			DirectX::XMStoreFloat4(&q, rotationQuat);
			ImGui::InputFloat4("Quat (X,Y,Z,W)", &q.x);


			//スケール
			ImGui::InputFloat3("Scale", &scale.x);
			//速力
			ImGui::InputFloat3("Velocity", &velocity.x);

			//接地
			ImGui::Checkbox("IsGround", &isGround);
		}
	}
	ImGui::End();
}

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
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)//長さがあったら
	{
		//単位ベクトル化　　　
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

//ジャンプ入力処理
void Player::InputJump(float elapsedTime)
{
	/*Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButton() & mouse.BTN_LEFT)
	{
		JumpTime += elapsedTime;
	}
	else
	{
		JumpTime = 0.0f;
	}*/


	JumpPermission = true;
	for (const auto& group : BlockManager::Instance().GetGroups()) {
		if (GroupType::Start == group->GetType() && Group::State::Rotating == group->GetState()/* || Group::State::Moving == group->GetState()*/)
		{
			JumpPermission = false;//ジャンプ許可
		}
	}


	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A ) {

		//ジャンプ回数制限
		if (jumpCount < jumpLimit) {
			//ジャンプ
			++jumpCount;
			
			if(JumpPermission)
			Jump(jumpSpeed);
		}
	}
}

void Player::ApplyLocalGravity(float elapsedTime)
{
	// プレイヤーのローカルupベクトルを計算
	DirectX::XMVECTOR UP = DirectX::XMVectorSet(0, 1, 0, 0);

	// 【クォータニオンを使用】rotationQuatから直接ローカルUpベクトルを生成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(rotationQuat);
	DirectX::XMVECTOR localUp = DirectX::XMVector3TransformNormal(UP, R);

	// upベクトルの正反対（down方向）を求める
	DirectX::XMVECTOR localDown = DirectX::XMVectorNegate(localUp);

	// 重力加速度ベクトルを求める（Character::gravityは負値なので-multしない）
	DirectX::XMVECTOR gravityVec = DirectX::XMVectorScale(localDown, std::abs(gravity));

	// velocityに加算
	DirectX::XMVECTOR velocityVec = DirectX::XMLoadFloat3(&velocity);
	velocityVec = DirectX::XMVectorAdd(velocityVec, DirectX::XMVectorScale(gravityVec, elapsedTime));
	DirectX::XMStoreFloat3(&velocity, velocityVec);
}

void Player::PlayAnimation(const char* name, bool loop)
{
	int index = 0;
	const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
	for (const ModelResource::Animation& animation : animations)
	{
		if (animation.name == name)
		{
			PlayAnimation(index, loop);
			return;
		}
		++index;
	}
}
// アニメーション再生
void Player::PlayAnimation(int index, bool loop)
{
	animationPlaying = true;
	animationLoop = loop;
	animationIndex = index;
	animationSeconds = 0.0f;
}

// アニメーション更新処理
void Player::UpdateAnimation(float elapsedTime)
{
	if (animationPlaying)
	{
		//アニメーション切り替え時のブレンド率を計算
		float blendRate = 1.0f;
		if (animationSeconds < animationBlendSecondsLength)
		{
			blendRate = animationSeconds / animationBlendSecondsLength;
		}

		std::vector<Model::Node>& nodes = model->GetNodes();

		//現在のアニメーションデータを取得
		const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
		const ModelResource::Animation& animation = animations.at(animationIndex);

		//時間経過
		animationSeconds += elapsedTime;

		//再生時間が経過時間を超えたら
		if (animationSeconds >= animation.secondsLength)
		{
			if (animationLoop)
			{
				//ループ：先頭に戻す（超えた分だけ残す）
				animationSeconds -= animation.secondsLength;
			}
			else
			{
				animationPlaying = false;

				//非ループ：最後で止める
				animationSeconds = animation.secondsLength;
			}
		}

		//アニメーションデータからキーフレームデータリストを取得
		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		int keyCount = static_cast<int>(keyframes.size());
		for (int keyIndex = 0;keyIndex < keyCount - 1;++keyIndex)
		{
			//現在の時間がどのキーフレームの間にいるか判定する
			const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
			const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

			if (animationSeconds >= keyframe0.seconds && animationSeconds < keyframe1.seconds)
			{
				//再生時間とキーフレームの時間から補完率を計算
				float rate = (animationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);


				//全てのノードの姿勢を計算する
				int nodeCount = static_cast<int>(nodes.size());
				for (int nodeIndex = 0;nodeIndex < nodeCount;++nodeIndex)
				{
					//キーフレームデータ取得
					const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
					const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

					//ノード取得
					Model::Node& node = nodes[nodeIndex];

					//前のキーフレームと次のキーフレームの姿勢を補完
					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);//rotateがXMLoadFloat4なのはクウォータニオンだから
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate); //translate...平行移動
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

					DirectX::XMVECTOR S = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&node.scale), DirectX::XMVectorLerp(S0, S1, rate), blendRate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&node.rotate), DirectX::XMQuaternionSlerp(R0, R1, rate), blendRate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&node.translate), DirectX::XMVectorLerp(T0, T1, rate), blendRate);

					//計算結果をボーンに格納
					DirectX::XMStoreFloat3(&node.scale, S);
					DirectX::XMStoreFloat4(&node.rotate, R);
					DirectX::XMStoreFloat3(&node.translate, T);
				}
				break;
			}
		}
	}
	//モデル行列更新
	model->UpdateTransform();
}

void Player::StickToBlockFace()
{
	Mouse& mouse = Input::Instance().GetMouse();

	// マウス左クリックが押された瞬間だけ処理する
	if (!(mouse.GetButtonDown() & mouse.BTN_LEFT)) return;

	// --- レイの生成処理 (省略なし、既存の IWVP を使う処理) ---
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	POINT cursor;
	::GetCursorPos(&cursor);
	::ScreenToClient(Graphics::Instance().GetWindowHandle(), &cursor);

	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMMATRIX IWVP = DirectX::XMMatrixInverse(nullptr, (DirectX::XMMatrixIdentity() * View * Projection));

	auto ScreenToWorld = [&](float z) {
		DirectX::XMVECTOR ndc = DirectX::XMVectorSet(
			2.0f * (cursor.x) / screenWidth - 1.0f,
			1.0f - 2.0f * (cursor.y) / screenHeight,
			z, 1.0f);
		return DirectX::XMVector3TransformCoord(ndc, IWVP);
		};

	DirectX::XMVECTOR WorldRayStart = ScreenToWorld(0.0f);
	DirectX::XMVECTOR WorldRayFar = ScreenToWorld(1.0f);

	DirectX::XMFLOAT3 rayStart, rayEnd;
	DirectX::XMStoreFloat3(&rayStart, WorldRayStart);
	DirectX::XMStoreFloat3(&rayEnd, WorldRayFar);

	// --- レイキャスト判定 ---
	DirectX::XMFLOAT3 hitPos, hitNormal;
	float hitDist;
	float minDistance = FLT_MAX;
	Block* bestBlock = nullptr;
	DirectX::XMVECTOR bestHitPoint;

	for (const auto& group : BlockManager::Instance().GetGroups()) {
		if (GroupType::Start == group->GetType() && Group::State::Idle == group->GetState())
		{
			for (const auto& block : group->GetBlocks()) {
				if (Collision::RayCast(rayStart, rayEnd, block->Gettranceform(), block->GetModel(), hitPos, hitNormal, hitDist)) {
					if (hitDist < minDistance) {
						minDistance = hitDist;
						bestBlock = block.get();
						bestHitPoint = DirectX::XMLoadFloat3(&hitPos);
					}
				}
			}
		}
	}

	// ヒットした時の配置処理 
	if (bestBlock) {
		Block::FaceData bestFace = bestBlock->FindBestFace(bestHitPoint);

		// 位置を面の中心に設定
		position = bestFace.worldPos;

		// クォータニオンで正確な回転行列を作る
		DirectX::XMVECTOR newUp = DirectX::XMLoadFloat3(&bestFace.worldNormal);
		newUp = DirectX::XMVector3Normalize(newUp);

		// プレイヤーの現在の正面（Forward）を取得
		DirectX::XMMATRIX currentRot = DirectX::XMMatrixRotationQuaternion(rotationQuat);
		DirectX::XMVECTOR currentForward = currentRot.r[2]; // Z軸（前方向）

		// 新しい上方向（法線）と現在の前方向が平行に近い場合の対策
		float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(newUp, currentForward));
		DirectX::XMVECTOR tempForward = currentForward;
		if (fabsf(dot) > 0.99f) {
			tempForward = currentRot.r[1]; // 平行なら現在のローカルUp(Y軸)を仮の正面にする
		}

		// 正規直交基底を再構築して、正しい向きの回転行列を作る
		DirectX::XMVECTOR newRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(newUp, tempForward));
		DirectX::XMVECTOR newForward = DirectX::XMVector3Cross(newRight, newUp);

		DirectX::XMMATRIX rotMat = DirectX::XMMatrixIdentity();
		rotMat.r[0] = newRight;
		rotMat.r[1] = newUp;
		rotMat.r[2] = newForward;

		// 行列からクォータニオンに変換して保存（オイラー角を一切挟まない）
		rotationQuat = DirectX::XMQuaternionRotationMatrix(rotMat);

		// 状態の更新
		isGround = true;
		velocity = { 0, 0, 0 };
		standingGroup = bestBlock->GetGroup();
	}
}


void Player::Grounding()
{
	bool isHitLocal = false;
	Block* hitBlockPtr = nullptr;

	bool onGround = isGround;
	isGround = false;

	// 【クォータニオンを使用】現在の姿勢からローカルUp/Downを計算
	DirectX::XMVECTOR UP = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(rotationQuat);
	DirectX::XMVECTOR localUp = DirectX::XMVector3TransformNormal(UP, R);
	DirectX::XMVECTOR localDown = DirectX::XMVectorNegate(localUp);

	DirectX::XMVECTOR velocityVec = DirectX::XMLoadFloat3(&velocity);
	float UpOrDown = DirectX::XMVectorGetX(DirectX::XMVector3Dot(velocityVec, localDown));

	if (UpOrDown > 0.0f)
	{
		DirectX::XMVECTOR downVelocityVec = DirectX::XMVectorScale(localDown, UpOrDown);
		DirectX::XMStoreFloat3(&DownGravity, downVelocityVec);

		DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
		
		DirectX::XMVECTOR START = DirectX::XMVectorMultiplyAdd(localUp, DirectX::XMVectorReplicate(0.5f), playerPosVec); //playerPos + localUp * 0.5f
		DirectX::XMVECTOR END = playerPosVec;//DirectX::XMVectorMultiplyAdd(localDown, DirectX::XMVectorReplicate(1.5f), playerPosVec); //playerPos + localDown * 1.0f

		DirectX::XMFLOAT3 start, end;
		DirectX::XMStoreFloat3(&start, START);
		DirectX::XMStoreFloat3(&end, END);

		DirectX::XMFLOAT3 hitPosition, hitNormal;
		float hitDistance;
		float minDistance = FLT_MAX;
		DirectX::XMVECTOR bestHitPointVec;

		for (const auto& group : BlockManager::Instance().GetGroups())
		{
			if (GroupType::Start == group->GetType())
			{
				for (const auto& block : group->GetBlocks())
				{
					if (Collision::RayCast(start, end, block->Gettranceform(), block->GetModel(), hitPosition, hitNormal, hitDistance))
					{
						if (hitDistance < minDistance)
						{
							minDistance = hitDistance;
							isHitLocal = true;
							hitBlockPtr = block.get();
							bestHitPointVec = DirectX::XMLoadFloat3(&hitPosition);
						}
					}
				}
			}
		}

		if (isHitLocal)
		{
			Block::FaceData bestFace = hitBlockPtr->FindBestFace(bestHitPointVec);

			OnLanding();

			// 1. 位置の更新
			position = bestFace.worldPos;

			// 2. 【着地時のクォータニオン姿勢更新】
			DirectX::XMVECTOR newUp = DirectX::XMLoadFloat3(&bestFace.worldNormal);
			newUp = DirectX::XMVector3Normalize(newUp);

			// 現在の正面向きをベースに正規直交基底を作る
			DirectX::XMMATRIX currentRot = DirectX::XMMatrixRotationQuaternion(rotationQuat);
			DirectX::XMVECTOR currentForward = currentRot.r[2];

			float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(newUp, currentForward));
			DirectX::XMVECTOR tempForward = currentForward;
			if (fabsf(dot) > 0.99f) {
				tempForward = currentRot.r[1];
			}

			DirectX::XMVECTOR newRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(newUp, tempForward));
			DirectX::XMVECTOR newForward = DirectX::XMVector3Cross(newRight, newUp);

			DirectX::XMMATRIX rotMat = DirectX::XMMatrixIdentity();
			rotMat.r[0] = newRight;
			rotMat.r[1] = newUp;
			rotMat.r[2] = newForward;

			// クォータニオンを直接上書き
			rotationQuat = DirectX::XMQuaternionRotationMatrix(rotMat);

			velocity = { 0, 0, 0 };
			isGround = true;
			standingGroup = hitBlockPtr->GetGroup();
		}
		else
		{
			isGround = onGround;
			if (!isGround) standingGroup = nullptr;
		}
	}
	else
	{
		DownGravity = { 0.0f, 0.0f, 0.0f };
	}
}


