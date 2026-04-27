#include "Block_Start.h"

//コンストラクタ
BlockStart::BlockStart(GroupType type)
{
	model = new Model("Data/Model/BlockNormal/BlockNormal.mdl");
	scale.x = scale.y = scale.z = 0.01f;
	position = { 0,0,0 };
	Stop();
}

BlockStart::~BlockStart()
{
	delete model;
}

void BlockStart::Update(float elapsedTime)
{
	UpdateTransform();
}

void BlockStart::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

void BlockStart::Move(float elapsedTime, const std::vector<Block*>& allBlocks)
{
	if (!isMoving) return;
    float nextX = position.x + 1 * elapsedTime;

    for (auto other : allBlocks)
    {
        if (other == this) continue;

        if (WillHit(other, nextX))
        {
            Stop();
            return;
        }
    }

    position.x = nextX;
}