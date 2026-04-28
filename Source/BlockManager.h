#pragma once
#include <vector>
#include <set>
#include "Block.h"
#include "Group.h"
class BlockManager
{
private:
	BlockManager() {}
	~BlockManager() {}
public:
	static BlockManager& Instance()
	{
		static BlockManager instance;
		return instance;
	}

	void Update(float elapsedTime);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void Remove(Group* group);

	void AddGroup(std::unique_ptr<Group> group)
	{
		groups.push_back(std::move(group));
	}

	void InputMove();



private:
	//‰ò
	std::vector<std::unique_ptr<Group>> groups;
	std::set<Group*> removes;
};