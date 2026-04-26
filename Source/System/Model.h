#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "ModelResource.h"

#include <cfloat>
#include <algorithm>

// モデル
class Model
{
public:
	Model(const char* filename);
	~Model() {}

	struct Node
	{
		const char*			name;
		Node*				parent;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4X4	localTransform;
		DirectX::XMFLOAT4X4	globalTransform;

		std::vector<Node*>	children;
	};

	// 行列計算
	void UpdateTransform();

	// ノードリスト取得
	const std::vector<Node>& GetNodes() const { return nodes; }
	std::vector<Node>& GetNodes() { return nodes; }

	// リソース取得
	const ModelResource* GetResource() const { return resource.get(); }

	DirectX::XMFLOAT3 min = { FLT_MAX,FLT_MAX,FLT_MAX };
	DirectX::XMFLOAT3 max = { -FLT_MAX,-FLT_MAX,-FLT_MAX };

	//座標計算祭り
	void CalculateBounds();
private:
	std::shared_ptr<ModelResource>	resource;
	std::vector<Node>				nodes;
};
