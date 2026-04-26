#include "Block_Normal.h"

//コンストラクタ
BlockNormal::BlockNormal()
{
	model = new Model("Data/Model/BlockNormal/BlockNormal.mdl");
	scale.x = scale.y = scale.z = 0.01f;

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