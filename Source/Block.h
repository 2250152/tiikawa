#pragma once
#include "System/ModelRenderer.h"

class BlockManager;

class Block
{
public:
	Block() {}
	virtual ~Block() {}

	void Destroy();

	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

	virtual void Update(float elapsedTime) = 0;

	virtual void Move(float elapsedTime) = 0;

	void UpdateTransform()
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMMATRIX W = S * R * T;
		DirectX::XMStoreFloat4x4(&transform, W);
	}
	//DirectX::XMFLOAT3 worldMin =
	//{
	//	position.x + model->min.x * scale.x,
	//	position.y + model->min.y * scale.y,
	//	position.z + model->min.z * scale.z
	//};
	//
	//DirectX::XMFLOAT3 worldMax =
	//{
	//	position.x + model->max.x * scale.x,
	//	position.y + model->max.y * scale.y,
	//	position.z + model->max.z * scale.z
	//};
	//
	//
	//void GetAABB(DirectX::XMFLOAT3& outMin, DirectX::XMFLOAT3& outMax)
	//{
	//	outMin =
	//	{
	//		position.x + model->min.x * scale.x,
	//		position.y + model->min.y * scale.y,
	//		position.z + model->min.z * scale.z
	//	};
	//
	//	outMax =
	//	{
	//		position.x + model->max.x * scale.x,
	//		position.y + model->max.y * scale.y,
	//		position.z + model->max.z * scale.z
	//	};
	//}


	

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3	angle = { 0,0,0 };
	DirectX::XMFLOAT3 direction = { 0,0,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	BlockManager* manager = nullptr;
	int group;

	float face;
	Model* model = nullptr;
};