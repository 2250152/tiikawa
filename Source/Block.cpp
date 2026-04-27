#include "Block.h"
#include "BlockManager.h"

void Block::Destroy()
{
	
}


void Block::GetAABB(DirectX::XMFLOAT3& outMin, DirectX::XMFLOAT3& outMax)
{
    float margin = 0.01f;
    outMin =
    {
        position.x + model->min.x * scale.x - margin,
        position.y + model->min.y * scale.y - margin,
        position.z + model->min.z * scale.z - margin
    };

    outMax =
    {
        position.x + model->max.x * scale.x + margin,
        position.y + model->max.y * scale.y + margin,
        position.z + model->max.z * scale.z + margin
    };
}


DirectX::XMFLOAT3 Block::GetTopCenter()
{
    DirectX::XMFLOAT3 Min, Max;
    GetAABB(Min, Max);

    return
    {
        (Min.x + Max.x) * 0.5f,
        Max.y,
        (Min.z + Max.z) * 0.5f
    };
}
DirectX::XMFLOAT3 Block::GetBottomCenter()
{
    DirectX::XMFLOAT3 Min, Max;
    GetAABB(Min, Max);

    return
    {
        (Min.x + Max.x) * 0.5f,
        Min.y,
        (Min.z + Max.z) * 0.5f
    };
}
DirectX::XMFLOAT3 Block::GetFrontCenter()
{
    DirectX::XMFLOAT3 Min, Max;
    GetAABB(Min, Max);

    return
    {
        (Min.x + Max.x) * 0.5f,
        (Min.y + Max.y) * 0.5f,
        Max.z
    };
}
DirectX::XMFLOAT3 Block::GetBackCenter()
{
    DirectX::XMFLOAT3 Min, Max;
    GetAABB(Min, Max);

    return
    {
        (Min.x + Max.x) * 0.5f,
        (Min.y+Max.y)*0.5f,
        Min.z
    };
}
DirectX::XMFLOAT3 Block::GetRightCenter()
{
    DirectX::XMFLOAT3 Min, Max;
    GetAABB(Min, Max);

    return
    {
        Max.x,
        (Min.y+Max.y)*0.5f,
        (Min.z + Max.z) * 0.5f
    };
}
DirectX::XMFLOAT3 Block::GetLeftCenter()
{
    DirectX::XMFLOAT3 Min, Max;
    GetAABB(Min, Max);

    return
    {
         Min.x,
        (Min.y + Max.y)*0.5f,
        (Min.z + Max.z) * 0.5f
    };
}

bool Block::IsHit(Block* other)
{
    DirectX::XMFLOAT3 minA, maxA;
    DirectX::XMFLOAT3 minB, maxB;

    GetAABB(minA, maxA);
    other->GetAABB(minB, maxB);

    return
        (minA.x <= maxB.x && maxA.x >= minB.x) &&
        (minA.y <= maxB.y && maxA.y >= minB.y) &&
        (minA.z <= maxB.z && maxA.z >= minB.z);
}


bool Block::WillHit(Block* other, float nextX)
{
    float oldX = position.x;
    position.x = nextX;

    DirectX::XMFLOAT3 minA, maxA;
    GetAABB(minA, maxA);

    DirectX::XMFLOAT3 minB, maxB;
    other->GetAABB(minB, maxB);

    position.x = oldX;

    return
        (minA.x <= maxB.x && maxA.x >= minB.x) &&
        (minA.y <= maxB.y && maxA.y >= minB.y) &&
        (minA.z <= maxB.z && maxA.z >= minB.z);
}