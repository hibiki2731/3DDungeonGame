#pragma once
#include <map>
#include <vector>
#include <memory>
#include "Graphic.h"

struct MeshData {
	int NumParts;
	//頂点バッファ
	std::vector<UINT> NumVertices;
	std::vector<ComPtr<ID3D12Resource>> VertexBuf;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> VertexBufView;
	//マテリアルデータ
	std::vector<XMFLOAT4> Material;
	std::vector<std::string> TextureName;
};

enum class ObjectName {
	ROCK_WALL_SIDE,
	ROCK_WALL,
	ROCK_FLOOR,
	SLIME,
};

class AssetManager
{
public:

	AssetManager(const std::shared_ptr<Graphic>& graphic);
	~AssetManager();

	void create(ObjectName objectName);

	std::shared_ptr<MeshData> getMeshData(ObjectName objectName);

private:
	std::shared_ptr<Graphic> mGraphic;
	std::map<ObjectName, std::shared_ptr<MeshData>> mLoadData;
};

