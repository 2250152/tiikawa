#pragma once

#include <DirectXMath.h>

//カメラコントローラー
class CameraController
{
public:
	// インスタンス取得
	static CameraController& Instance()
	{
		static CameraController instance;
		return instance;
	}

	//初期化
	void Initialize();

	//更新処理
	void Update(float elapsedTime);

	//デバッグ用GUI描画
	void DrawDebugGUI();

	//ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

private:
	const float PIDIV180 = 0.017452f;	// PAI/180
	const float MAX_CAMERA_DISTANCE = 50.0F;
	const float MIN_CAMERA_DISTANCE = 3.0F;
	const float YAW_MAX = 1.3f;
	const float YAW_MIN = -1.3f;

	DirectX::XMFLOAT3 target = { 0,0,0 }; //注視点
	DirectX::XMFLOAT3 eye = { 0, 0, 0 }; //視点
	DirectX::XMFLOAT3 front = {0, 0, 0}; //前方向ベクトル
	DirectX::XMFLOAT3 up = {0, 0, 0}; //上方向ベクトル
	DirectX::XMFLOAT3 right = {0, 0, 0}; //右方向ベクトル
	float             range = 10.0f; //注視点から視点までの距離

	//マウス感度！設定できるようにするなら変えてね
	float mouseSensitivity = 0.3f; //変更済
	float wheelSensitivity = 0.01f; //変更済
	int wheelParam = 0; // GUI描画用変数

	////---------------------------------------------------------------------------------------------------
	///*極座標制御*/
	DirectX::XMFLOAT3 angle = { 0, 0, 0 }; //回転量
	//DirectX::XMFLOAT3 position = { 0, 0, 0 }; //カメラ位置(デバッグGUI用)
	////-------- クォータニオン ----------------------------------------------------------------
	DirectX::XMFLOAT4	orientation = { 0, 0, 0, 1};
	float				yaw = 0.0f, pitch = 0.0f, roll = 0.0f;
};