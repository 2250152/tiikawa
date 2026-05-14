#pragma once
#include <vector>
#include <set>
#include "Block.h"
#include "Group.h"
#include "Effect.h"


class BlockManager
{
private:
	BlockManager()
	{
		hitEffect = new Effect("Data/Effect/Blow11.efk"); 
		willHitEffect = new Effect("Data/Effect/ChoiseBlockY.efk"); 
	}
	~BlockManager() 
	{ 
		delete hitEffect;
		delete willHitEffect;
	}
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


	void DrawDebugGUI();

	void InputRotation();

	//ÉQÉbÉ^Å[
	std::vector<std::unique_ptr<Group>>& const GetGroups() { return groups; }

	void Clear();

private:
	//âÚ
	std::vector<std::unique_ptr<Group>> groups;
	std::set<Group*> removes;

	Effect* hitEffect = nullptr;
	Effect* willHitEffect = nullptr;
	

};