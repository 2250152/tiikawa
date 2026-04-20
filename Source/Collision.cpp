#include"Collision.h"

//球と球の交差判定
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outpositionB)
{
	//A→Bの単位ベクトルを算出   ※XMVECTOR は「CPU が SIMD 命令（ベクトル演算命令）」で高速に計算するための特別な型です。
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);//←VECTORというのは変数を３つ持っているという事らしい。
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);//関数を変えている。なぜか知らん
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);//2転換の距離を統合して値を返している？のかな。DirectXが元々用意している関数？
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	//距離判定
	float range = radiusA + radiusB; //2点間の距離
	if (lengthSq > range*range)
	{
		return false;
	}

	//AからBを押し出す
	Vec = DirectX::XMVector3Normalize(Vec);
	Vec = DirectX::XMVectorScale(Vec, range);//押し出す
	PositionB = DirectX::XMVectorAdd(PositionA, Vec);
	DirectX::XMStoreFloat3(&outpositionB, PositionB);//PDFでいう青いサークルを押し出す

	return true;
}

//円柱と円柱の交差判定
bool Collision::IntersectCylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	float heightA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outPositionB)
{
	//Aの足元がBの頭より上なら当たってない
	if (positionA.y > positionB.y + heightB) {
		return false;
	}
	//Aの頭がBの足元より下なら当たっていない
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}
	//XZ平面での範囲チェック
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);//←三角形の斜めの長さが出る
	if (distXZ > range)
	{
		return false; //当たってません
	}
	//単位ベクトル化
	vx /= distXZ;
	vz /= distXZ;
	//AがBを押し出す
	outPositionB.x = positionA.x + (vx * range);
	outPositionB.y = positionB.y;
	outPositionB.z = positionA.z + (vz * range);

	return true;
}

//球と円柱の交差判定
bool Collision::IntersectSphereVsCylinder(
	const DirectX::XMFLOAT3& spherePosition,
	float sphereRadius,
	const DirectX::XMFLOAT3& cylinderPosition,
	float cylinderRadius,
	float cylinderHeight,
	DirectX::XMFLOAT3& outCylinderPosition)
{
	//高さチェック
	if (spherePosition.y + sphereRadius < cylinderPosition.y)return false;
	if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)return true;

	//XZ平面での範囲チェック
	float vx = cylinderPosition.x - spherePosition.x;
	float vz = cylinderPosition.z - spherePosition.z;
	float range = sphereRadius + cylinderRadius;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)return false;

	//球が円柱を押し出す
	vx /= distXZ;
	vz /= distXZ;
	outCylinderPosition.x = spherePosition.x + (vx * range);
	outCylinderPosition.y = cylinderPosition.y;
	outCylinderPosition.z = spherePosition.z + (vz * range);

	return true;
}