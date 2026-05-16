#include "Stage.h"
#include "BlockManager.h"
#include "Group.h"
#include "Block_Normal.h"
#include "Block_Start.h"
#include "Block_Stop.h"
#include "Block_Goal.h"


void Stage::Load(int stageNo)
{
    BlockManager::Instance().Clear();

    //もうここで全部作ってね
    switch (stageNo)
    {
         case 1:
         {
             auto start = std::make_unique<Group>(GroupType::Start);
             auto groupNormal = std::make_unique<Group>(GroupType::Normal);
             auto groupNormal2 = std::make_unique<Group>(GroupType::Normal);
             auto groupNormal3 = std::make_unique<Group>(GroupType::Stop);
             auto groupNormal4 = std::make_unique<Group>(GroupType::Normal);
             auto groupNormal5 = std::make_unique<Group>(GroupType::Normal);
             auto groupGoal = std::make_unique<Group>(GroupType::Goal);
           //↓最初からつながってるやつはここを一緒にしてね(下二つみたいな感じ！！)
             start->CreateBlock<BlockStart>(GroupType::Start,DirectX::XMFLOAT3(0, 0, 0));
             start->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(1, 0, 0));
                                                       //↑ここはブロック自体の働きだから合わせず欲しいブロックのものを
             groupNormal->CreateBlock<BlockNormal>(GroupType::Normal,DirectX::XMFLOAT3(3, 0, 2));
             groupNormal2->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(1, 9, 2));
             groupNormal3->CreateBlock<BlockStop>(GroupType::Stop, DirectX::XMFLOAT3(0, 5, 0));
             groupNormal4->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(1, 6, 0));
             groupNormal5->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(0, 7, 2));
             groupGoal->CreateBlock<BlockGoal>(GroupType::Goal, DirectX::XMFLOAT3(0, 9, 9));
             
            //groupNormal->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(0, 0, 5));
            //groupNormal2->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(0, 0, -5));
            //groupNormal3->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(0, -5, 0));
            //groupNormal4->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(0, 5, 0));
            //groupNormal5->CreateBlock<BlockNormal>(GroupType::Normal, DirectX::XMFLOAT3(-5, 0, 0));
			//groupGoal->CreateBlock<BlockGoal>(GroupType::Goal, DirectX::XMFLOAT3(10, 0, 0));
             

             BlockManager::Instance().AddGroup(std::move(start));
             BlockManager::Instance().AddGroup(std::move(groupNormal));
             BlockManager::Instance().AddGroup(std::move(groupNormal2));
             BlockManager::Instance().AddGroup(std::move(groupNormal3));
             BlockManager::Instance().AddGroup(std::move(groupNormal4));
             BlockManager::Instance().AddGroup(std::move(groupNormal5));
             BlockManager::Instance().AddGroup(std::move(groupGoal));
             break;
         }
         
         case 2:
         {
             // 別配置
             auto start = std::make_unique<Group>(GroupType::Start);
             auto normal1 = std::make_unique<Group>(GroupType::Normal);
             start->CreateBlock<BlockStart>(GroupType::Start,DirectX::XMFLOAT3(0, 0, 0));
             normal1->CreateBlock<BlockNormal>(GroupType::Normal,DirectX::XMFLOAT3(5, 3, 0));

             BlockManager::Instance().AddGroup(std::move(start));
             BlockManager::Instance().AddGroup(std::move(normal1));

             break;
         }
         case 10:
         {
             // ExceptHitTest
             auto start = std::make_unique<Group>(GroupType::Start);
             auto normal1 = std::make_unique<Group>(GroupType::Normal);
             auto normal2 = std::make_unique<Group>(GroupType::Normal);
             auto normal3 = std::make_unique<Group>(GroupType::Normal);
             start->CreateBlock<BlockStart>(GroupType::Start,DirectX::XMFLOAT3(0, 0, 0));
             normal1->CreateBlock<BlockNormal>(GroupType::Normal,DirectX::XMFLOAT3(0, 4, 0));
             normal2->CreateBlock<BlockNormal>(GroupType::Normal,DirectX::XMFLOAT3(2, 0, 0));
             normal3->CreateBlock<BlockNormal>(GroupType::Normal,DirectX::XMFLOAT3(0, 0, 2));

             BlockManager::Instance().AddGroup(std::move(start));
             BlockManager::Instance().AddGroup(std::move(normal1));
             BlockManager::Instance().AddGroup(std::move(normal2));
             BlockManager::Instance().AddGroup(std::move(normal3));

             break;
         }
    }
}