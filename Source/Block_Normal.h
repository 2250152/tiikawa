#pragma once

#include "Block.h"
#include "System/Model.h"

//ふっつーのブロック
class BlockNormal :public Block
{
public:
	BlockNormal();
	~BlockNormal();

	void Update(float elapsedTime) override;

	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

private:
	Model* model = nullptr;

};