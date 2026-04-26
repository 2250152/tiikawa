#pragma once
#pragma once

#include "Block.h"
#include "System/Model.h"

//ふっつーのブロック
class BlockStart :public Block
{
public:
	BlockStart();
	~BlockStart();

	void Update(float elapsedTime) override;

	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	void Move(float elapsedTime);




};