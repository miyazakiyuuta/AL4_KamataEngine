#include "SceneManager.h"

void SceneManager::Update() {
	// 次シーンの予約があるなら
	if (nextScene_) {
		// 旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}

		// シーン切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;

		scene_->SetSceneManager(this);
		// 次シーンを初期化
		scene_->Initialize();
	}

	if (!scene_) {
		return;
	}

	scene_->Update();
}

void SceneManager::Draw() {
	if (!scene_) {
		return;
	}
	scene_->Draw();
}

SceneManager::~SceneManager() {
	if (scene_) {
		scene_->Finalize();
		delete scene_;
	}
}

void SceneManager::SetNextScene(BaseScene* nextScene) {
	if (!nextScene) { return; }
	nextScene_ = nextScene;
}
