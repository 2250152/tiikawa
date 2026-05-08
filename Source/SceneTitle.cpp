#include"System/Graphics.h"
#include"SceneTitle.h"
#include"StageSelect.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include"SceneLoading.h"

bool Back = false;
bool feedOut = false;
float feedColor = 0.0f;
float feedColor2 = 1.0f;
//初期化
void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/Title.png"); //タイトル背景
	font = new Sprite("Data/Sprite/S.png");       //タイトル
	feed = new Sprite("Data/Sprite/black.png");  //フェードアウト
}

//終了化
void SceneTitle::Finalize()
{
	//スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;		
		delete font;
		delete feed;
		sprite = nullptr;
		font = nullptr;
		feed = nullptr;
		feedColor = 0;
		feedColor2 = 1;
	}
}

//更新処理
void SceneTitle::Update(float elapsedTime)
{
	if (Back)
	{
		feedColor2 -= 0.005f;
		if (feedColor2 <= 0) {
			feedColor2 = 0;
		}
	}
	GamePad& gamePad = Input::Instance().GetGamePad();

	// 何かボタンを押したらゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_X |
		GamePad::BTN_Y;
	
	if (gamePad.GetButtonDown() & anyButton)
	{
		feedOut = true;
	}
	if (feedOut)
	{
		feedColor += 0.005f;
	}
	if (feedColor >= 1.0f) {
		SceneManager::Instance().ChangeScene(new StageSelect);
		feedOut = false;
		Back = true;
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
			1, 1, 1, 1);
		
		font->Render(rc,
			570, 500, 0, 256, 256,
			0,
			1, 1, 1, 1);
		

		feed->Render(rc,
			0, 0, 0, screenWidth, screenHeight,
			0,
			1, 1, 0, feedColor);
		
		if (Back) {
			feed->Render(rc,
				0, 0, 0, screenWidth, screenHeight,
				0,
				1, 1, 0, feedColor2);
		}
	}
}

//GUI描画
void SceneTitle::DrawGUI()
{


}

