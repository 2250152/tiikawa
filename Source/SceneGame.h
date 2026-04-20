#pragma once
#include "Stage.h"
#include "Player.h"
#include"CameraController.h"

#include"Scene.h"

// ゲームシーン
class SceneGame:public Scene
{
public:
	SceneGame() {};
	/*~SceneGame() {};*/
	~SceneGame()override {};

	// 初期化        ※ゲームは　初期化,終了化,更新処理,描画処理　が基本
	/*void Initialize();*/
	void Initialize()override;

	// 終了化        ※←Unityとかは自動でやってくれるけどDirectXtexは自分でやらなアカン
	/*void Finalize();*/
	void Finalize()override;

	// 更新処理
	/*void Update(float elapsedTime);*/
	void Update(float elapsedTime)override;

	// 描画処理
	/*void Render();*/
	void Render()override;

	// GUI描画
	/*void DrawGUI();*/
	void DrawGUI()override;

private:
	Stage* stage = nullptr;
	/*Player* player = nullptr;*/
	CameraController* cameraController = nullptr;
};
