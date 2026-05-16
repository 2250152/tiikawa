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
	InputJump();
	//重力処理
	ApplyLocalGravity(elapsedTime);
	//接地処理
	Grounding();
	//速度処理更新
	UpdateVelocity(elapsedTime);

	
	
#ifdef _DEBUG 

	//debug用の旋回処理(Z軸が中心の回転)
	DebugTurn(elapsedTime);

#endif

	GamePad& gamePad = Input::Instance().GetGamePad();
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
	DirectX::XMVECTOR localUp = DirectX::XMVector3TransformNormal(UP, R);//XMVector3TransformNormal関数はベクトルを行列で変換する関数。ここではプレイヤーの回転行列Rを使って、ワールド座標系のupベクトルをプレイヤーのローカル座標系のupベクトルに変換している。

	// upベクトルの正反対（down方向）を求める
	DirectX::XMVECTOR localDown = DirectX::XMVectorNegate(localUp);//XMVectorNegate関数はベクトルの符号を反転させる関数。ここではプレイヤーのローカルupベクトルを反転させて、ローカルdownベクトルを求めている。

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

//void Player::StickToBlockFace()
//{
//	
//	Mouse& mouse = Input::Instance().GetMouse();
//
//	//スクリーンサイズを取得
//	float screenWidth = Graphics::Instance().GetScreenWidth();
//	float screenHeight = Graphics::Instance().GetScreenHeight();
//
//	//マウスカーソルの位置を取得
//	POINT cursor;
//	::GetCursorPos(&cursor);
//	::ScreenToClient(Graphics::Instance().GetWindowHandle(), &cursor);//スクリーン座標をクライアント座標に変換
//
//	//各行列を取得
//	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
//	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
//	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
//
//	//スクリーン座標の設定
//	DirectX::XMVECTOR ScreenPosition, WorldPositionNear, WorldPositionFar;
//	DirectX::XMFLOAT3 screenPosition;
//	screenPosition.x = static_cast<float>(cursor.x);
//	screenPosition.y = static_cast<float>(cursor.y);
//
//
//	float viewportX = 0.0f;
//	float viewportY = 0.0f;
//	float viewportW = screenWidth;
//	float viewportH = screenHeight;
//	float viewportMinZ = 0.0f;
//	float viewportMaxZ = 1.0f;
//
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	//スクリーン座標をワールド座標に変換し、レイの始点を求める。
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	screenPosition.z = 0.0f;
//
//	//スクリーン座標からNDC座標へ変換
//	DirectX::XMVECTOR NDCPositionNear = DirectX::XMVectorSet(
//		2.0f * (screenPosition.x - viewportX) / viewportW - 1.0f,
//		1.0f - 2.0f * (screenPosition.y - viewportY) / viewportH,
//		(screenPosition.z - viewportMinZ) / (viewportMaxZ - viewportMinZ),
//		1.0f);
//
//	//NDC座標からワールド座標へ変換
//	DirectX::XMMATRIX WVP = World * View * Projection;
//	DirectX::XMMATRIX IWVP = DirectX::XMMatrixInverse(nullptr, WVP);//逆行列を作る
//	WorldPositionNear = DirectX::XMVector3TransformCoord(NDCPositionNear, IWVP);//ベクトルに行列を掛けて位置として変換する.vec × matrix → w 除算も自動でやる
//
//
//	DirectX::XMFLOAT3 rayStart;
//	DirectX::XMStoreFloat3(&rayStart, WorldPositionNear);
//
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	//スクリーン座標をワールド座標に変換し、レイの終点を求める
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	screenPosition.z = 1.0f;
//
//	//スクリーン座標からNDC座標へ変換
//	DirectX::XMVECTOR NDCPositionFar = DirectX::XMVectorSet(  //4次元ベクトルを作る関数。例..XMVECTOR v = XMVectorSet(1,2,3,1);中身は(x=1, y=2, z=3, w=1)
//		2.0f * (screenPosition.x - viewportX) / viewportW - 1.0f,
//		1.0f - 2.0f * (screenPosition.y - viewportY) / viewportH,
//		(screenPosition.z - viewportMinZ) / (viewportMaxZ - viewportMinZ),
//		1.0f);
//
//	WorldPositionFar = DirectX::XMVector3TransformCoord(NDCPositionFar, IWVP);
//
//	DirectX::XMFLOAT3 rayEnd;
//	DirectX::XMStoreFloat3(&rayEnd, WorldPositionFar);
//
//	//============================================================================================================
//
//	//ステージとレイキャストを行い、配置座標を求める
//	DirectX::XMFLOAT3 hitPosition, hitNormal;
//	float hitDistance;
//	float normalX, normalY, normalZ;
//	float minDistance = FLT_MAX; // 一番近い距離を保存する変数
//	bool isHit = false;
//	DirectX::XMFLOAT3 bestPosition, bestNormal;
//
//	for (const auto& group : BlockManager::Instance().GetGroups())
//	{
//		for (const auto& block : group->GetBlocks())
//		{
//			if (Collision::RayCast(rayStart,rayEnd,block->Gettranceform(),block->GetModel(),hitPosition,hitNormal,hitDistance))
//			{
//				if (hitDistance < minDistance)
//				{
//
//					// 1. ブロックの回転行列だけを取り出す（スケールや平行移動を含まないように）
//				    // block->Gettranceform() から回転成分だけを抽出するか、angleから作り直す
//					DirectX::XMFLOAT3 bAngle = block->GetAngle();
//					DirectX::XMMATRIX blockRotation = DirectX::XMMatrixRotationRollPitchYaw(bAngle.x, bAngle.y, bAngle.z);
//					// 2. 法線に回転を適用する
//					DirectX::XMVECTOR localNormal = DirectX::XMLoadFloat3(&hitNormal);
//					DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(localNormal, blockRotation);
//					/*
//					ポイント：なぜ XMVector3TransformNormal を使うのか？
//					ベクトルに行列を掛ける関数には XMVector3TransformCoord もありますが、法線（方向）を変換する場合は XMVector3TransformNormal を使います。
//					・TransformCoord : 位置（座標）を変換する。平行移動の影響を受ける。
//					・TransformNormal : 向きだけを変換する。平行移動の影響を無視する。*/
//
//					// 3. 変換した法線を保存
//					DirectX::XMStoreFloat3(&bestNormal, worldNormal);
//
//				
//
//					SearchClosestHitPositionToFaceCenter(hitPosition, block.get());
//					minDistance = hitDistance;
//					bestPosition = hitPosition;
//					//bestNormal = hitNormal;　上に書き換え
//					isHit = true;
//
//					//ここにいったんついかした
//					if (isHit) {
//						// block.get() から所属グループを取得して保持
//						standingGroup = block->GetGroup();
//					}
//				}
//			}
//		}
//	}
//
//	GamePad& gamePad = Input::Instance().GetGamePad();
//
//	//ダメだったコード
//	//if (isHit && (mouse.GetButtonDown() & mouse.BTN_LEFT))
//	//{
//	//	DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(&bestNormal);
//	//	normal = DirectX::XMVector3Normalize(normal);
//
//	//	normalX = DirectX::XMVectorGetX(normal);
//	//	normalY = DirectX::XMVectorGetY(normal);
//	//	normalZ = DirectX::XMVectorGetZ(normal);
//
//	//	// ヒット処理
//	//	position = bestPosition;
//	//	angle.x = atan2f(normalZ, normalY);
//	//	angle.z = -atan2f(normalX, normalY);
//	//	isGround = true;
//	//	velocity = { 0.0f,0.0f,0.0f };
//	//}
//	
//	if (isHit && (mouse.GetButtonDown() & mouse.BTN_LEFT))
//	{
//		DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(&bestNormal);
//		normal = DirectX::XMVector3Normalize(normal);
//
//		// 1. 新しい「上」を法線にする
//		DirectX::XMVECTOR newUp = normal;
//
//		// 2. 「前」方向をとりあえず決める（真上・真下対策付き）
//		DirectX::XMVECTOR worldForward = DirectX::XMVectorSet(0, 0, 1, 0);
//		if (fabsf(DirectX::XMVectorGetY(newUp)) > 0.99f) {
//			worldForward = DirectX::XMVectorSet(1, 0, 0, 0);
//		}
//
//		// 3. 右と前を再計算して「正規直交基底」を作る
//		DirectX::XMVECTOR newRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(newUp, worldForward));
//		DirectX::XMVECTOR newForward = DirectX::XMVector3Cross(newRight, newUp);
//
//		// 4. 回転行列を作成
//		DirectX::XMMATRIX rot = DirectX::XMMatrixIdentity();
//		rot.r[0] = newRight;
//		rot.r[1] = newUp;
//		rot.r[2] = newForward;
//
//		// 5. 行列からクォータニオンに変換し、そこからオイラー角を「安全に」取り出す
//		DirectX::XMVECTOR q = DirectX::XMQuaternionRotationMatrix(rot);
//
//		// 角度を抽出する際の補助関数（もし自作していなければ、以下のように直接計算）
//		// ※この時、angle.y (ヨー) は現在の向きを維持するようにしないと、クリックのたびにキャラの正面が変わります
//
//		// シンプルに法線から直接角度を入れる方法に一度戻しましょう
//		float nx = DirectX::XMVectorGetX(normal);
//		float ny = DirectX::XMVectorGetY(normal);
//		float nz = DirectX::XMVectorGetZ(normal);
//
//		// Z軸の回転（左右の傾き）
//		angle.z = atan2f(-nx, ny);
//		// X軸の回転（前後の傾き）
//		// ※nzが効くように。ピタッと張り付かせるための最短ルートです
//		angle.x = atan2f(nz, sqrtf(nx * nx + ny * ny));
//
//		// 位置と状態の更新
//		position = closestHitPositionToFaceCenter;
//		isGround = true;
//		velocity = { 0, 0, 0 };
//	}
//	//if (isHit && (mouse.GetButtonDown() & mouse.BTN_LEFT))
//	//{
//	//	DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(&bestNormal);
//	//	normal = DirectX::XMVector3Normalize(normal);
//
//	//	//　新しい上方向ベクトル（法線）
//	//	DirectX::XMVECTOR newUp = normal;
//
//	//	//　仮の右方向ベクトルを計算（法線が真上・真下に近い場合の回避処理付き）
//	//	DirectX::XMVECTOR worldForward = DirectX::XMVectorSet(0, 0, 1, 0);
//	//	if (fabsf(DirectX::XMVectorGetY(newUp)) > 0.99f) {
//	//		worldForward = DirectX::XMVectorSet(1, 0, 0, 0); // 真上/真下なら前方向を変える
//	//	}
//
//	//	DirectX::XMVECTOR newRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(newUp, worldForward));
//	//	DirectX::XMVECTOR newForward = DirectX::XMVector3Cross(newRight, newUp);
//
//	//	//　回転行列を作成
//	//	DirectX::XMMATRIX rotationMatrix;
//	//	rotationMatrix.r[0] = newRight;
//	//	rotationMatrix.r[1] = newUp;
//	//	rotationMatrix.r[2] = newForward;
//	//	rotationMatrix.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
//
//	//	// ヒット処理
//	//	position = closestHitPositionToFaceCenter;//bestPosition
//
//	//	// もし angle を直接更新したい場合（簡易版）
//	//	// 法線(nx, ny, nz)に基づいた角度計算の修正
//	//	float nx = DirectX::XMVectorGetX(normal);
//	//	float ny = DirectX::XMVectorGetY(normal);
//	//	float nz = DirectX::XMVectorGetZ(normal);
//
//	//	angle.x = asinf(nz);    //
//	//	angle.z = atan2f(-nx, ny);
//	//	angle.y = 0; // 必要に応じて現在の向きを保持
//
//	//	isGround = true;
//	//	velocity = { 0.0f,0.0f,0.0f };
//	//}
//}
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

	// --- ヒットした時の配置処理 ---
	if (bestBlock) {
		// 【重要】新しく作った FindBestFace を呼び出す
		// レイが当たった地点(bestHitPoint)を渡して、最適な面データをもらう
		Block::FaceData bestFace = bestBlock->FindBestFace(bestHitPoint);

		// 1. 位置を面の中心に設定
		position = bestFace.worldPos;

		// 2. 法線を正規化して角度を計算
		DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(&bestFace.worldNormal);
		normal = DirectX::XMVector3Normalize(normal);

		float nx = DirectX::XMVectorGetX(normal);
		float ny = DirectX::XMVectorGetY(normal);
		float nz = DirectX::XMVectorGetZ(normal);

		// Z軸の回転（左右の傾き）
		angle.z = atan2f(-nx, ny);
		// X軸の回転（前後の傾き）
		angle.x = atan2f(nz, sqrtf(nx * nx + ny * ny));
		// angle.y は必要に応じて現在の値を維持するか、0にする

		// 3. 状態の更新
		isGround = true;
		velocity = { 0, 0, 0 };
		standingGroup = bestBlock->GetGroup();
	}
}



////接地処理
//void Player::Grounding()
//{
//	bool isHitLocal = false; // この関数内での当たり判定結果
//	Block* hitBlockPtr = nullptr; // 当たったブロックを一時保存
//
//	
//
//	bool onGround = isGround; //目のフレームのisGroundを保存
//	isGround = false;
//
//	//プレイヤーのローカルdownベクトルを計算
//	DirectX::XMVECTOR UP = DirectX::XMVectorSet(0, 1, 0, 0);
//	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
//	DirectX::XMVECTOR localUp = DirectX::XMVector3TransformNormal(UP, R);
//	DirectX::XMVECTOR localDown = DirectX::XMVectorNegate(localUp);//符号を反転させてdownベクトルを求める
//	//現在の速度ベクトルを取得
//	DirectX::XMVECTOR velocityVec = DirectX::XMLoadFloat3(&velocity);
//	//playerが下がっているか、Dotで確かめる。
//	float UpOrDown = DirectX::XMVectorGetX(DirectX::XMVector3Dot(velocityVec, localDown));
//
//
//	//playerが降下中だったら
//	if (UpOrDown > 0.0f)
//	{
//		//重力抽出
//		DirectX::XMVECTOR downVelocityVec = DirectX::XMVectorScale(localDown, UpOrDown);
//		DirectX::XMStoreFloat3(&DownGravity, downVelocityVec);
//
//
//		//レイの始点と終点を求める
//		DirectX::XMVECTOR START, END;
//		DirectX::XMFLOAT3 start, end;
//	    //playerのposをぶち奪い
//		DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
//
//		//                
//		START = DirectX::XMVectorMultiplyAdd(localUp, DirectX::XMVectorReplicate(0.5f), playerPosVec); //playerPos + localUp * 0.5f
//		END = playerPosVec;//DirectX::XMVectorMultiplyAdd(localDown, DirectX::XMVectorReplicate(1.5f), playerPosVec); //playerPos + localDown * 1.0f
//
//		DirectX::XMStoreFloat3(&start, START);
//		DirectX::XMStoreFloat3(&end, END);
//
//		DirectX::XMFLOAT3 hitPosition, hitNormal;
//		float hitDistance;
//		float minDistance = FLT_MAX; // 一番近い距離を保存する変数
//		//bool isHit = false;
//		DirectX::XMFLOAT3 bestPosition;
//
//		for (const auto& group : BlockManager::Instance().GetGroups())
//		{
//			for (const auto& block : group->GetBlocks())
//			{
//				if (Collision::RayCast(start, end, block->Gettranceform(), block->GetModel(), hitPosition, hitNormal,hitDistance))
//				{
//					SearchClosestHitPositionToFaceCenter(hitPosition, block.get());
//					minDistance = hitDistance;
//					bestPosition = hitPosition;
//					isHitLocal = true;      // ローカル変数に保存
//					hitBlockPtr = block.get(); // 当たったブロックを保存
//				}
//			}
//		}
//		if (isHitLocal)//isHit
//		{
//			OnLanding();
//			position = closestHitPositionToFaceCenter;//hitPosition
//			velocity = { 0,0,0 };
//			isGround = true;
//			standingGroup = hitBlockPtr->GetGroup(); // ここでグループを保存！
//		}
//		else
//		{
//			isGround = onGround;
//			if (!isGround) standingGroup = nullptr;
//		}
//	}
//	else
//	{
//		// 上に向かっている、または止まっているときは 0 にする
//		DownGravity = { 0.0f, 0.0f, 0.0f };
//	}
//}

void Player::Grounding()
{
	bool isHitLocal = false;
	Block* hitBlockPtr = nullptr;

	bool onGround = isGround;
	isGround = false;

	DirectX::XMVECTOR UP = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMVECTOR localUp = DirectX::XMVector3TransformNormal(UP, R);
	DirectX::XMVECTOR localDown = DirectX::XMVectorNegate(localUp);

	DirectX::XMVECTOR velocityVec = DirectX::XMLoadFloat3(&velocity);
	float UpOrDown = DirectX::XMVectorGetX(DirectX::XMVector3Dot(velocityVec, localDown));

	if (UpOrDown > 0.0f)
	{
		DirectX::XMVECTOR downVelocityVec = DirectX::XMVectorScale(localDown, UpOrDown);
		DirectX::XMStoreFloat3(&DownGravity, downVelocityVec);

		DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR START = DirectX::XMVectorMultiplyAdd(localUp, DirectX::XMVectorReplicate(0.5f), playerPosVec);
		DirectX::XMVECTOR END = DirectX::XMVectorMultiplyAdd(localDown, DirectX::XMVectorReplicate(0.1f), playerPosVec);

		DirectX::XMFLOAT3 start, end;
		DirectX::XMStoreFloat3(&start, START);
		DirectX::XMStoreFloat3(&end, END);

		DirectX::XMFLOAT3 hitPosition, hitNormal;
		float hitDistance;
		float minDistance = FLT_MAX;
		DirectX::XMVECTOR bestHitPointVec;

		for (const auto& group : BlockManager::Instance().GetGroups())
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

		if (isHitLocal)
		{
			Block::FaceData bestFace = hitBlockPtr->FindBestFace(bestHitPointVec);

			OnLanding();

			// 1. 位置の更新
			position = bestFace.worldPos;

			// 2. 【追加】角度の更新（面の法線に合わせる）
			DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(&bestFace.worldNormal);
			normal = DirectX::XMVector3Normalize(normal);

			float nx = DirectX::XMVectorGetX(normal);
			float ny = DirectX::XMVectorGetY(normal);
			float nz = DirectX::XMVectorGetZ(normal);

			// 法線から X軸と Z軸の傾きを計算
			angle.z = atan2f(-nx, ny);
			angle.x = atan2f(nz, sqrtf(nx * nx + ny * ny));
			// ※ angle.y (旋回) はそのまま維持することで、着地した瞬間にキャラが急に回るのを防ぎます

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

//法線と外積がおなじになったらバグる問題
