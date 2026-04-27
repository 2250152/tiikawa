#include "System/Graphics.h"
#include "SceneGame.h"
#include"Camera.h"
#include"EffectManager.h"
#include "Block.h"
#include "BlockManager.h"
#include "Group.h"
#include "Block_Normal.h"
#include "Block_Start.h"

// 初期化
void SceneGame::Initialize()
{
	
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




	//EnemySlime* slime = new EnemySlime();             敵を増やしたいから消して下のfor文にした。
	//slime->SetPosition(DirectX::XMFLOAT3(0,0,5));
	//enemyManager.Register(slime);

	

	// グループ作成
	std::unique_ptr<Group> groupStart = std::make_unique<Group>(GroupType::Start);
	std::unique_ptr<Group> groupNormal = std::make_unique<Group>(GroupType::Normal);


	
	// ブロック生成兼グループに追加
	groupStart->CreateBlock<BlockStart>(GroupType::Start);
	groupNormal->CreateBlock<BlockNormal>(GroupType::Normal);

	// マネージャーに登録
	BlockManager::Instance().AddGroup(std::move(groupStart));
	BlockManager::Instance().AddGroup(std::move(groupNormal));



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



	
	//if (player != nullptr)
	//{
	//	delete player;       //←本来ならこれだけで全てdeleteされるだと
	//	player = nullptr;    //←これは万が一カスが残ってたらまずいから保険でnullを
	//}
	Player::Instance().Finalize();

	
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

	

	//プレイヤー更新処理
	/*player->Update(elapsedTime);*/
	Player::Instance().Update(elapsedTime);


	
	//エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	BlockManager::Instance().Update(elapsedTime);
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
		

		//プレーヤー描画
		/*player->Render(rc, modelRenderer);*/
		Player::Instance().Render(rc, modelRenderer);

		
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

	}
}

// GUI描画
void SceneGame::DrawGUI()
{
	/*player->DrawDebugGUI();*/
	Player::Instance().DrawDebugGUI();
}