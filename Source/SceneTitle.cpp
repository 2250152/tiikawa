#include"System/Graphics.h"
#include"SceneTitle.h"
#include"StageSelect.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include"SceneLoading.h"


//初期化
void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/Title.png");
	font = new Sprite("Data/Sprite/S.png");
}

//終了化
void SceneTitle::Finalize()
{
	//スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;		
		delete font;
		sprite = nullptr;
		font = nullptr;
	}
}

//更新処理
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// 何かボタンを押したらゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_X |
		GamePad::BTN_Y;

	if (gamePad.GetButtonDown() & anyButton)
	{
		/*SceneManager::Instance().ChangeScene(new SceneGame);*/
		/*SceneManager::Instance().ChangeScene(new SceneLoading);*/
		SceneManager::Instance().ChangeScene(new StageSelect);
		
	}

}

//描画処理
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();



	//2Dスプライト描画
	{
		//タイトル描画
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());

		sprite->Render(rc,
			0, 0, 0, screenWidth, screenHeight,
			0,
			1, 1, 0, 1);
		float a= 1.0f;
		font->Render(rc,
			570, 500, 0, 256, 256,
			0,
			1, 1, 1, a);
		
	/*	DrawText(font, rc, "START", 640, 600, 5, 5);
	RECT rect;
	SetRect(&rect, 10, 10, 200, 100);

	DrawTextW(font,rc, "Hello, World!",  10, 10, 32,32);*/

	
	}
	

}

//GUI描画
void SceneTitle::DrawGUI()
{


}

