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

enum State
{
	Idle,   // Ž~‚Ü‚Á‚Ä‚é
	Moving, // “®‚¢‚Ä‚é
};

class Group
{
private:
	
public:
	Group(GroupType type) : type(type) {}
	~Group() {}

	GroupType GetType() const { return type; }

	void Update(float elpsedTime, const std::vector<Group*>& allGroups);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void Move(float elapsedTime, const std::vector<Group*>& allGroups);

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

	bool WillHit(Group* otherGroup, float dx);

	void Stop() { isMoving = false; }
	void Start() { isMoving = true; }

	bool WillHitAnyGroup(float dx, const std::vector<Group*>& allGroups);

private:
	std::vector<std::unique_ptr<Block>> blocks;

	GroupType type;

	DirectX::XMFLOAT3 position;

	bool isMoving = false;
	bool isBlocked = false;
	State state = Idle;

	Group* pendingMerge = nullptr;

	
};