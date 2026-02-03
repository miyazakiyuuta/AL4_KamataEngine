#pragma once
#include "BaseScene.h"

class SceneManager {
public:
	void Update();
	void Draw();
	~SceneManager();

	void SetNextScene(BaseScene* nextScene);

private:
	// 今のシーン
	BaseScene* scene_ = nullptr;
	// 次のシーン
	BaseScene* nextScene_ = nullptr;
};
