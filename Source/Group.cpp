#include <DirectXCollision.h>
#include "Group.h"
#include "Block.h"
#include "Player.h"
#include<imgui.h>
#include <Block_Goal.h>
#include"SceneGame.h"
#include "StageSelect.h"

//ここでグループ分けしたやつらを動かそうの会

//試行錯誤で進めとるからまとまってきたらいらんの消すね

//void Group::Initialize(int stageNo)
//{
//	m_stageNo = stageNo;
//	stage.Load(m_stageNo);
//}

void Group::Update(float elapsedTime,const std::vector<Group*>& allGroups)
{
	//爆散の時はそれだけ動かす
	if (GetExplosion())
	{
		ExplosionUpdate(elapsedTime);

		for (auto& block : blocks)
		{
			block->Update(elapsedTime);
		}

		return;
	}
	
	Rotate();

	if (type == GroupType::Start)
	Move(elapsedTime, allGroups);

	ExceptHitting(elapsedTime, allGroups);

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
						if (g->GetType() == GroupType::Goal)
						{
							g->OnHitGoal(this);
							//クリア
							clearFlag = true;
						}
						

					}
					//数が足りていなかったら
					else
					{
						//爆散
						Explosion();
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
						b->position.x = roundf(b->position.x);
						b->position.y = roundf(b->position.y);
						b->position.z = roundf(b->position.z);
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
						b->position.x = roundf(b->position.x);
						b->position.y = roundf(b->position.y);
						b->position.z = roundf(b->position.z);
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
						b->position.x = roundf(b->position.x);
						b->position.y = roundf(b->position.y);
						b->position.z = roundf(b->position.z);
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
						b->position.x = roundf(b->position.x);
						b->position.y = roundf(b->position.y);
						b->position.z = roundf(b->position.z);
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
						b->position.x = roundf(b->position.x);
						b->position.y = roundf(b->position.y);
						b->position.z = roundf(b->position.z);
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
						b->position.x = roundf(b->position.x);
						b->position.y = roundf(b->position.y);
						b->position.z = roundf(b->position.z);
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
	/*for (auto& b : blocks)
	{
		b->position.x =
			roundf(b->position.x * 1000.0f) / 1000.0f;

		b->position.y =
			roundf(b->position.y * 1000.0f) / 1000.0f;

		b->position.z =
			roundf(b->position.z * 1000.0f) / 1000.0f;
	}*/

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

//ブロックが衝突する予測が立った場合の処理(予測もこの関数内で行う)
void Group::ExceptHitting(float elapsedTime, const std::vector<Group*>& allGroups)
{
	if (state != Idle) return;


	Player& player = Player::Instance();


	DirectX::XMFLOAT3 v = player.GetDownGravity();

	DirectX::XMFLOAT3 speed = { v.x,v.y,v.z };
	DirectX::XMFLOAT3 move;

	move.x = speed.x * elapsedTime;
	move.y = speed.y * elapsedTime;
	move.z = speed.z * elapsedTime;

	//ヒット予測
	ExpectUntilDistanceHit(allGroups, /*move*/{ 0, 1, 0 });

	if (willCollideBlockAddress != nullptr)
	{
		willCollideBlockAddress->SetWillHitPositions(willCollideBlockAddress->GetPosition());
	}
}

//ヒットするまでの距離予測(どのブロックと衝突するかもこの関数で判定)
void Group::ExpectUntilDistanceHit(const std::vector<Group*>& allGroups, DirectX::XMFLOAT3 move) // moveは単位ベクトル
{
	const float threshold = 0.1f; //閾値

	//レイキャストを飛ばしてブロックのヒット予想
	for (auto& a : blocks)
	{
		if (a->GetGroup()->GetType() != GroupType::Start)
			continue;
		for (auto& b : allGroups)
		{
			for (auto& c : b->GetBlocks())
			{
				//進行方向上に比較対象がなければスキップ
				// X方向への移動時
				if (move.y == 0 && move.z == 0)
				{
					if ((a->GetPosition().y + threshold < c->GetPosition().y && a->GetPosition().y - threshold > c->GetPosition().y) ||
						(a->GetPosition().z + threshold < c->GetPosition().z && a->GetPosition().z - threshold > c->GetPosition().z)) //対象が進行方向になければ当たらない
						continue;
					if (move.x > 0 && a->GetPosition().x > c->GetPosition().x) // move.xが＋且つ移動前位置のほうが大きい値ならば当たらない
						continue;
					if (move.x < 0 && a->GetPosition().x < c->GetPosition().x) //対象が進行方向になければ当たらない
						continue;
				}
				// Y方向への移動時
				if (move.x == 0 && move.z == 0)
				{
					if ((a->GetPosition().x + threshold < c->GetPosition().x && a->GetPosition().x - threshold > c->GetPosition().x) ||
						(a->GetPosition().z + threshold < c->GetPosition().z && a->GetPosition().z - threshold > c->GetPosition().z)) //対象が進行方向になければ当たらない
						continue;
					if (move.y > 0 && a->GetPosition().y > c->GetPosition().y) //対象が進行方向になければ当たらない
						continue;
					if (move.y < 0 && a->GetPosition().y < c->GetPosition().y) //対象が進行方向になければ当たらない
						continue;
				}
				// Z方向への移動時
				if (move.x == 0 && move.y == 0)
				{
					if ((a->GetPosition().x + threshold < c->GetPosition().x && a->GetPosition().x - threshold > c->GetPosition().x) ||
						(a->GetPosition().y + threshold < c->GetPosition().y && a->GetPosition().y - threshold > c->GetPosition().y)) //対象が進行方向になければ当たらない
						continue;
					if (move.z > 0 && a->GetPosition().z > c->GetPosition().z) //対象が進行方向になければ当たらない
						continue;
					if (move.z < 0 && a->GetPosition().z < c->GetPosition().z) //対象が進行方向になければ当たらない
						continue;
				}

				//レイキャスト
				DirectX::XMFLOAT3 s = { a->GetPosition() };
				DirectX::XMFLOAT3 e = { c->GetPosition() };
				DirectX::XMFLOAT3 hitPosition, hitNormal;
				if (RayCast(s, e, c->Gettranceform(), c->GetModel(), hitPosition, hitNormal))
				{
					DirectX::XMVECTOR lengthVec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&s), DirectX::XMLoadFloat3(&hitPosition));
					if (DirectX::XMVectorGetX(DirectX::XMVector3Length(lengthVec)) < willCollideDist)
					{
						willCollideDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(lengthVec));
						willCollideBlockAddress = c.get();
					}									
				}
			}
		}
	}
	return;
}

bool Group::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4X4& worldTransform, const Model* model, DirectX::XMFLOAT3& hitPosition, DirectX::XMFLOAT3& hitNormal)
{
	bool hit = false;

	// 始点と終点からレイのベクトルと長さを求める
	DirectX::XMVECTOR WorldRayStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldRayEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldRayEnd, WorldRayStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	float nearestDist = DirectX::XMVectorGetX(WorldRayLength);
	if (nearestDist <= 0.0f) return false;

	DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);

	// モデル内の全てのメッシュと交差判定を行う
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// メッシュのワールド行列を求める
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
		DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
		DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(GlobalTransform, ParentWorldTransform);

		// レイをメッシュのローカル空間に変換する
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
		DirectX::XMVECTOR LocalRayStart = DirectX::XMVector3Transform(WorldRayStart, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayEnd = DirectX::XMVector3Transform(WorldRayEnd, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayVec = DirectX::XMVectorSubtract(LocalRayEnd, LocalRayStart);
		DirectX::XMVECTOR LocalRayDirection = DirectX::XMVector3Normalize(LocalRayVec);
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(LocalRayVec));

		// メッシュ内の全ての三角形と交差判定を行う
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			// 三角形の頂点座標を取得
			const ModelResource::Vertex& a = mesh.vertices.at(mesh.indices.at(i + 0));
			const ModelResource::Vertex& b = mesh.vertices.at(mesh.indices.at(i + 1));
			const ModelResource::Vertex& c = mesh.vertices.at(mesh.indices.at(i + 2));

			DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
			DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

			// 三角形の交差判定
			if (DirectX::TriangleTests::Intersects(LocalRayStart, LocalRayDirection, A, B, C, distance))
			{
				// メッシュのローカル空間での交点を求める
				DirectX::XMVECTOR LocalHitVec = DirectX::XMVectorScale(LocalRayDirection, distance);
				DirectX::XMVECTOR LocalHitPosition = DirectX::XMVectorAdd(LocalRayStart, LocalHitVec);

				// メッシュのローカル空間での交点をワールド空間に変換する
				DirectX::XMVECTOR WorldHitPosition = DirectX::XMVector3Transform(LocalHitPosition, WorldTransform);

				// ワールド空間でのレイの始点から交点までの距離を求める
				DirectX::XMVECTOR WorldHitVec = DirectX::XMVectorSubtract(WorldHitPosition, WorldRayStart);
				DirectX::XMVECTOR WorldHitDist = DirectX::XMVector3Length(WorldHitVec);
				float worldHitDist = DirectX::XMVectorGetX(WorldHitDist);

				// 交点までの距離が今までに計算した交点より近い場合は交差したと判定する
				if (worldHitDist <= nearestDist)
				{
					// メッシュのローカル空間での三角形の法線ベクトルを算出
					DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
					DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
					DirectX::XMVECTOR LocalHitNormal = DirectX::XMVector3Cross(AB, BC);

					// メッシュのローカル空間での法線ベクトルをワールド空間に変換する
					DirectX::XMVECTOR WorldHitNormal = DirectX::XMVector3TransformNormal(LocalHitNormal, WorldTransform);

					// 三角形の裏表判定（内積の結果がマイナスならば表向き）
					DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(LocalRayDirection, LocalHitNormal);
					float dot = DirectX::XMVectorGetX(Dot);
					if (dot < 0)
					{
						// 交差したのでヒット情報を格納
						DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(WorldHitNormal));
						DirectX::XMStoreFloat3(&hitPosition, WorldHitPosition);

						// 交点までの距離を更新する
						nearestDist = worldHitDist;
						hit = true;
					}
				}
			}
		}
	}
	return hit;
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

void Group::Explosion()
{
	explosionf = true;

	for (auto& b : blocks)
	{
		DirectX::XMFLOAT3 dir =
		{
			(float)(rand() % 200 - 100) / 100.0f,
			(float)(rand() % 200 - 100) / 100.0f,
			(float)(rand() % 200 - 100) / 100.0f
		};

		float len = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

		if (len > 0.0f)
		{
			dir.x /= len;
			dir.y /= len;
			dir.z /= len;
		}

		float speed = 5.0f;

		b->velocity.x = dir.x * speed;
		b->velocity.y = dir.y * speed;
		b->velocity.z = dir.z * speed;
	}
}

void Group::ExplosionUpdate(float elapsedTime)
{
	for (auto& b : blocks)
	{
		{
			b->position.x += b->velocity.x * elapsedTime;
			b->position.y += b->velocity.y * elapsedTime;
			b->position.z += b->velocity.z * elapsedTime;
		}
	
	}
}


void Group::OnHitGoal(Group* other)
{
	if (other->GetMergeCount() > 10)
	{
		for (auto& b : blocks)
		{
			if (auto goal = dynamic_cast<BlockGoal*>(b.get()))
			{
				goal->BreakBarrier();
			}
		}
	}
}


