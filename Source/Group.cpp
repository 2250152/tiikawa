#include "Group.h"
#include "Block.h"
//‚±‚±‚ÅƒOƒ‹[ƒv•ª‚¯‚µ‚½‚â‚Â‚ç‚ğ“®‚©‚»‚¤‚Ì‰ï

void Group::Update(float elapsedTime,const std::vector<Group*>& allGroups)
{
	

	Rotate();

	if (type == GroupType::Start)
	Move(elapsedTime, allGroups);

	for (auto& block : blocks)
	{
		block->Update(elapsedTime);
	}
}

void Group::Move(float elapsedTime, const std::vector<Group*>& allGroups)
{
	if (state != Moving) return;

	float dx = 1.0f * elapsedTime;


	for (auto& g : allGroups)
	{
		if (g == this) continue;

		if (WillHit(g, dx))
		{
			Merge(g);
			return;
		}
	}

	for (auto& b : blocks)
	{
		b->position.x += dx;
	}
}

void Group::Rotate()
{

}

void Group::Merge(Group* other)
{
	for (auto& block : other->blocks)
	{
		block->SetGroup(this);
		blocks.push_back(std::move(block));
	}
	other->blocks.clear();
	state = Idle;
}


void Group::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (auto& block : blocks)
	{
		block->Render(rc, renderer);
	}
}

void Group::AddBlock(std::unique_ptr<Block> block)
{
	block->SetGroup(this);
	blocks.push_back(std::move(block));
}

void Group::Go()
{
	state = Moving;
}

bool Group::WillHit(Group* otherGroup, float dx)
{
	for (auto& a : blocks)
	{
		DirectX::XMFLOAT3 minA, maxA;
		a->GetAABB(minA, maxA);

		minA.x += dx;
		maxA.x += dx;

		for (auto& b : otherGroup->blocks)
		{
			DirectX::XMFLOAT3 minB, maxB;
			b->GetAABB(minB, maxB);

			if ((minA.x <= maxB.x && maxA.x >= minB.x) &&
				(minA.y <= maxB.y && maxA.y >= minB.y) &&
				(minA.z <= maxB.z && maxA.z >= minB.z))
			{
				return true;
			}
		}
	}
	return false;
}

bool Group::WillHitAnyGroup(float dx, const std::vector<Group*>& allGroups)
{
	for (auto& otherGroup : allGroups)
	{
		if (otherGroup == this) continue;

		for (auto& a : blocks)
		{
			DirectX::XMFLOAT3 minA, maxA;
			a->GetAABB(minA, maxA);

			minA.x += dx;
			maxA.x += dx;

			for (auto& b : otherGroup->blocks)
			{
				DirectX::XMFLOAT3 minB, maxB;
				b->GetAABB(minB, maxB);

				if ((minA.x <= maxB.x && maxA.x >= minB.x) &&
					(minA.y <= maxB.y && maxA.y >= minB.y) &&
					(minA.z <= maxB.z && maxA.z >= minB.z))
				{
					return true;
				}
			}
		}
	}

	return false;
}