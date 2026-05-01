#pragma once
#pragma once

#include "Block.h"
#include "System/Model.h"
#include "GroupType.h"
enum class GroupType;

//ふっつーのブロック
class BlockStart :public Block
{
public:
	BlockStart(GroupType type, DirectX::XMFLOAT3 pos);
	~BlockStart();

	void Update(float elapsedTime) override;

	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

//	void Move(float elapsedTime, const std::vector<Block*>& allBlocks)override;

	void DrawDebugGUI() override;

	GroupType GetType() const override { return GroupType::Start; }

};