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
	Sprite* Map = nullptr;
	int stage=0;  //選択してるステージ
};