#include "Block.h"
#include "BlockManager.h"

void Block::Destroy()
{
	
}


void Block::GetAABB(DirectX::XMFLOAT3& outMin, DirectX::XMFLOAT3& outMax)
{
    //後で消す鴨　
    float margin = 0.00f;
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


float Block::DistSq(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

HitInfo Block::IsHit(Block* other)
{
    HitInfo info{ 0, 0.0f };

    DirectX::XMFLOAT3 minA, maxA;
    DirectX::XMFLOAT3 minB, maxB;

    GetAABB(minA, maxA);
    other->GetAABB(minB, maxB);

    // --- AABB衝突チェック ---
    if (!(minA.x <= maxB.x && maxA.x >= minB.x &&
        minA.y <= maxB.y && maxA.y >= minB.y &&
        minA.z <= maxB.z && maxA.z >= minB.z))
        return info;

    // --- 侵入量（MTV） ---
    float dx1 = maxB.x - minA.x;
    float dx2 = maxA.x - minB.x;
    float dy1 = maxB.y - minA.y;
    float dy2 = maxA.y - minB.y;
    float dz1 = maxB.z - minA.z;
    float dz2 = maxA.z - minB.z;

    float penX = (dx1 < dx2) ? dx1 : -dx2;
    float penY = (dy1 < dy2) ? dy1 : -dy2;
    float penZ = (dz1 < dz2) ? dz1 : -dz2;

    float absX = fabs(penX);
    float absY = fabs(penY);
    float absZ = fabs(penZ);

    const float FACE_EPS = 0.1f; // ←ここが“面判定の命”

    // --- X面 ---
    if (absX < absY && absX < absZ)
    {
        float overlapY = min(maxA.y, maxB.y) - max(minA.y, minB.y);
        float overlapZ = min(maxA.z, maxB.z) - max(minA.z, minB.z);

        if (overlapY > FACE_EPS && overlapZ > FACE_EPS)
        {
            info.dir = (penX > 0) ? 1 : 2;
            info.penetration = penX;
            return info;
        }
    }

    // --- Y面 ---
    if (absY < absX && absY < absZ)
    {
        float overlapX = min(maxA.x, maxB.x) - max(minA.x, minB.x);
        float overlapZ = min(maxA.z, maxB.z) - max(minA.z, minB.z);

        if (overlapX > FACE_EPS && overlapZ > FACE_EPS)
        {
            info.dir = (penY > 0) ? 3 : 4;
            info.penetration = penY;
            return info;
        }
    }

    // --- Z面 ---
    if (absZ < absX && absZ < absY)
    {
        float overlapX = min(maxA.x, maxB.x) - max(minA.x, minB.x);
        float overlapY = min(maxA.y, maxB.y) - max(minA.y, minB.y);

        if (overlapX > FACE_EPS && overlapY > FACE_EPS)
        {
            info.dir = (penZ > 0) ? 5 : 6;
            info.penetration = penZ;
            return info;
        }
    }

    return info;
}
