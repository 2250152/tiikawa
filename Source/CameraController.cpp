#include"System/Input.h"
#include"CameraController.h"
#include"Camera.h"
#include<imgui.h>
#define DEBUG


inline float normalizeAngle(float radian) //radianの範囲を変更
{
	long ofs = *(long*)&radian;
	ofs = ofs & 0x80000000;
	ofs = ofs | 0x3F000000;										// 0x3F000000で-180～180
	float ofs_f = *(float*)&ofs;
	int ofs_i = (int)(radian * DirectX::XM_1DIV2PI + ofs_f);	// +ofs_fをはずすと-360～360
	return (radian - (ofs_i * DirectX::XM_2PI));
}

//初期化
void CameraController::Initialize()
{
	/*-----クォータニオン-----*/
	//DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());
	 
	DirectX::XMFLOAT3 n(0, 0, 1);		//軸（正規化）
	float angleRad = 180 * PIDIV180;	//角度（ラジアン）
	 
	orientation = {
		sinf(angleRad / 2) * n.x,
		sinf(angleRad / 2) * n.y,
		sinf(angleRad / 2) * n.z,
		cosf(angleRad / 2)
	};
}

//更新処理
void CameraController::Update(float elapsedTime)
{
	//マウス操作にぶち変え
	Mouse& mouse = Input::Instance().GetMouse();
	float ax = (float)mouse.GetMoveX();
	float ay = (float)mouse.GetMoveY();

	//-----------------------------------------------------
	//--------| 任意軸回転・改 |---------------------------
	//-----------------------------------------------------
	DirectX::XMFLOAT3 Angle = { 0, 0, 0 };

	//視点から注視点へのベクトルと仮の上方向ベクトルが平行なら、仮の上方向ベクトルを異なる値にする
	//回転移動の方向を算出
	DirectX::XMVECTOR upDir;
	DirectX::XMVECTOR rightDir;
	//正規化した視点から注視点へのベクトル
	DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&eye)));
	float isParallel; //2つのベクトルが平行か
	DirectX::XMStoreFloat(&isParallel, DirectX::XMVector3Dot(N, { 0, 1, 0 }));
	if (isParallel == 1 || isParallel == -1)
	{
		rightDir = DirectX::XMVector3Cross({ 0, 0, 1 }, N);
	}
	else
	{
		rightDir = DirectX::XMVector3Cross({ 0, 1, 0 }, N);
	}
	rightDir = DirectX::XMVector3Normalize(rightDir);
	upDir = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(N, rightDir));
	DirectX::XMStoreFloat3(&up, upDir);
	DirectX::XMStoreFloat3(&right, rightDir);

	//-----------------------------------------------------------
	//クォータニオンでカメラ制御
	// XMVECTORクラスへ変換
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orientation);
	DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientationVec);
	if (mouse.GetButton() & mouse.BTN_LEFT)
	{
		angle.x = ax * mouseSensitivity * elapsedTime;
		angle.y = ay * mouseSensitivity * elapsedTime;

		//回転軸と回転角から回転クォータニオン（q）を求める
		// yaw
		DirectX::XMVECTOR qy;
		// yawの範囲制限
		if (yaw - angle.y > YAW_MAX)
		{
			float correctionValue = yaw - YAW_MAX; //補正された回転角
			qy = DirectX::XMQuaternionRotationAxis(rightDir, correctionValue);
			orientationVec = DirectX::XMQuaternionMultiply(orientationVec, qy);
			yaw = normalizeAngle(yaw - correctionValue);
		}
		else if (yaw - angle.y < -YAW_MAX)
		{
			float correctionValue = yaw - YAW_MIN; //補正された回転角
			qy = DirectX::XMQuaternionRotationAxis(rightDir, correctionValue);
			orientationVec = DirectX::XMQuaternionMultiply(orientationVec, qy);
			yaw = normalizeAngle(yaw - correctionValue);
		}
		else
		{
			qy = DirectX::XMQuaternionRotationAxis(rightDir, angle.y);
			orientationVec = DirectX::XMQuaternionMultiply(orientationVec, qy);
			yaw = normalizeAngle(yaw - angle.y);
		}


		// pitch
		DirectX::XMVECTOR qx = DirectX::XMQuaternionRotationAxis(upDir, angle.x);
		orientationVec = DirectX::XMQuaternionMultiply(orientationVec, qx);
		pitch = normalizeAngle(pitch - angle.x);
		// roll(スキップ)
	}

	//カメラ距離
	wheelParam = mouse.GetWheelRotationDelta();
	if (mouse.GetWheelRotationDelta())
	{		
		range -= mouse.GetWheelRotationDelta() * wheelSensitivity;
		if (range > MAX_CAMERA_DISTANCE)
			range = MAX_CAMERA_DISTANCE;
		if (range < MIN_CAMERA_DISTANCE)
			range = MIN_CAMERA_DISTANCE;
	}

#ifdef DEBUG
	//初期位置へ (デバッグ用)
	if (GetKeyState('1') < 0)
	{
		orientationVec = DirectX::XMQuaternionNormalize({ 0.0f, 0.0f, 1.0f, 1.0f });

		yaw = pitch = roll = 0.0f;
	}
#endif // DEBUG

	N = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&eye)));
	DirectX::XMStoreFloat(&isParallel, DirectX::XMVector3Dot(N, { 0, 1, 0 }));
	if (isParallel == 1 || isParallel == -1)
	{
		rightDir = DirectX::XMVector3Cross({ 0, 0, 1 }, N);
	}
	else
	{
		rightDir = DirectX::XMVector3Cross({ 0, 1, 0 }, N);
	}
	rightDir = DirectX::XMVector3Normalize(rightDir);
	upDir = DirectX::XMVector3Cross(N, rightDir);
	upDir = DirectX::XMVector3Normalize(upDir);
	DirectX::XMStoreFloat3(&up, upDir);
	DirectX::XMStoreFloat3(&right, rightDir);

	//クォータニオンを回転行列に変換
	DirectX::XMMATRIX Q = DirectX::XMMatrixRotationQuaternion(orientationVec);

	// orientationVecからorientationを更新
	DirectX::XMStoreFloat4(&orientation, orientationVec);

	//回転行列から前方向ベクトルを取り出す
	DirectX::XMStoreFloat3(&front, Q.r[2]);
	//DirectX::XMVECTOR positionVec = DirectX::XMLoadFloat3(&position);

	//注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	//-------------------------------------------------------------------------------------------------------
	//カメラの視点と注視点を設定
	Camera::Instance().SetlookAt(eye, target, up);
}

//デバッグ用GUI描画
void CameraController::DrawDebugGUI()
{
#ifdef DEBUG
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 1920 - 310, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("CameraController", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("RotateMatrix", ImGuiTreeNodeFlags_DefaultOpen))
		{			
			//注視点
			ImGui::InputFloat3("target", &target.x);
			//視点
			ImGui::InputFloat3("eye", &eye.x);
			//前方向ベクトル
			ImGui::InputFloat3("front", &front.x);
			//右方向ベクトル
			ImGui::InputFloat3("right", &right.x);
			//上方向ベクトル
			ImGui::InputFloat3("up", &up.x);
			//回転
			ImGui::InputFloat("yaw", &yaw);
			ImGui::InputFloat("pitch", &pitch);
			ImGui::InputFloat("roll", &roll);
			//カメラ距離
			ImGui::InputFloat("range", &range);
			//ImGui::SliderFloat("range", &range, MIN_CAMERA_DISTANCE, MAX_CAMERA_DISTANCE, "%.3f");
			//マウス感度
			ImGui::InputFloat("mouseSensitivity", &mouseSensitivity);
			//マウスホイール感度
			ImGui::InputFloat("wheelSensitivity", &wheelSensitivity);
			//マウスホイール値
			ImGui::InputInt("mouseWheelScrollParam", &wheelParam);
		}
	}
	ImGui::End();
#endif //DEBUG
}