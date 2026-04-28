#include"Player.h"
#include"System/Input.h"
#include<imgui.h>
#include"Camera.h"
#include"System/Audio.h"

void Player::Initialize()
{
	model = new Model("Data/Model/player/Astronaut.mdl");

	//ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/Hit.efk");

	//ヒットSE読み込み
	hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.0005f;
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
	InputMove(elapsedTime);

	//速度処理更新
	UpdateVelocity(elapsedTime);
	
	//ジャンプ入力処理
	//InputJump();

	//重力処理
	ApplyLocalGravity(elapsedTime);


	//GamePad& gamePad = Input::Instance().GetGamePad();
	//switch (state)
	//{
	//case State::IDLE:
	//	if (gamePad.GetButtonDown() & GamePad::BTN_A && isGround)
	//	{
	//		state = State::JumpHipDrop;
	//		PlayAnimation("Armature|CharacterAnimation", false);
	//	}
	//	break;
	//case State::JumpHipDrop:
	//	if (!isGround)
	//	{
	//		//ジャンプヒップドロップ中の処理
	//	}
	//	else
	//	{
	//		state = State::IDLE;
	//		PlayAnimation("Armature|CharacterAnimation", true);
	//	}
	//	break;
	//}
	



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
			//速力
			ImGui::InputFloat3("Velocity", &velocity.x);
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

void Player::ApplyLocalGravity(float elapsedTime)
{
	// プレイヤーのローカルupベクトルを計算
	DirectX::XMVECTOR UP = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
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