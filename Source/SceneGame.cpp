#include "System/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EffectManager.h"
#include "Block.h"
#include "BlockManager.h"
#include "Group.h"
#include "Block_Normal.h"
#include "Block_Start.h"
#include"SkyBox.h"
#include"SceneManager.h"
#include"SceneLoading.h"
#include"StageSelect.h"
#include "System/Sprite.h"




SceneGame::SceneGame(int stageNo)
{
	m_stageNo = stageNo;
}

// 初期化
void SceneGame::Initialize()
{
	
	//プレーヤー初期化
	/*player = new Player();*/
	Player::Instance().Initialize();
	
	//メニュー
	menyu = new Sprite("Data/Sprite/black.png");
	choise = new Sprite("Data/Sprite/choise.png");
	flame = new Sprite("Data/Sprite/flame.png");

	//カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetlookAt(
		DirectX::XMFLOAT3(0, 10, -10),//視点
		DirectX::XMFLOAT3(0, 0, 0),   //注視点
		DirectX::XMFLOAT3(0, 1, 0)    //上方向
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),  //視野角
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),  //画面アスペクト比
		0.1f,  //クリップ距離(近)
		1000.0f  //クリップ距離(遠)
	);

	CameraController::Instance().Initialize();
	

	//背景
	SkyBox::Instance().Initialize();

	//ここでstage呼ぶわよ 引数の数値で呼ぶstage変えてねぃ
	
	stage.Load(m_stageNo);
	//stage.Load(1);

}

// 終了化
void SceneGame::Finalize()
{
	//カメラコントローラー終了化
	/*if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}*/

	if (menyu != nullptr)
	{
		delete menyu;
		delete choise;
		delete flame;
		menyu = nullptr;
		choise = nullptr;
		flame = nullptr;
		menyuON = false;
	}

	
	//if (player != nullptr)
	//{
	//	delete player;       //←本来ならこれだけで全てdeleteされるだと
	//	player = nullptr;    //←これは万が一カスが残ってたらまずいから保険でnullを
	//}
	Player::Instance().Finalize();

	//背景
	SkyBox::Instance().Finalize();
	
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	//カメラコントローラー更新処理
	/*DirectX::XMFLOAT3 target = player->GetPosition();*/
	//DirectX::XMFLOAT3 target = Player::Instance().GetPosition();
	DirectX::XMFLOAT3 target = BlockManager::Instance().GetGroups()[0]->GetBlocks()[0]->GetPosition();
	target.y += 0.5f;
	CameraController::Instance().SetTarget(target);
	CameraController::Instance().Update(elapsedTime);

	

	//プレイヤー更新処理
	/*player->Update(elapsedTime);*/

	//背景
	SkyBox::Instance().Update(elapsedTime);

	
	//エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	BlockManager::Instance().Update(elapsedTime);

	Player::Instance().Update(elapsedTime);

	//ステージセレクトに戻る
	GamePad& gamepad = Input::Instance().GetGamePad();
	
	
	

	//メニューを出す
	if (gamepad.GetButtonDown() & (GamePad::BTN_LEFT))
	{
		menyuON = true;
	}
	if (menyuON)
	{

		if (gamepad.GetButtonDown() & (GamePad::BTN_UP))  
		{
			posY = 360;
			BACK = false;
		}

		if (gamepad.GetButtonDown() & (GamePad::BTN_DOWN))
		{
			posY = 544;
			BACK = true;
		}

		if (gamepad.GetButtonDown() & (GamePad::BTN_RIGHT))
		{
			menyuON = false;
		}
		if (BACK == true && gamepad.GetButtonDown() & (GamePad::BTN_A))  //ボタンは自由に、 ステージセレクトへ
		{
			SceneManager::Instance().ChangeScene(
				new SceneLoading(new StageSelect));
		}

		if (BACK == false && gamepad.GetButtonDown() & (GamePad::BTN_A))  //リトライ
		{
			//Clear = true;
			this->Finalize();
			this->Initialize();
		}

		


	}
	//クリア時ステージ切り替え
	for (auto& g : BlockManager::Instance().GetGroups())
	{
		if (g->isClear())
			SceneManager::Instance().ChangeScene(
				new SceneLoading(new SceneGame(m_stageNo + 1))
			);
		//this->Initialize();
		return;
	}

}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	// 描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f };	// ライト方向（下方向）
	rc.renderState = graphics.GetRenderState();

	float w = (float)graphics.GetScreenWidth();
	float h = (float)graphics.GetScreenHeight();

	//カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();


	



	/*　　　　　　　↑
	　　ビュー行列とプロジェクション行列はもう必要ない。代わりに上の //カメラパラメータ設定 を使う
	     　　　　   ↓

	// ビュー行列
	{
		
		DirectX::XMFLOAT3 eye = { 0, 10, -10 };	// カメラの視点（位置）
		DirectX::XMFLOAT3 focus = { 0, 0, 0 };	// カメラの注視点（ターゲット）
		DirectX::XMFLOAT3 up = { 0, 1, 0 };		// カメラの上方向

		DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
		DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
		DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
		DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
		DirectX::XMStoreFloat4x4(&rc.view, View);
	}
	// プロジェクション行列
	{
		float fovY = DirectX::XMConvertToRadians(45);	// 視野角
		float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();	// 画面縦横比率
		float nearZ = 0.1f;	// カメラが映し出すの最近距離
		float farZ = 1000.0f;	// カメラが映し出すの最遠距離
		DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
		DirectX::XMStoreFloat4x4(&rc.projection, Projection);
	}

	*/


	// 3Dモデル描画
	{
		

		//プレーヤー描画
		/*player->Render(rc, modelRenderer);*/
		Player::Instance().Render(rc, modelRenderer);

		//背景
		SkyBox::Instance().Render(rc, modelRenderer);
		
		//エフェクト描画
		EffectManager::Instance().Render(rc.view, rc.projection);
		BlockManager::Instance().Render(rc, modelRenderer);
	}

	// 3Dデバッグ描画
	{
		//プレイヤーデバッグプリミティブ描画
		/*player->RenderDebugPrimitive(rc, shapeRenderer);*/ //←プレイヤーの下に黒い線の球体のやつ
		Player::Instance().RenderDebugPrimitive(rc, shapeRenderer);
		
	
	}

	// 2Dスプライト描画
	{
		if (menyuON) {
			menyu->Render(rc, 780, 360, 0, 400, 400, 0, 0, 1, 1, 0.5f);
			choise->Render(rc, 800, 360, 0, 380, 430, 0, 1, 1, 1, 1);
			flame->Render(rc, 790, posY, 0, 400, 400, 0, 1, 1, 1, 1);
		}
	}
}

// GUI描画
void SceneGame::DrawGUI()
{
	/*player->DrawDebugGUI();*/
	Player::Instance().DrawDebugGUI();
	BlockManager::Instance().DrawDebugGUI();
	CameraController::Instance().DrawDebugGUI();
}
