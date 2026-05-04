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
	InputRotation();

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
	//for (int i = 0; i < allBlocks.size(); i++)
	//{
	//	for (int j = i + 1; j < allBlocks.size(); j++)
	//	{
	//		Block* a = allBlocks[i];
	//		Block* b = allBlocks[j];
	//
	//		HitInfo hit = a->IsHit(b);
	//
	//		if (hit.dir != 0)
	//		{
	//			// 押し戻し
	//			if (hit.dir == 1 || hit.dir == 2)
	//				a->position.x += hit.penetration;
	//			else if (hit.dir == 3 || hit.dir == 4)
	//				a->position.y += hit.penetration;
	//			else if (hit.dir == 5 || hit.dir == 6)
	//				a->position.z += hit.penetration;
	//
	//			// 停止
	//			Group* groupA = a->GetGroup();
	//			Group* groupB = b->GetGroup();
	//
	//			groupA->Stop();
	//		}
	//
	//	}
	//}

	




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

void BlockManager::DrawDebugGUI()
{
	for (auto& group : groups)
	{

		group->DrawDebugGUI();
	}
}


void BlockManager::InputRotation()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		for (auto& group : groups)
		{
			group->revolve();
		}
	}
}