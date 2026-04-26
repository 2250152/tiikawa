#include "Block_Start.h"

//コンストラクタ
BlockStart::BlockStart()
{
	model = new Model("Data/Model/BlockNormal/BlockNormal.mdl");
	scale.x = scale.y = scale.z = 0.01f;
	position = { 10,0,0 };
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

void BlockStart::Move(float elapsedTime)
{
	if (!isMoving) return;
	position.x += 0.01f * elapsedTime;
}