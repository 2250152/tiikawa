#include "Group.h"
//‚±‚±‚ÅƒOƒ‹[ƒv•ª‚¯‚µ‚½‚â‚Â‚ç‚ğ“®‚©‚»‚¤‚Ì‰ï

void Group::Update(float elapsedTime)
{
	Move(elapsedTime);

	Rotate();

	UpdateMember();

}

void Group::Move(float elapsedTime)
{
	for (auto& block : blocks)
	{
		block->Update(elapsedTime);
	}
}

void Group::Rotate()
{

}

void Group::UpdateMember()
{

}

void Group::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (auto& block : blocks)
	{
		block->Render(rc, renderer);
	}
}