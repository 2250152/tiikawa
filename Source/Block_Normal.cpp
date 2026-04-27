#include "Block_Normal.h"

//コンストラクタ
BlockNormal::BlockNormal(GroupType type)
{
	model = new Model("Data/Model/BlockNormal/BlockNormal.mdl");
	scale.x = scale.y = scale.z = 0.01f;
	position = { 10,0,0 };

}

BlockNormal::~BlockNormal()
{
	delete model;
}

void BlockNormal::Update(float elapsedTime)
{
	UpdateTransform();
}

void BlockNormal::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

void BlockNormal::Move(float elapsedTime, const std::vector<Block*>& allBlocks)
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