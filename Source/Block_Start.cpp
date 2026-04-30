#include "Block_Start.h"
#include<imgui.h>
//コンストラクタ
BlockStart::BlockStart(GroupType type, DirectX::XMFLOAT3 pos)
{
	model = new Model("Data/Model/BlockNormal/BlockNormal.mdl");
	scale.x = scale.y = scale.z = 0.01f;
	size = model->GetSize();
	position = pos;
	
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

//void BlockStart::Move(float elapsedTime, const std::vector<Block*>& allBlocks)
//{
//	if (!isMoving) return;
//    float nextX = position.x + 1 * elapsedTime;
//
//    for (auto other : allBlocks)
//    {
//        if (other == this) continue;
//   
//        if (other->GetGroup() == this->GetGroup()) continue;zzz
//
//        if (WillHit(other, nextX))
//        {
//            Stop();
//            return;
//        }
//    }
//
//    position.x = nextX;
//}


void BlockStart::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		//トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//位置
			ImGui::InputFloat3("Sizex", &size.x);
			ImGui::InputFloat3("Sizey", &size.y);
			ImGui::InputFloat3("Sizez", &size.z);
			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//スケール
			ImGui::InputFloat3("Scale", &scale.x);
			//速力
			//ImGui::InputFloat3("Velocity", &velocity.x);
		}
	}
	ImGui::End();
}
