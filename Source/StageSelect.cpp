#include"StageSelect.h"
#include"System/Graphics.h"
#include"System/Input.h"
#include"SceneManager.h"
#include"SceneLoading.h"
#include"SceneGame.h"

int PosX=0;
void StageSelect::Initialize()
{
	MapBack = new Sprite("Data/Sprite/Title.png");//ステージセレクト背景
	Map1 = new Sprite("Data/Sprite/Title.png");
	Map2 = new Sprite("Data/Sprite/Title.png");
	Map3 = new Sprite("Data/Sprite/Title.png");
	Map4 = new Sprite("Data/Sprite/Title.png");
	Map5 = new Sprite("Data/Sprite/Title.png");

}

void StageSelect::Finalize()
{
	delete MapBack;
	MapBack = nullptr;
	delete Map1;
	Map1 = nullptr;
	delete Map2;
	Map2 = nullptr;
	delete Map3;
	Map3 = nullptr;
	delete Map4;
	Map4 = nullptr;
	delete Map5;
	Map5 = nullptr;
}

void StageSelect::Update(float elapsedTime)
{
	GamePad& gamepad = Input::Instance().GetGamePad();
	//左右でステージセレクト
	if (gamepad.GetButtonDown() & GamePad::BTN_LEFT)
	{
		stage--;
		if (stage <= 0)stage = 5;  //最大ステージ数になる（右端
	}
	if (gamepad.GetButtonDown() & GamePad::BTN_RIGHT)
	{
		stage++;
		if (stage >= 5)stage = 0;  //最初のステージになる（左端
	}

	//ステージを決定する
	if (gamepad.GetButtonDown() & GamePad::BTN_A)//ボタンは自由に
	{
		SceneManager::Instance().ChangeScene(
			new SceneLoading(new SceneGame(stage))
		);
	}
}

void StageSelect::Render()
{
	Graphics& graphics = Graphics::Instance();

	RenderContext rc;
	rc.deviceContext = graphics.GetDeviceContext();
	rc.renderState = graphics.GetRenderState();

	float w = (float)graphics.GetScreenWidth();
	float h = (float)graphics.GetScreenHeight();

	MapBack->Render(rc, PosX, 0, 0, w, h, 0, 1, 1, 1, 1);
	switch (stage)
	{
	case 0://ステージ1
		Map1->Render(rc, 0, 0, 0, w, h, 0, 1, 0, 1, 1);  break;
	case 1://ステージ２
		Map2->Render(rc, 0, 0, 0, w, h, 0, 1, 0, 0, 1);  break;
	case 2://ステージ３
		Map3->Render(rc, 0, 0, 0, w, h, 0, 0, 1, 0, 1);  break;
	case 3://ステージ４
		Map4->Render(rc, 0, 0, 0, w, h, 0, 1, 1, 0, 1);  break;
	case 4://ステージ５
		Map5->Render(rc, 0, 0, 0, w, h, 0, 0, 1, 1, 1);  break;
	}
}

void StageSelect::DrawGUI()
{
}

