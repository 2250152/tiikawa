#include "Block_Stop.h"

//コンストラクタ
BlockStop::BlockStop(GroupType type, DirectX::XMFLOAT3 pos)
{
	model = new Model("Data/Model/BlockStop/BlockStop.mdl");
	scale.x = scale.y = scale.z = 0.01f;

	position = pos;

}

BlockStop::~BlockStop()
{
	delete model;
}

void BlockStop::Update(float elapsedTime)
{
	UpdateTransform();
}

void BlockStop::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}