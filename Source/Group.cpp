#include "Group.h"
#include "Block.h"
//ここでグループ分けしたやつらを動かそうの会

//試行錯誤で進めとるからまとまってきたらいらんの消すね

void Group::Update(float elapsedTime,const std::vector<Group*>& allGroups)
{
	

	Rotate();

	if (type == GroupType::Start)
	Move(elapsedTime, allGroups);

	if (pendingMerge)
	{
		Merge(pendingMerge);
		pendingMerge = nullptr;
	}

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

	DirectX::XMFLOAT3 speed = {0,1,0};
	DirectX::XMFLOAT3 move;
	//ここでplayerとか使って変えよう




	move.x = speed.x * elapsedTime;
	move.y = speed.y * elapsedTime;
	move.z = speed.z * elapsedTime;

	//まず動く
	for (auto& b : blocks)
	{
		b->position.x += move.x;
		b->position.y += move.y;
		b->position.z += move.z;
	}

	//その後当たり判定
	for (auto& g : allGroups)
	{
		if (g == this) continue;

		int hitDir = WillHit(g, { 0,0,0 }); 

		if (hitDir != 0)
		{
			pendingMerge = g;
			state = Idle;
			//return;
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

void Group::revolve(RotateAxis axis)
{
	if (state != Idle)
		return;

	rotateAxis = axis;

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
	const float EPS = 0.01f;

	for (auto& a : blocks)
	{
		//先を見る
		DirectX::XMFLOAT3 original = a->position;
		a->position.x += move.x;
		a->position.y += move.y;
		a->position.z += move.z;

		for (auto& b : otherGroup->blocks)
		{
			//Y
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
					hitEvent.pos = original;
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
					hitEvent.pos = original;
					return 1;
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
					hitEvent.pos = original;
					return 3;
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

	visualAngle += angleDelta;

	rotatedAmount += fabs(angleDelta);


	// 止める
	if (rotatedAmount >= DirectX::XM_PIDIV2)
	{
		angleDelta -= (rotatedAmount - DirectX::XM_PIDIV2);
		currentAngle = targetAngle;
		visualAngle = DirectX::XM_PIDIV2;
		state = Idle;
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
			R = DirectX::XMMatrixRotationX(angleDelta);
			break;
		case AxisY:
			R = DirectX::XMMatrixRotationY(angleDelta);
			break;
		case AxisZ:
			R = DirectX::XMMatrixRotationZ(angleDelta);
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