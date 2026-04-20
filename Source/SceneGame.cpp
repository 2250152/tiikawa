#include "System/Graphics.h"
#include "SceneGame.h"
#include"Camera.h"
#include "EnemyManager.h"
#include"EnemySlime.h"
#include"EffectManager.h"


// 初期化
void SceneGame::Initialize()
{
	//ステージ初期化
	stage = new Stage();
	//プレーヤー初期化
	/*player = new Player();*/
	Player::Instance().Initialize();

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

	
	cameraController = new CameraController();

	//エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();
	for (int i = 0; i < 2; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		slime->SetTerritory(slime->GetPosition(), 10.0f);
		enemyManager.Register(slime);
	}


	//EnemySlime* slime = new EnemySlime();             敵を増やしたいから消して下のfor文にした。
	//slime->SetPosition(DirectX::XMFLOAT3(0,0,5));
	//enemyManager.Register(slime);
	for (int i = 0;i < 2;++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		enemyManager.Register(slime);
	}
}

// 終了化
void SceneGame::Finalize()
{
	//カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}



	//ステージ終了化
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}

	//if (player != nullptr)
	//{
	//	delete player;       //←本来ならこれだけで全てdeleteされるだと
	//	player = nullptr;    //←これは万が一カスが残ってたらまずいから保険でnullを
	//}
	Player::Instance().Finalize();

	//エネミー終了化
	EnemyManager::Instance().Clear();

}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	//カメラコントローラー更新処理
	/*DirectX::XMFLOAT3 target = player->GetPosition();*/
	DirectX::XMFLOAT3 target = Player::Instance().GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	//ステージ更新処理
	stage->Update(elapsedTime);

	//プレイヤー更新処理
	/*player->Update(elapsedTime);*/
	Player::Instance().Update(elapsedTime);


	//エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	//エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
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
		//ステージ描画
		stage->Render(rc, modelRenderer);

		//プレーヤー描画
		/*player->Render(rc, modelRenderer);*/
		Player::Instance().Render(rc, modelRenderer);

		//エネミーの描画
		EnemyManager::Instance().Render(rc, modelRenderer);

		//エフェクト描画
		EffectManager::Instance().Render(rc.view, rc.projection);

	}

	// 3Dデバッグ描画
	{
		//プレイヤーデバッグプリミティブ描画
		/*player->RenderDebugPrimitive(rc, shapeRenderer);*/ //←プレイヤーの下に黒い線の球体のやつ
		Player::Instance().RenderDebugPrimitive(rc, shapeRenderer);

		//エネミープリミティブ描画
		EnemyManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);

	}

	// 2Dスプライト描画
	{

	}
}

// GUI描画
void SceneGame::DrawGUI()
{
	/*player->DrawDebugGUI();*/
	Player::Instance().DrawDebugGUI();
}
