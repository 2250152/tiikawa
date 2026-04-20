#pragma once

#include<vector>
#include<set>

#include"Enemy.h"

//エネミーマネージャー
class EnemyManager
{
private:
	EnemyManager(){}
	~EnemyManager(){}

public:
	//唯一のインスタンス取得
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//エネミー登録
	void Register(Enemy* enemy);

	//エネミー全削除
	void Clear();

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	//エネミー数取得
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	//エネミー取得
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	void Remove(Enemy* enemy);


private:
	std::vector<Enemy*>   enemies;

	/*
	    std::vector	 C++ 標準ライブラリにある「動的配列クラス」
		<Enemy*>	 テンプレート引数。「このベクターの中に何を入れるか」を指定している
		enemies	     変数名。実際に使うベクターの名前
	*/

	//エネミー同士の衝突処理
	void CollisionEnemyVsEnemies();

	std::set<Enemy*> removes;

};