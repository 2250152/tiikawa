#include "Group.h"
#include "Block.h"
//‚±‚±‚ÅƒOƒ‹[ƒv•ª‚¯‚µ‚½‚â‚Â‚ç‚ğ“®‚©‚»‚¤‚Ì‰ï

void Group::Update(float elapsedTime,const std::vector<Block*>& allBlocks)
{
	Move(elapsedTime, allBlocks);

	Rotate();



	for (auto& block : blocks)
	{
		block->Update(elapsedTime);
	}
}

void Group::Move(float elapsedTime, const std::vector<Block*>& allBlocks)
{
	for (auto& block : blocks)
	{
		block->Move(elapsedTime, allBlocks);
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
	for (auto& block : blocks)
	{
		block->Start();
	}
}