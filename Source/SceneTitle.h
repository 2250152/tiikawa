#pragma once

#include "System/Sprite.h"
#include"Scene.h"



//タイトルシーン
class SceneTitle :public Scene
{
public:
	SceneTitle(){}
	~SceneTitle()override{}

	//初期化
	void Initialize()override;

	//終了化
	void Finalize()override;

	//更新処理
	void Update(float elapsedTime)override;
	
	//描画処理
	void Render()override;

	///GUI描画
	void DrawGUI()override;

private:
	Sprite* sprite = nullptr;

	Sprite* font = nullptr;
	//void DrawText(Sprite* font, const RenderContext& rc,
	//	const char* text,
	//	float x, float y,

	//	float charW, float charH)
	//{
	//	int len = strlen(text);

	//	for (int i = 0; i < len; i++)
	//	{
	//		char c = text[i];

	//		// 'A'を基準にインデックス化
	//		int index = c - 'A';

	//		// フォント画像上の位置
	//		float sx = (index % 16) * charW;
	//		float sy = (index / 16) * charH;

	//		font->Render(rc,
	//			x + i * charW, y, 0,
	//			charW, charH,
	//			sx, sy, charW, charH,
	//			0,
	//			1, 1, 1, 1);
	//	}
	//}
};