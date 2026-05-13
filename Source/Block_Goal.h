#pragma once
#pragma once

#include "Block.h"
#include "System/Model.h"
#include "GroupType.h"
#include "Effect.h"
enum class GroupType;

//ふっつーのブロック
class
	BlockGoal :public Block
{
public:

	BlockGoal(GroupType type, DirectX::XMFLOAT3 pos);
	~BlockGoal();

	void Update(float elapsedTime) override;

	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	void DrawDebugGUI()
	{

	}

	//void Move(float elapsedTime, const std::vector<Block*>& allBlocks)override;

	GroupType GetType() const override { return GroupType::Normal; }

	Effect* BarrierEffect = nullptr;

};