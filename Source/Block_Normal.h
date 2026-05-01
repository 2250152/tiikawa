#pragma once

#include "Block.h"
#include "System/Model.h"
#include "GroupType.h"
enum class GroupType;

//ふっつーのブロック
class BlockNormal :public Block
{
public:
	BlockNormal(GroupType type, DirectX::XMFLOAT3 pos);
	~BlockNormal();

	void Update(float elapsedTime) override;

	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	void DrawDebugGUI()
	{

	}

	//void Move(float elapsedTime, const std::vector<Block*>& allBlocks)override;

	GroupType GetType() const override { return GroupType::Normal; }


};