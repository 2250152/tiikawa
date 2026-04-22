#pragma once
#include "System/ModelRenderer.h"

class Block
{
public:
	Block() {}
	~Block() {}

	void Destroy();

	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

};