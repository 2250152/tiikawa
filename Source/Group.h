#pragma once
//#include "BlockManager.h"
#include <vector>
#include <set>
#include "System/ModelRenderer.h"
#include "GroupType.h"
#include "StageSelect.h"
#include <SceneGame.h>
#include"SceneGame.h"

class Block;

enum State
{
	Idle,   // 止まってる
	Moving, // 動いてる
	Rotating, //回転している
};
enum RotateAxis
{
	AxisX,
	AxisY,
	AxisZ
};
class Group
{
private:
	
public:
	Group(GroupType type) : type(type) { mergeCount = 1; }
	~Group() {}

	GroupType GetType() const { return type; }

	//void Initialize(int stageNo);

	void Update(float elpsedTime, const std::vector<Group*>& allGroups);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void ExceptHitting(float elapsedTime, const std::vector<Group*>& allGroups);

	void Move(float elapsedTime, const std::vector<Group*>& allGroups);

	void Rotate();

	//ヒットするまでの距離予測
	void ExpectUntilDistanceHit(const std::vector<Group*>& allGroups, DirectX::XMFLOAT3 move);
	//レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const DirectX::XMFLOAT4X4& worldTransform,
		const Model* model,
		DirectX::XMFLOAT3& hitPosition,
		DirectX::XMFLOAT3& hitNormal);

	void AddBlock(std::unique_ptr<Block> block);

	std::vector<std::unique_ptr<Block>>& GetBlocks()
	{
		return blocks;
	}


	const std::vector<std::unique_ptr<Block>>& GetBlocks() const
	{
		return blocks;
	}


	template<typename T, typename... Args>
	void CreateBlock(Args&&... args)
	{
		auto block = std::make_unique<T>(std::forward<Args>(args)...);
		block->SetGroup(this);
		blocks.push_back(std::move(block));
	}

	void Merge(Group* other);

	void Go();

	void revolve(RotateAxis axis,float dir);

	int WillHit(Group* otherGroup, DirectX::XMFLOAT3 dx);

	void Stop() { isMoving = false; }
	void Start() { isMoving = true; }

	bool WillHitAnyGroup(float dx, const std::vector<Group*>& allGroups);

	void Rotation(float elapsedTime);

	void DrawDebugGUI();

	//中心ぶち取り
	DirectX::XMFLOAT3 GetCenter();
	
	DirectX::XMFLOAT3 GetStartBlockCenter();

	bool GetHitEvent()  { return hitEvent.active; }
	const std::vector<DirectX::XMFLOAT3>& GetHitPositions() { return hitEvent.pos; };

	void ClearHitEvent()
	{
		hitEvent.active = false;
		hitEvent.pos.clear();
	}

	void RequestRotate(RotateAxis axis,float dir);

	int GetMergeCount() const { return mergeCount; }

	void Explosion();

	bool GetExplosion() const { return explosionf; }

	void ExplosionUpdate(float elapsedTime);

	virtual void OnHitGoal(Group* other);

	bool ClearFlag = false;

	

private:
	std::vector<std::unique_ptr<Block>> blocks;
	float willCollideDist = 30; //衝突するであろうブロックとの距離を管理
	Block* willCollideBlockAddress = nullptr; //衝突するであろうブロックのアドレスを管理  //後で配列に

	GroupType type;

	DirectX::XMFLOAT3 position;

	DirectX::XMFLOAT3 angle;

	bool isMoving = false;
	bool isBlocked = false;
	State state = Idle;

	std::vector<Group*>  pendingMerge;

	const float PIDIV180 = 0.017452f;

	int count = 0;

	float   targetAngle;
	
	float rotateSpeed=1.0f;

	float currentAngle = 0.0f;

	float prevAngle = 0.0f;

	DirectX::XMFLOAT3 pivot = { 0,0,0 };

	float rotatedAmount = 0.0f;

	


	struct HitEvent
	{
		bool active = false;
		std::vector<DirectX::XMFLOAT3> pos;
	};

	HitEvent hitEvent;

	RotateAxis rotateAxis=AxisX;

	//演出の回転用
	float visualAngle = 0.0f;

	float rotateDir = 1.0f;

	int mergeCount = 0;

	const float EPS = 0.01f;

	//当たったブロック
	Block* selfHitBlock = nullptr;

	//GroupType::Startで当たったブロック
	Block* hitBlock = nullptr;

	bool explosionf = 0;
};