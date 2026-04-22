#pragma once
//#include "BlockManager.h"
#include <vector>
#include <set>
#include "System/ModelRenderer.h"
#include "Block.h"

class Block;

class Group
{
private:
	
public:
	Group() {}
	~Group() {}
	void Update();

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void Move();

	void Rotate();

	void UpdateMember();

private:
	std::vector<std::unique_ptr<Block>> blocks;
};