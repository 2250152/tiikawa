#pragma once
//#include "BlockManager.h"
#include <vector>
#include <set>
#include "System/ModelRenderer.h"

enum class GroupType
{
	Normal,
	Start
};

class Block;

class Group
{
private:
	
public:
	Group(GroupType type) : type(type) {}
	~Group() {}

	GroupType GetType() const { return type; }

	void Update(float elpsedTime, const std::vector<Block*>& allBlocks);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void Move(float elapsedTime, const std::vector<Block*>& allBlocks);

	void Rotate();


	void AddBlock(std::unique_ptr<Block> block);

	std::vector<std::unique_ptr<Block>>& GetBlocks()
	{
		return blocks;
	}


	const std::vector<std::unique_ptr<Block>>& GetBlocks() const
	{
		return blocks;
	}


	template<typename T, typename... Args>
	void CreateBlock(Args&&... args)
	{
		auto block = std::make_unique<T>(std::forward<Args>(args)...);
		block->SetGroup(this);
		blocks.push_back(std::move(block));
	}

	void Merge(Group* other);

	void Go();

private:
	std::vector<std::unique_ptr<Block>> blocks;

	GroupType type;


};