#include "Block.h"
#include "BlockManager.h"
#include "Collision.h"

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

    //AABB衝突チェック
    if (!(minA.x <= maxB.x && maxA.x >= minB.x &&
        minA.y <= maxB.y && maxA.y >= minB.y &&
        minA.z <= maxB.z && maxA.z >= minB.z))
        return info;

    //侵入量
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

    const float FACE_EPS = 0.1f; 

    //X
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

    //Y
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

    //Z
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


//越智君が追加したやつ ==============================================================================================
std::vector<Block::FaceData> Block::GetAllFaces() {
    std::vector<FaceData> faces(6);

    // 現在のこのブロックの回転行列（個々の angle を使用）
    DirectX::XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);

    // 6つの面の定義（ローカル空間）
    struct { DirectX::XMVECTOR n; float h, w, d; } faceDefs[6] = {
        { { 0, 1, 0}, 1, 0, 0 }, // Top
        { { 0,-1, 0}, 1, 0, 0 }, // Bottom
        { { 0, 0, 1}, 0, 0, 1 }, // Front
        { { 0, 0,-1}, 0, 0, 1 }, // Back
        { { 1, 0, 0}, 0, 1, 0 }, // Right
        { {-1, 0, 0}, 0, 1, 0 }  // Left
    };

    // ブロックのサイズ（スケール考慮）
    float halfSizeX = (model->max.x - model->min.x) * scale.x * 0.5f;
    float halfSizeY = (model->max.y - model->min.y) * scale.y * 0.5f;
    float halfSizeZ = (model->max.z - model->min.z) * scale.z * 0.5f;

    for (int i = 0; i < 6; i++) {
        // 1. 【法線の回転】
        // ローカル法線にブロックの回転を適用。これで「今どっちを向いているか」が出る
        DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(faceDefs[i].n, rot);//VECTORとMATRIXの掛け算の結果をVECTORで返す関数

        // 2. 【位置の回転】
        // ローカルの中心座標を回転させてから、ブロックのワールド座標を足す
        DirectX::XMVECTOR localOffset = DirectX::XMVectorMultiply(faceDefs[i].n, DirectX::XMVectorSet(halfSizeX, halfSizeY, halfSizeZ, 0));//XMVectorMultiply...2つのベクトルの各成分ごとの掛け算をする関数
        DirectX::XMVECTOR worldPos = DirectX::XMVectorAdd(DirectX::XMVector3Transform(localOffset, rot), pos);

        DirectX::XMStoreFloat3(&faces[i].worldNormal, worldNormal);
        DirectX::XMStoreFloat3(&faces[i].worldPos, worldPos);
    }
    return faces;
}

Block::FaceData Block::FindBestFace(DirectX::XMVECTOR hitPos) {
    std::vector<FaceData> allFaces = GetAllFaces();

    // 1. ソート (DistSqの引数を XMVECTOR に合わせる)
    std::sort(allFaces.begin(), allFaces.end(), [&](const FaceData& a, const FaceData& b) {
        DirectX::XMVECTOR posA = DirectX::XMLoadFloat3(&a.worldPos);
        DirectX::XMVECTOR posB = DirectX::XMLoadFloat3(&b.worldPos);
        return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(hitPos, posA))) <
            DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(hitPos, posB)));
        });

    // 2. 判定ループ
    for (const auto& face : allFaces) {
        DirectX::XMVECTOR wPos = DirectX::XMLoadFloat3(&face.worldPos);
        DirectX::XMVECTOR wNormal = DirectX::XMLoadFloat3(&face.worldNormal);

        // XMVECTOR 同士なら * でスカラー倍ができます
        DirectX::XMVECTOR rayStartVec = DirectX::XMVectorSubtract(wPos, DirectX::XMVectorScale(wNormal, 0.1f));
        DirectX::XMVECTOR rayDirVec = wNormal;

        // CheckIfBlocked に渡すために XMFLOAT3 に変換して格納
        DirectX::XMFLOAT3 rayStart, rayDir;
        DirectX::XMStoreFloat3(&rayStart, rayStartVec);
        DirectX::XMStoreFloat3(&rayDir, rayDirVec);

        // 3. ここで自作の判定関数を呼ぶ
        if (!CheckIfBlocked(rayStart, rayDir)) { return face; }
    }

    return allFaces[0];
}

bool Block::CheckIfBlocked(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDir) {
    // レイの終点を決める（面から外側へ 0.5f ほど飛ばす）
    DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&rayStart);
    DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&rayDir);
    DirectX::XMVECTOR endVec = DirectX::XMVectorAdd(startVec, DirectX::XMVectorScale(dirVec, 0.5f));

    DirectX::XMFLOAT3 rayEnd;
    DirectX::XMStoreFloat3(&rayEnd, endVec);

    // 衝突結果を受け取るダミー変数（今回は使わないが引数として必要）
    DirectX::XMFLOAT3 dummyPos, dummyNormal;
    float dummyDist;

    // シーン内の全ブロックに対してレイキャストを行う
    // ※BlockManagerなどで管理している全ブロックのリストを走査します
    for (const auto& group : BlockManager::Instance().GetGroups())
    {
        for (const auto& block : group->GetBlocks())
        {
            if (block.get() == this) continue;

            if (Collision::RayCast(
                rayStart,
                rayEnd,
                block->Gettranceform(),  // block に変更
                block->GetModel(),       // block に変更
                dummyPos,
                dummyNormal,
                dummyDist))
            {
                return true;
            }
        }
    }

    return false; // 何も当たらなかった ＝ その面は開いている
}