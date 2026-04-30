#pragma once
#include "System/ModelRenderer.h"

class BlockManager;
class Group;

class Block
{
public:
	Block() {}
	virtual ~Block() {}

	void Destroy();

	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

	virtual void Update(float elapsedTime) = 0;

	//virtual void Move(float elapsedTime, const std::vector<Block*>& allBlocks) = 0;

	void GetAABB(DirectX::XMFLOAT3& min, DirectX::XMFLOAT3& max);

	void UpdateTransform()
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMMATRIX W = S * R * T;
		DirectX::XMStoreFloat4x4(&transform, W);
	}
	//ñ ÇÃíÜêSÇÃç¿ïWÇéÊÇÎÇ§ÇÃâÔ
	DirectX::XMFLOAT3 GetTopCenter();
	DirectX::XMFLOAT3 GetBottomCenter();
	DirectX::XMFLOAT3 GetFrontCenter();
	DirectX::XMFLOAT3 GetBackCenter();
	DirectX::XMFLOAT3 GetRightCenter();
	DirectX::XMFLOAT3 GetLeftCenter();

	int IsHit(Block* other);
	
	Group* GetGroup() const { return group; }
	void SetGroup(Group* g) { group = g; }
	

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3	angle = { 0,0,0 };

	virtual void DrawDebugGUI()=0;

	Model* GetModel() { return model; }
	const DirectX::XMFLOAT4X4 Gettranceform() const{ return transform; }

//	bool WillHit(Block* other, float nextX);
protected:
	DirectX::XMFLOAT3 size;
	
	DirectX::XMFLOAT3 direction = { 0,0,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	BlockManager* manager = nullptr;
	Group* group = nullptr;

	float face;
	Model* model = nullptr;



	
};