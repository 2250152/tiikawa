#pragma once

#include <DirectXMath.h>
#include"System/ModelRenderer.h"


class Collision
{
public:
	//レイキャスト
	static bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const DirectX::XMFLOAT4X4& worldTransform,
		const Model* model,
		DirectX::XMFLOAT3& hitPOsition,
		DirectX::XMFLOAT3& hitNormal
	);
};
