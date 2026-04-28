#include"SkyBox.h"
#include"Camera.h"

SkyBox::SkyBox()
{
}

SkyBox::~SkyBox()
{
}

void SkyBox::Initialize()
{
	model = new Model("Data/Model/SkyBox/skybox.mdl");

	position = Camera::Instance().GetEye();

	scale.x = scale.y = scale.z = 5.0f;
}

void SkyBox::Finalize()
{
	delete model;
}

void SkyBox::Update(float elapsedTime)
{
	position = Camera::Instance().GetEye();

	UpdateTransform();
	//model->UpdateTransform();
}

void SkyBox::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    ID3D11DeviceContext* context = rc.deviceContext;

    // 現在のラスタライザーステートを保存
    ID3D11RasterizerState* prevState = nullptr;
    context->RSGetState(&prevState);

    // デバイス取得
    ID3D11Device* device = nullptr;
    context->GetDevice(&device);

    // カリングなしのラスタライザーステートを作成
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthClipEnable = TRUE;

    ID3D11RasterizerState* noCullState = nullptr;
    device->CreateRasterizerState(&rasterDesc, &noCullState);

    // カリングなしに設定
    context->RSSetState(noCullState);

    // SkyBox描画
    renderer->Render(rc, transform, model, ShaderId::Basic);

    // 元のラスタライザーステートに戻す
    context->RSSetState(prevState);

    // 解放
    if (noCullState) noCullState->Release();
    if (prevState) prevState->Release();
    if (device) device->Release();
}

void SkyBox::UpdateTransform()
{
	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//回転行列を作成
	DirectX::XMFLOAT3 a;//ラジアンにする。
	a.x = DirectX::XMConvertToRadians(angle.x);
	a.y = DirectX::XMConvertToRadians(angle.y);
	a.z = DirectX::XMConvertToRadians(angle.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(a.x, a.y, a.z);
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}