#pragma once
#include "KamataEngine.h"

enum class MapChipType {
	kBlank, // 余白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};


/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField {
public:

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	// 範囲矩形
	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	void ResetMapChipData(); // マップチップデータをリセット

	void LoadMapChipCsv(const std::string& filePath); // マップチップCSVを読み込む

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex); // インデックスからマップチップの種類を取得

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex); // インデックスからマップチップの位置を取得

	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position); // 位置からマップチップのインデックスを取得

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex); // インデックスからマップチップの範囲矩形を取得

	uint32_t GetNumBLockVirtical() { return kNumBlockVirtical; } // 垂直方向のブロック数を取得
	uint32_t GetNumBLockHorizontal() { return kNumBlockHorizontal; } // 水平方向のブロック数を取得

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 10;
	static inline const uint32_t kNumBlockHorizontal = 20;

	MapChipData mapChipData_;
};
