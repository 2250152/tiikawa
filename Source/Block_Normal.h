#pragma once

#include "Block.h"
#include "System/Model.h"

enum class GroupType;

//ふっつーのブロック
class BlockNormal :public Block
{
public:
	BlockNormal(GroupType type);
	~BlockNormal();

	void Update(float elapsedTime) override;

	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	void Move(float elapsedTime, const std::vector<Block*>& allBlocks);



};