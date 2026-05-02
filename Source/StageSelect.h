#pragma once
#include "Scene.h"

class Sprite;

class StageSelect : public Scene
{
public:
	void Initialize()override;
	void Finalize()override;
	void Update(float elapsedTime)override;
	void Render()override;
	void DrawGUI()override;

private:
	Sprite* MapBack = nullptr;
	Sprite* Map1 = nullptr;
	Sprite* Map2 = nullptr;
	Sprite* Map3 = nullptr;
	Sprite* Map4 = nullptr;
	Sprite* Map5 = nullptr;
	int stage=0;  //選択してるステージ
};