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
protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3	angle = { 0,0,0 };
	DirectX::XMFLOAT3 direction = { 0,0,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	BlockManager* manager = nullptr;
	int group;
};