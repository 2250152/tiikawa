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
	void Update(float elpsedTime);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void Move(float elapsedTime);

	void Rotate();

	void UpdateMember();

	void AddBlock(std::unique_ptr<Block> block)
	{
		blocks.push_back(std::move(block));
	}

private:
	std::vector<std::unique_ptr<Block>> blocks;
};