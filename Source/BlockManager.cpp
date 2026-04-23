#include "BlockManager.h"
//‚Ô‚¿Á‚µ
void BlockManager::Remove(Group* group)
{
	removes.insert(group);
}

void BlockManager::Update(float elapsedTime)
{
	for (auto& group : groups)
	{
		group->Update(elapsedTime);
	}

	//ƒOƒ‹[ƒv‚Ô‚¿Á‚µ
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