#include "BlockManager.h"
#include"System/Input.h"
//ぶち消し
void BlockManager::Remove(Group* group)
{
	removes.insert(group);
}

void BlockManager::Update(float elapsedTime)
{
	InputMove();

	std::vector<Block*> allBlocks;
	std::vector<Group*> allGroups;

	//消したっていい
	for (auto& group : groups)
	{
		for (auto& block : group->GetBlocks())
		{
			allBlocks.push_back(block.get());
		}
	}

	for (auto& group : groups)
	{
		allGroups.push_back(group.get());
	}

	//普通の更新ちゃん
	for (auto& group : groups)
	{
		
		group->Update(elapsedTime,allGroups);
	}



	

	//当たり判定
	for (int i = 0; i < allBlocks.size(); i++)
	{
		for (int j = i + 1; j < allBlocks.size(); j++)
		{
			Block* a = allBlocks[i];
			Block* b = allBlocks[j];

			if (a->IsHit(b))
			{
				if (a->IsHit(b)==1)
				a->position.x -= 1 * elapsedTime;
				else if (a->IsHit(b)==2)
				a->position.x += 1 * elapsedTime;
				else if (a->IsHit(b)==3)
				a->position.y -= 1 * elapsedTime;
				else if (a->IsHit(b)==4)
				a->position.y += 1 * elapsedTime;
				else if (a->IsHit(b)==5)
				a->position.z -= 1 * elapsedTime;
				else if (a->IsHit(b)==6)
				a->position.z += 1 * elapsedTime;

				Group* groupA = a->GetGroup();
				Group* groupB = b->GetGroup();

				//同じグループなら無視
				//if (groupA == groupB) continue;
				//
				////合体処理
				//groupA->Merge(groupB);
				//Remove(groupB);
				//a->Stop();
				//b->Stop();
				break;
			}

		}
	}

	




	//グループぶち消し
	for (Group* group : removes)
	{
		auto it = std::find_if(groups.begin(), groups.end(),
			[group](const std::unique_ptr<Group>& g)
			{
				return g.get() == group;
			});
		if (it != groups.end())
		{
			groups.erase(it);
		}
		//delete group;
	}
	removes.clear();
}

void BlockManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (auto& group : groups)
	{
		group->Render(rc,renderer);
	}
}

void BlockManager::InputMove()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		for (auto& group : groups)
		{
			group->Go();
		}
	}
}