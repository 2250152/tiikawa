#pragma once

#include<DirectXMath.h>
#include"System/Model.h"
#include"System/ShapeRenderer.h"
#include"System/ModelRenderer.h"
#include<string>

class SkyBox
{
private:
	Model* model = nullptr;
	
	DirectX::XMFLOAT3     position = { 0,0,0 };
	DirectX::XMFLOAT3     angle = { 0,0,0 };
	DirectX::XMFLOAT3     scale = { 1,1,1 };
	DirectX::XMFLOAT4X4   transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

public:
	static SkyBox& Instance()
	{
		static SkyBox instabce;
		return instabce;
	}

	SkyBox();
	~SkyBox();

	void Initialize();

	void Finalize();
	
	void UpdateTransform();

	void Update(float elapsedTime);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

};