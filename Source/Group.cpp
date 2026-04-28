#include "Group.h"
#include "Block.h"
//ここでグループ分けしたやつらを動かそうの会

//試行錯誤で進めとるからまとまってきたらいらんの消すね

void Group::Update(float elapsedTime,const std::vector<Group*>& allGroups)
{
	

	Rotate();

	if (type == GroupType::Start)
	Move(elapsedTime, allGroups);

	if (pendingMerge)
	{
		Merge(pendingMerge);
		pendingMerge = nullptr;
	}

	for (auto& block : blocks)
	{
		block->Update(elapsedTime);
	}
}

void Group::Move(float elapsedTime, const std::vector<Group*>& allGroups)
{
	if (state != Moving) return;

	float dx = 1.0f * elapsedTime;

	/*for (auto& g : allGroups)
	{
		if (g == this) continue;

		if (WillHit(g, dx))
		{
			pendingMerge = g;
			state = Idle;
			return;
		}
	}

	for (auto& b : blocks)
	{
		b->position.x += dx;
	}*/
	//先動かす
	for (auto& b : blocks)
	{
		b->position.x += dx;
	}

	
	for (auto& g : allGroups)
	{
		if (g == this) continue;

		if (WillHit(g, 0.0f))
		{
			// 戻す
			for (auto& b : blocks)
			{
				b->position.x -= dx;
			}

			pendingMerge = g;
			state = Idle;
			return;
		}
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