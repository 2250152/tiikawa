#include "Group.h"
#include "Block.h"
#include "Player.h"
#include<imgui.h>
//ここでグループ分けしたやつらを動かそうの会

//試行錯誤で進めとるからまとまってきたらいらんの消すね

void Group::Update(float elapsedTime,const std::vector<Group*>& allGroups)
{
	if (GetExplosion())
	{
		Explosion(elapsedTime);
		return;
	}
	
	Rotate();

	if (type == GroupType::Start)
	Move(elapsedTime, allGroups);

	for (auto& g : pendingMerge)
	{
		Merge(g);
	}

	pendingMerge.clear();

	if (type == GroupType::Start)
		Rotation(elapsedTime);

	for (auto& block : blocks)
	{
		block->Update(elapsedTime);
	}
	
}

void Group::Move(float elapsedTime, const std::vector<Group*>& allGroups)
{
	if (state != Moving) return;


	Player& player = Player::Instance();
	

	DirectX::XMFLOAT3 v = player.GetDownGravity();

	DirectX::XMFLOAT3 speed = {v.x,v.y,v.z};
	DirectX::XMFLOAT3 move;
	//ここでplayerとか使って変えよう




	move.x = speed.x * elapsedTime;
	move.y = speed.y * elapsedTime;
	move.z = speed.z * elapsedTime;


	int step = 70;

	DirectX::XMFLOAT3 subMove =
	{
		move.x / step,
		move.y / step,
		move.z / step
	};
	if(player.IsGround())
	for (int i = 0; i < step; i++)
	{
		bool hit = false;

		for (auto& g : allGroups)
		{
			if (g == this) continue;

			int hitDir = WillHit(g, subMove);

			if (hitDir != 0)
			{
				hit = true;
				if (g->GetType() == GroupType::Goal)
				{
					//数が足りていたら
					if (this->GetMergeCount() > 10)
					{
						//ゴールの判定

					}
					//数が足りていなかったら
					else
					{
						//爆散
						Explosion(elapsedTime);
					}

				}
				else if (g->GetType() == GroupType::Normal)
				{
					pendingMerge.push_back(g);
				}
				else
				{
					// 少し離す
					/*for (auto& b : blocks)
					{
						b->position.x -= subMove.x * 0.1f;
						b->position.y -= subMove.y * 0.1f;
						b->position.z -= subMove.z * 0.1f;
					}*/
				}

				if (this->GetType() == GroupType::Start)
				{
					//何個つながっているか
					mergeCount++;
				}
				

				state = Idle;
				switch (hitDir)
				{
				case 1:
				{
					float offset =
						(hitBlock->position.y - 1.0f)
						- selfHitBlock->position.y;

					for (auto& b : blocks)
					{
						b->position.y += offset;
					}
					break;
				}
					
				case 2:
				{
					float offset =
						(hitBlock->position.y + 1.0f)
						- selfHitBlock->position.y;

					for (auto& b : blocks)
					{
						b->position.y += offset;
					}
					break;
				}
					
				case 3:
				{
					float offset =
						(hitBlock->position.x - 1.0f)
						- selfHitBlock->position.x;

					for (auto& b : blocks)
					{
						b->position.x += offset;
					}
					break;
				}
				
				case 4:
				{
					float offset =
						(hitBlock->position.x + 1.0f)
						- selfHitBlock->position.x;

					for (auto& b : blocks)
					{
						b->position.x += offset;
					}
					break;
				}
				case 5:
				{
					float offset =
						(hitBlock->position.z - 1.0f)
						- selfHitBlock->position.z;

					for (auto& b : blocks)
					{
						b->position.z += offset;
					}
					break;
				}
				case 6:
				{
					float offset =
						(hitBlock->position.z + 1.0f)
						- selfHitBlock->position.z;

					for (auto& b : blocks)
					{
						b->position.z += offset;
					}
					break;
				}
				}

				//break;
			}
		}

		if (hit)
			break;

		for (auto& b : blocks)
		{

			b->position.x += subMove.x;
			b->position.y += subMove.y;
			b->position.z += subMove.z;
		}
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
	if(state==Idle)
	state = Moving;
}

void Group::revolve(RotateAxis axis,float dir)
{
	if (state != Idle)
		return;

	rotateAxis = axis;
	rotateDir = dir;

	pivot = GetStartBlockCenter();

	currentAngle = 0.0f;
	prevAngle = 0.0f;

	targetAngle = DirectX::XM_PIDIV2;

	visualAngle = 0.0f;
	rotatedAmount = 0.0f;

	state = Rotating;

}

int Group::WillHit(Group* otherGroup, DirectX::XMFLOAT3 move)
{

	for (auto& a : blocks)
	{
		//先を見る
		DirectX::XMFLOAT3 original = a->position;
		a->position.x += move.x;
		a->position.y += move.y;
		a->position.z += move.z;

		for (auto& b : otherGroup->blocks)
		{
			//Y上からあたる
			{
				auto topA = a->GetTopCenter();
				auto bottomB = b->GetBottomCenter();

				float dx = fabs(topA.x - bottomB.x);
				float dz = fabs(topA.z - bottomB.z);
				float dy = fabs(topA.y - bottomB.y);

				if (dx < EPS && dz < EPS && dy < EPS)
				{
					a->position = original;
					hitEvent.active = true;
					hitEvent.pos.push_back(original);
					hitBlock = b.get();
					selfHitBlock = a.get();
					return 1;
				}
			}
			//下からあたる
			{
				auto topB = a->GetBottomCenter();
				auto bottomA = b->GetTopCenter();

				float dx = fabs(topB.x - bottomA.x);
				float dz = fabs(topB.z - bottomA.z);
				float dy = fabs(topB.y - bottomA.y);

				if (dx < EPS && dz < EPS && dy < EPS)
				{
					a->position = original;
					hitEvent.active = true;
					hitEvent.pos.push_back(original);
					hitBlock = b.get();
					selfHitBlock = a.get();
					return 2;
				}
			}

			//X
			{
				auto rightA = a->GetRightCenter();
				auto leftB = b->GetLeftCenter();

				float dy = fabs(rightA.y - leftB.y);
				float dz = fabs(rightA.z - leftB.z);
				float dx = fabs(rightA.x - leftB.x);

				if (dy < EPS && dz < EPS && dx < EPS)
				{
					a->position = original;
					hitEvent.active = true;
					hitEvent.pos.push_back(original);
					hitBlock = b.get();
					selfHitBlock = a.get();
					return 3;
				}
			}
			{
				auto rightB = a->GetLeftCenter();
				auto leftA = b->GetRightCenter();
				float dy = fabs(rightB.y - leftA.y);
				float dz = fabs(rightB.z - leftA.z);
				float dx = fabs(rightB.x - leftA.x);
				if (dy < EPS && dz < EPS && dx < EPS)
				{
					a->position = original;
					hitEvent.active = true;
					hitEvent.pos.push_back(original);
					hitBlock = b.get();
					selfHitBlock = a.get();
					return 4;
				}
			}


			//Z
			{
				auto frontA = a->GetFrontCenter();
				auto backB = b->GetBackCenter();

				float dx = fabs(frontA.x - backB.x);
				float dy = fabs(frontA.y - backB.y);
				float dz = fabs(frontA.z - backB.z);

				if (dx < EPS && dy < EPS && dz < EPS)
				{
					a->position = original;
					hitEvent.active = true;
					hitEvent.pos.push_back(original);
					hitBlock = b.get();
					selfHitBlock = a.get();
					return 5;
				}
			}
			{
				auto frontB = a->GetBackCenter();
				auto backA = b->GetFrontCenter();
				float dx = fabs(frontB.x - backA.x);
				float dy = fabs(frontB.y - backA.y);
				float dz = fabs(frontB.z - backA.z);
				if (dx < EPS && dy < EPS && dz < EPS)
				{
					a->position = original;
					hitEvent.active = true;
					hitEvent.pos.push_back(original);
					hitBlock = b.get();
					selfHitBlock = a.get();
					return 6;
				}
			}
		}

		//元に戻す
		a->position = original;
	}

	return 0;
}



bool Group::WillHitAnyGroup(float dx, const std::vector<Group*>& allGroups)
{
	for (auto& otherGroup : allGroups)
	{
		if (otherGroup == this) continue;

		for (auto& a : blocks)
		{
			DirectX::XMFLOAT3 minA, maxA;
			a->GetAABB(minA, maxA);

			minA.x += dx;
			maxA.x += dx;

			for (auto& b : otherGroup->blocks)
			{
				DirectX::XMFLOAT3 minB, maxB;
				b->GetAABB(minB, maxB);

				if ((minA.x <= maxB.x && maxA.x >= minB.x) &&
					(minA.y <= maxB.y && maxA.y >= minB.y) &&
					(minA.z <= maxB.z && maxA.z >= minB.z))
				{
					if(minA.x == maxB.x)
					return true;
				/*	else if (maxA.x == minB.x)
					{
						return 2;
					}
					else if (minA.y == maxB.y)
					{
						return 3;
					}
					else if (maxA.y >= minB.y)
					{
						return 4;
					}
					else if (minA.z <= maxB.z)
					{
						return 5;
					}
					else if (maxA.z >= minB.z)
					{
						return 6;
					}*/
				}
			}
		}
	}

	return false;
}

void Group::RequestRotate(RotateAxis axis,float dir)
{
	revolve(axis,dir);
}

void Group::Rotation(float elapsedTime)
{
	if (state != Rotating) return;
	float moveDelta = rotateSpeed * elapsedTime;
	float diff = targetAngle - currentAngle;


	if (diff > DirectX::XM_PI)
		diff -= DirectX::XM_2PI;
	if (diff < -DirectX::XM_PI)
		diff += DirectX::XM_2PI;


	if (fabs(diff) <= moveDelta)
	{
		currentAngle = targetAngle;

		if (currentAngle >= DirectX::XM_2PI)
			currentAngle -= DirectX::XM_2PI;
		if (currentAngle < 0.0f)
			currentAngle += DirectX::XM_2PI;

		state = Idle;
	}
	else
	{
		currentAngle += (diff > 0 ? moveDelta : -moveDelta);
	}

	float angleDelta = currentAngle - prevAngle;

	visualAngle += angleDelta * rotateDir;

	rotatedAmount += fabs(angleDelta);


	// 止める
	if (rotatedAmount >= DirectX::XM_PIDIV2)
	{
		angleDelta -= (rotatedAmount - DirectX::XM_PIDIV2);
		currentAngle = targetAngle;
		visualAngle = DirectX::XM_PIDIV2;
		state = Idle;
		for (auto& b : blocks)
		{
			b->position.x = roundf(b->position.x);
			b->position.y = roundf(b->position.y);
			b->position.z = roundf(b->position.z);
		}
		prevAngle = currentAngle;
		return;
	}



	for (auto& b : blocks)
	{
		// 今の位置
		DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&b->position);
		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&pivot);

		// pivot基準に変換
		DirectX::XMVECTOR local = DirectX::XMVectorSubtract(p, c);

		// 回転
	
		DirectX::XMMATRIX R;

		switch (rotateAxis)
		{
		case AxisX:
			R = DirectX::XMMatrixRotationX(angleDelta * rotateDir);
			break;
		case AxisY:
			R = DirectX::XMMatrixRotationY(angleDelta * rotateDir);
			break;
		case AxisZ:
			R = DirectX::XMMatrixRotationZ(angleDelta * rotateDir);
			break;

		}

		local = DirectX::XMVector3Transform(local, R);

		// 戻す
		DirectX::XMVECTOR result = DirectX::XMVectorAdd(local, c);

		DirectX::XMStoreFloat3(&b->position, result);

		/*switch (rotateAxis)
		{
		case AxisX:
			b->angle.x += angleDelta;
			break;

		case AxisY:
			b->angle.y += angleDelta;
			break;

		case AxisZ:
			b->angle.z += angleDelta;
			break;
		}*/
		b->angle = { 0,0,0 };

		switch (rotateAxis)
		{
		case AxisX:
			b->angle.x = visualAngle;
			break;

		case AxisY:
			b->angle.y = visualAngle;
			break;

		case AxisZ:
			b->angle.z = visualAngle;
			break;
		}
	}

	prevAngle = currentAngle;
}

void Group::DrawDebugGUI()
{
	if (this->GetType() == GroupType::Start)
	{
		ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
		ImGui::SetNextWindowPos(ImVec2(pos.x + 210, pos.y + 10), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Group", nullptr, ImGuiWindowFlags_None))
		{
			//何個つながっているか
			ImGui::InputInt("mergeCount", &mergeCount);
		}
		ImGui::End();
	}
	
	
	for (auto& b : blocks)
	{
		b->DrawDebugGUI();
	}
}


//中心ぶち取り
DirectX::XMFLOAT3 Group::GetCenter()
{
	DirectX::XMFLOAT3 sum = { 0,0,0 };

	for (auto& b : blocks)
	{
		sum.x += b->position.x;
		sum.y += b->position.y;
		sum.z += b->position.z;
	}

	float n = (float)blocks.size();

	return { sum.x / n, sum.y / n, sum.z / n };
}


DirectX::XMFLOAT3 Group::GetStartBlockCenter()
{
	for (auto& b : blocks)
	{
		if (b->GetType() == GroupType::Start)
		{
			return b->position;
		}
	}

	// fallback（念のため）
	return GetCenter();
}

void Group::Explosion(float elapsedTime)
{
	//グループのブロックがそれぞれランダムな方向へ飛んでいく
	for (auto& b : blocks)
	{
		//ランダムな方向ベクトルを生成
		DirectX::XMFLOAT3 dir = {
			(float)(rand() % 200 - 100) / 100.0f,
			(float)(rand() % 200 - 100) / 100.0f,
			(float)(rand() % 200 - 100) / 100.0f
		};
		//正規化
		float length = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
		if (length > 0.0f)
		{
			dir.x /= length;
			dir.y /= length;
			dir.z /= length;
		}
		//速さをかける
		float speed = 5.0f; // 好きな速さに調整
		dir.x *= speed;
		dir.y *= speed;
		dir.z *= speed;
		//時間をかけて移動



		b->position.x +=dir.x; 
		b->position.y +=dir.y; 
		b->position.z +=dir.z; 
	}

}