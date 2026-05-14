#pragma once
#include <vector>
#include <set>
#include "Block.h"
#include "Group.h"
#include "Effect.h"
#include "System/Audio.h"


class BlockManager
{
private:
	BlockManager()
	{
		hitEffect = new Effect("Data/Effect/Blow11.efk");
		hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");
	}
	~BlockManager()
	{
		/*delete hitEffect;
		delete hitSE;*/
		Clear();
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

	AudioSource* GetHitSE()
	{
		if (!hitSE)
		{
			hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");
		}
		return hitSE;
	}

	Effect* GetHitEffect() 
	{
		if (!hitEffect) 
		{
			hitEffect = new Effect("Data/Effect/Blow11.efk");
		}
		return hitEffect;
	}
private:
	//âÚ
	std::vector<std::unique_ptr<Group>> groups;
	std::set<Group*> removes;

	Effect* hitEffect = nullptr;
	Effect* willHitEffect = nullptr;
	
	AudioSource* hitSE = nullptr;
};