#include "Group.h"
#include "Block.h"
//‚±‚±‚ÅƒOƒ‹[ƒv•ª‚¯‚µ‚½‚â‚Â‚ç‚ð“®‚©‚»‚¤‚Ì‰ï

void Group::Update(float elapsedTime)
{
	Move(elapsedTime);

	Rotate();

	

	for (auto& block : blocks)
	{
		block->Update(elapsedTime);
	}
}

void Group::Move(float elapsedTime)
{
	for (auto& block : blocks)
	{
		block->Move(elapsedTime);
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