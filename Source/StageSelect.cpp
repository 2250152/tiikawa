#include"StageSelect.h"
#include"System/Graphics.h"
#include"System/Input.h"
#include"SceneManager.h"
#include"SceneLoading.h"
#include"SceneGame.h"

void StageSelect::Initialize()
{
	Map = new Sprite("Data/Sprite/Title.png");//ステージセレクト用の一枚絵
}

void StageSelect::Finalize()
{
	delete Map;
	Map = nullptr;
}

void StageSelect::Update(float elapsedTime)
{
	GamePad& gamepad = Input::Instance().GetGamePad();
	//左右でステージセレクト
	if (gamepad.GetButtonDown() & GamePad::BTN_LEFT)
	{
		stage--;
		if (stage < 0)stage = 5;  //最大ステージ数になる（右端
	}
	if (gamepad.GetButtonDown() & GamePad::BTN_RIGHT)
	{
		stage++;
		if (stage > 5)stage = 0;
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

	Map->Render(rc, 0, 0, 0, w, h, 0, 1, 1, 1, 1);

}

void StageSelect::DrawGUI()
{
}

