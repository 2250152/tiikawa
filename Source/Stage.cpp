#include "Stage.h"
#include "BlockManager.h"
#include "Group.h"
#include "Block_Normal.h"
#include "Block_Start.h"


void Stage::Load(int stageNo)
{
    BlockManager::Instance().Clear();

    switch (stageNo)
    {
    case 1:
    {
        auto start = std::make_unique<Group>(GroupType::Start);
        auto groupNormal = std::make_unique<Group>(GroupType::Normal);
        auto groupNormal2 = std::make_unique<Group>(GroupType::Normal);
        auto groupNormal3 = std::make_unique<Group>(GroupType::Normal);
        auto groupNormal4 = std::make_unique<Group>(GroupType::Normal);
        auto groupNormal5 = std::make_unique<Group>(GroupType::Normal);
        auto groupNormal6 = std::make_unique<Group>(GroupType::Normal);

        start->CreateBlock<BlockStart>(GroupType::Start,DirectX::XMFLOAT3(0, 0, 0));
        groupNormal->CreateBlock<BlockNormal>(GroupType::Normal,DirectX::XMFLOAT3(3, 0, 2));
        groupNormal2->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(1, 9, 2));
        groupNormal3->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(0, 5, 0));
        groupNormal4->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(1, 6, 0));
        groupNormal5->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(0, 7, 2));
        groupNormal6->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(1, 0, 0));

        BlockManager::Instance().AddGroup(std::move(start));
        BlockManager::Instance().AddGroup(std::move(groupNormal));
        BlockManager::Instance().AddGroup(std::move(groupNormal2));
        BlockManager::Instance().AddGroup(std::move(groupNormal3));
        BlockManager::Instance().AddGroup(std::move(groupNormal4));
        BlockManager::Instance().AddGroup(std::move(groupNormal5));
        BlockManager::Instance().AddGroup(std::move(groupNormal6));

        break;
    }

    case 2:
    {
        auto start = std::make_unique<Group>(GroupType::Start);

        start->CreateBlock<BlockStart>(
            GroupType::Start,
            DirectX::XMFLOAT3(0, 0, 0));

        // •Ê”z’u
        auto normal1 = std::make_unique<Group>(GroupType::Normal);
        normal1->CreateBlock<BlockNormal>(
            GroupType::Normal,
            DirectX::XMFLOAT3(5, 3, 0));

        BlockManager::Instance().AddGroup(std::move(start));
        BlockManager::Instance().AddGroup(std::move(normal1));

        break;
    }
    }
}