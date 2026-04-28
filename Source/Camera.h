#pragma once

#include<DirectXMath.h>

//カメラ
class Camera
{
private:
	Camera(){}
	~Camera(){}

public:
	//唯一のインスタンス取得
	static Camera& Instance()
	{
		static Camera camera;
		return camera;
	}
		//指定方向を向く
		void SetlookAt(const DirectX::XMFLOAT3 & eye,
			      const DirectX::XMFLOAT3 & focus, const DirectX::XMFLOAT3 & up);

		//パースペクティブ設定
		void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

		//ビュー行列取得
		const DirectX::XMFLOAT4X4& GetView() const { return view; }

		//プロジェクション行列取得
		const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

		//視点取得
		const DirectX::XMFLOAT3& GetEye() const { return eye; }

		const DirectX::XMFLOAT3& GetFocus() const { return focus; }

		const DirectX::XMFLOAT3& GetUp() const { return up; }

		const DirectX::XMFLOAT3& GetFront() const { return front; }

		const DirectX::XMFLOAT3& GetRight() const { return right; }
		

	private:
		DirectX::XMFLOAT4X4    view = {};  //←行列を表す半数の型
		DirectX::XMFLOAT4X4    projection = {};//日本語の意味：投影

		DirectX::XMFLOAT3      eye = {};
		DirectX::XMFLOAT3      focus = {};

		DirectX::XMFLOAT3      up = {};
		DirectX::XMFLOAT3      front = {};
		DirectX::XMFLOAT3      right = {};

};
