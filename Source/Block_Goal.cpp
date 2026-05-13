#include "Block_Goal.h"

//コンストラクタ
BlockGoal::BlockGoal(GroupType type, DirectX::XMFLOAT3 pos)
{
	model = new Model("Data/Model/BlockNormal/BlockNormal.mdl");
	scale.x = scale.y = scale.z = 0.01f;

	position = pos;

	BarrierEffect = new Effect("Data/Effect/goal.efk");
	BarrierbreakEffect = new Effect("Data/Effect/goalEnd.efk");
	BarrierEffect->Play(position);
}

BlockGoal::~BlockGoal()
{
	delete model;
	delete BarrierEffect;
	delete BarrierbreakEffect;
}

void BlockGoal::Update(float elapsedTime)
{
	UpdateTransform();

	
}

void BlockGoal::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

//void BlockNormal::Move(float elapsedTime, const std::vector<Block*>& allBlocks)
//{
//	if (!isMoving) return;
//    float nextX = position.x + 1 * elapsedTime;
//
//    for (auto other : allBlocks)
//    {
//        if (other == this) continue;
//
//        if (other->GetGroup() == this->GetGroup()) continue;
//   
//        if (WillHit(other, nextX))
//        {
//            Stop();
//            return;
//        }
//    }
//
//    position.x = nextX;
//}

