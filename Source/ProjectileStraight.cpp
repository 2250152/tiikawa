#include"ProjectileStraight.h"

////コンストラクタ
//ProjectileStraight::ProjectileStraight()
//{
//	model = new Model("Data/Model/SpikeBall/Spikeball.mdl");
//
//	//表示サイズを調整
//	scale.x = scale.y = scale.z = 0.5f;
//}

//引数付きコンストラクタに変更
ProjectileStraight::ProjectileStraight(ProjectileManager* manager):Projectile(manager)
{
	//model = new Model("Data/Model/SpikeBall/Spikeball.mdl");
	model = new Model("Data/Model/Sword/Sword.mdl");

	//表示サイズを調整
	//scale.x = scale.y = scale.z = 0.5f;
	scale.x = scale.y = scale.z = 3.0f;
}

//デストラクタ
ProjectileStraight::~ProjectileStraight()
{
	delete model;
}

void ProjectileStraight::Update(float elapsedTime)
{
	//寿命処理
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0.0f)
	{
		//自分を削除
		Destroy();
	}



	//移動
	float speed = this->speed * elapsedTime;
	position.x += direction.x * speed;
	position.y += direction.y * speed;
	position.z += direction.z * speed;

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform();

}

//描画処理
void ProjectileStraight::Render(const RenderContext& rc,ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

//発射
void ProjectileStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction = direction;//引数でもらったdirectionはplayerの角度（direction）
	this->position = position;//引数でもらったdirectionはplayerの位置（position）
}