#include <DirectXCollision.h>
#include <cmath>
#include "Collision.h"

// レイキャスト
bool Collision::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const DirectX::XMFLOAT4X4& worldTransform,
	const Model* model,
	DirectX::XMFLOAT3& hitPosition,
	DirectX::XMFLOAT3& hitNormal)
{
	bool hit = false;

	// 始点と終点からレイのベクトルと長さを求める
	DirectX::XMVECTOR WorldRayStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldRayEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldRayEnd, WorldRayStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	float nearestDist = DirectX::XMVectorGetX(WorldRayLength);
	if (nearestDist <= 0.0f) return false;

	DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);

	// モデル内の全てのメッシュと交差判定を行う
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// メッシュのワールド行列を求める
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
		DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
		DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(GlobalTransform, ParentWorldTransform);

		// レイをメッシュのローカル空間に変換する
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
		DirectX::XMVECTOR LocalRayStart = DirectX::XMVector3Transform(WorldRayStart, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayEnd = DirectX::XMVector3Transform(WorldRayEnd, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayVec = DirectX::XMVectorSubtract(LocalRayEnd, LocalRayStart);
		DirectX::XMVECTOR LocalRayDirection = DirectX::XMVector3Normalize(LocalRayVec);
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(LocalRayVec));

		// メッシュ内の全ての三角形と交差判定を行う
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			// 三角形の頂点座標を取得
			const ModelResource::Vertex& a = mesh.vertices.at(mesh.indices.at(i + 0));
			const ModelResource::Vertex& b = mesh.vertices.at(mesh.indices.at(i + 1));
			const ModelResource::Vertex& c = mesh.vertices.at(mesh.indices.at(i + 2));

			DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
			DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

			// 三角形の交差判定
			if (DirectX::TriangleTests::Intersects(LocalRayStart, LocalRayDirection, A, B, C, distance))
			{
				// メッシュのローカル空間での交点を求める
				DirectX::XMVECTOR LocalHitVec = DirectX::XMVectorScale(LocalRayDirection, distance);
				DirectX::XMVECTOR LocalHitPosition = DirectX::XMVectorAdd(LocalRayStart, LocalHitVec);

				// メッシュのローカル空間での交点をワールド空間に変換する
				DirectX::XMVECTOR WorldHitPosition = DirectX::XMVector3Transform(LocalHitPosition, WorldTransform);

				// ワールド空間でのレイの始点から交点までの距離を求める
				DirectX::XMVECTOR WorldHitVec = DirectX::XMVectorSubtract(WorldHitPosition, WorldRayStart);
				DirectX::XMVECTOR WorldHitDist = DirectX::XMVector3Length(WorldHitVec);
				float worldHitDist = DirectX::XMVectorGetX(WorldHitDist);

				// 交点までの距離が今までに計算した交点より近い場合は交差したと判定する
				if (worldHitDist <= nearestDist)
				{
					// メッシュのローカル空間での三角形の法線ベクトルを算出
					DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
					DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
					DirectX::XMVECTOR LocalHitNormal = DirectX::XMVector3Cross(AB, BC);

					// メッシュのローカル空間での法線ベクトルをワールド空間に変換する
					DirectX::XMVECTOR WorldHitNormal = DirectX::XMVector3TransformNormal(LocalHitNormal, WorldTransform);

					// 三角形の裏表判定（内積の結果がマイナスならば表向き）
					DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(LocalRayDirection, LocalHitNormal);
					float dot = DirectX::XMVectorGetX(Dot);
					if (dot < 0)
					{
						// 交差したのでヒット情報を格納
						DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(WorldHitNormal));
						DirectX::XMStoreFloat3(&hitPosition, WorldHitPosition);

						// 交点までの距離を更新する
						nearestDist = worldHitDist;
						hit = true;
					}
				}
			}
		}
	}
	return hit;
}

// レイキャスト
//bool Collision::RayCast(
//	const DirectX::XMFLOAT3& start,
//	const DirectX::XMFLOAT3& end,
//	const DirectX::XMFLOAT4X4& worldTransform,
//	const Model* model,
//	DirectX::XMFLOAT3& hitPosition,
//	DirectX::XMFLOAT3& hitNormal,
//	float& hitDistance)
//{
//	bool hit = false;
//
//	// 始点と終点からレイのベクトルと長さを求める
//	DirectX::XMVECTOR WorldRayStart = DirectX::XMLoadFloat3(&start);
//	DirectX::XMVECTOR WorldRayEnd = DirectX::XMLoadFloat3(&end);
//	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldRayEnd, WorldRayStart);
//	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);
//
//	float nearestDist = DirectX::XMVectorGetX(WorldRayLength);
//	if (nearestDist <= 0.0f) return false;
//
//	DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);
//
//	// モデル内の全てのメッシュと交差判定を行う
//	const ModelResource* resource = model->GetResource();
//	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
//	{
//		// メッシュのワールド行列を求める
//		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
//		DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
//		DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(GlobalTransform, ParentWorldTransform);
//
//		// レイをメッシュのローカル空間に変換する
//		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
//		DirectX::XMVECTOR LocalRayStart = DirectX::XMVector3Transform(WorldRayStart, InverseWorldTransform);
//		DirectX::XMVECTOR LocalRayEnd = DirectX::XMVector3Transform(WorldRayEnd, InverseWorldTransform);
//		DirectX::XMVECTOR LocalRayVec = DirectX::XMVectorSubtract(LocalRayEnd, LocalRayStart);
//		DirectX::XMVECTOR LocalRayDirection = DirectX::XMVector3Normalize(LocalRayVec);
//		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(LocalRayVec));
//
//		// メッシュ内の全ての三角形と交差判定を行う
//		for (size_t i = 0; i < mesh.indices.size(); i += 3)
//		{
//			// 三角形の頂点座標を取得
//			const ModelResource::Vertex& a = mesh.vertices.at(mesh.indices.at(i + 0));
//			const ModelResource::Vertex& b = mesh.vertices.at(mesh.indices.at(i + 1));
//			const ModelResource::Vertex& c = mesh.vertices.at(mesh.indices.at(i + 2));
//
//			DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
//			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
//			DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);
//
//			// 三角形の交差判定
//			if (DirectX::TriangleTests::Intersects(LocalRayStart, LocalRayDirection, A, B, C, distance))
//			{
//				// メッシュのローカル空間での交点を求める
//				DirectX::XMVECTOR LocalHitVec = DirectX::XMVectorScale(LocalRayDirection, distance);
//				DirectX::XMVECTOR LocalHitPosition = DirectX::XMVectorAdd(LocalRayStart, LocalHitVec);
//
//				// メッシュのローカル空間での交点をワールド空間に変換する
//				DirectX::XMVECTOR WorldHitPosition = DirectX::XMVector3Transform(LocalHitPosition, WorldTransform);
//
//				// ワールド空間でのレイの始点から交点までの距離を求める
//				DirectX::XMVECTOR WorldHitVec = DirectX::XMVectorSubtract(WorldHitPosition, WorldRayStart);
//				DirectX::XMVECTOR WorldHitDist = DirectX::XMVector3Length(WorldHitVec);
//				float worldHitDist = DirectX::XMVectorGetX(WorldHitDist);
//
//				// 交点までの距離が今までに計算した交点より近い場合は交差したと判定する
//				if (worldHitDist <= nearestDist)
//				{
//					// メッシュのローカル空間での三角形の法線ベクトルを算出
//					DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
//					DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
//					DirectX::XMVECTOR LocalHitNormal = DirectX::XMVector3Cross(AB, BC);
//
//					// メッシュのローカル空間での法線ベクトルをワールド空間に変換する
//					DirectX::XMVECTOR WorldHitNormal = DirectX::XMVector3TransformNormal(LocalHitNormal, WorldTransform);
//
//					// 三角形の裏表判定（内積の結果がマイナスならば表向き）
//					DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(LocalRayDirection, LocalHitNormal);
//					float dot = DirectX::XMVectorGetX(Dot);
//					if (dot < 0)
//					{
//						// 交差したのでヒット情報を格納
//						DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(WorldHitNormal));
//						DirectX::XMStoreFloat3(&hitPosition, WorldHitPosition);
//
//						// 交点までの距離をぶち奪い
//						hitDistance = worldHitDist;
//
//						// 交点までの距離を更新する
//						nearestDist = worldHitDist;
//						hit = true;
//					}
//				}
//			}
//		}
//	}
//	return hit;
//}
// レイキャスト
bool Collision::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const DirectX::XMFLOAT4X4& worldTransform,
	const Model* model,
	DirectX::XMFLOAT3& hitPosition,
	DirectX::XMFLOAT3& hitNormal,
	float& hitDistance)
{
	bool hit = false;

	// 初期化
	hitDistance = FLT_MAX;

	// 始点と終点からレイベクトル作成
	DirectX::XMVECTOR WorldRayStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldRayEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldRayEnd, WorldRayStart);

	// レイ長さ
	float worldRayLength =
		DirectX::XMVectorGetX(DirectX::XMVector3Length(WorldRayVec));

	// レイ長さ0チェック
	if (worldRayLength < 0.0001f)
		return false;

	float nearestDist = worldRayLength;

	// 親ワールド行列
	DirectX::XMMATRIX ParentWorldTransform =
		DirectX::XMLoadFloat4x4(&worldTransform);

	// モデル取得チェック
	if (model == nullptr)
		return false;

	const ModelResource* resource = model->GetResource();

	if (resource == nullptr)
		return false;

	// 全メッシュ走査
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// ノード取得
		const Model::Node& node =
			model->GetNodes().at(mesh.nodeIndex);

		// メッシュワールド行列
		DirectX::XMMATRIX GlobalTransform =
			DirectX::XMLoadFloat4x4(&node.globalTransform);

		DirectX::XMMATRIX WorldTransform =
			DirectX::XMMatrixMultiply(GlobalTransform, ParentWorldTransform);

		// 逆行列
		DirectX::XMMATRIX InverseWorldTransform =
			DirectX::XMMatrixInverse(nullptr, WorldTransform);

		// NaNチェック
		if (std::isnan(InverseWorldTransform.r[0].m128_f32[0]))
			continue;

		// レイをローカル空間へ
		DirectX::XMVECTOR LocalRayStart =
			DirectX::XMVector3TransformCoord(
				WorldRayStart,
				InverseWorldTransform);

		DirectX::XMVECTOR LocalRayEnd =
			DirectX::XMVector3TransformCoord(
				WorldRayEnd,
				InverseWorldTransform);

		DirectX::XMVECTOR LocalRayVec =
			DirectX::XMVectorSubtract(
				LocalRayEnd,
				LocalRayStart);

		// ローカルレイ長さ
		float len =
			DirectX::XMVectorGetX(
				DirectX::XMVector3Length(LocalRayVec));

		// ゼロベクトル防止
		if (len < 0.0001f)
			continue;

		// 安全な正規化
		DirectX::XMVECTOR LocalRayDirection =
			DirectX::XMVectorScale(LocalRayVec, 1.0f / len);

		// NaNチェック
		if (std::isnan(LocalRayDirection.m128_f32[0]))
			continue;

		float distance = len;

		// 三角形ループ
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			const ModelResource::Vertex& a =
				mesh.vertices.at(mesh.indices.at(i + 0));

			const ModelResource::Vertex& b =
				mesh.vertices.at(mesh.indices.at(i + 1));

			const ModelResource::Vertex& c =
				mesh.vertices.at(mesh.indices.at(i + 2));

			DirectX::XMVECTOR A =
				DirectX::XMLoadFloat3(&a.position);

			DirectX::XMVECTOR B =
				DirectX::XMLoadFloat3(&b.position);

			DirectX::XMVECTOR C =
				DirectX::XMLoadFloat3(&c.position);

			// 交差判定
			if (DirectX::TriangleTests::Intersects(
				LocalRayStart,
				LocalRayDirection,
				A,
				B,
				C,
				distance))
			{
				// ローカル交点
				DirectX::XMVECTOR LocalHitVec =
					DirectX::XMVectorScale(
						LocalRayDirection,
						distance);

				DirectX::XMVECTOR LocalHitPosition =
					DirectX::XMVectorAdd(
						LocalRayStart,
						LocalHitVec);

				// ワールド交点
				DirectX::XMVECTOR WorldHitPosition =
					DirectX::XMVector3TransformCoord(
						LocalHitPosition,
						WorldTransform);

				// 距離計算
				DirectX::XMVECTOR WorldHitVec =
					DirectX::XMVectorSubtract(
						WorldHitPosition,
						WorldRayStart);

				float worldHitDist =
					DirectX::XMVectorGetX(
						DirectX::XMVector3Length(WorldHitVec));

				// 最短ヒットのみ
				if (worldHitDist <= nearestDist)
				{
					// 法線作成
					DirectX::XMVECTOR AB =
						DirectX::XMVectorSubtract(B, A);

					DirectX::XMVECTOR AC =
						DirectX::XMVectorSubtract(C, A);

					DirectX::XMVECTOR LocalHitNormal =
						DirectX::XMVector3Cross(AB, AC);

					// 法線長さ
					float normalLen =
						DirectX::XMVectorGetX(
							DirectX::XMVector3Length(LocalHitNormal));

					if (normalLen < 0.0001f)
						continue;

					// 法線正規化
					LocalHitNormal =
						DirectX::XMVectorScale(
							LocalHitNormal,
							1.0f / normalLen);

					// 裏表判定
					float dot =
						DirectX::XMVectorGetX(
							DirectX::XMVector3Dot(
								LocalRayDirection,
								LocalHitNormal));

					if (dot < 0.0f)
					{
						// ワールド法線
						DirectX::XMVECTOR WorldHitNormal =
							DirectX::XMVector3TransformNormal(
								LocalHitNormal,
								WorldTransform);

						WorldHitNormal =
							DirectX::XMVector3Normalize(
								WorldHitNormal);

						// 保存
						DirectX::XMStoreFloat3(
							&hitPosition,
							WorldHitPosition);

						DirectX::XMStoreFloat3(
							&hitNormal,
							WorldHitNormal);

						hitDistance = worldHitDist;
						nearestDist = worldHitDist;

						hit = true;
					}
				}
			}
		}
	}

	return hit;
}