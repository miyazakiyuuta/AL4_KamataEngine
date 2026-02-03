#include "Game.h"
#include "KamataEngine.h"
#include "SceneManager.h"
#include "TitleScene.h"

using namespace KamataEngine;

void Game::Initialize() {
	sceneManager_ = new SceneManager();
	BaseScene* scene = new TitleScene();
	scene->Initialize();
	// 最初のシーンをタイトルシーンに設定
	sceneManager_->SetNextScene(scene);
}

void Game::Update() {
	sceneManager_->Update();
}

void Game::Draw() {
	sceneManager_->Draw();
}

void Game::Finalize() {
	delete sceneManager_;
}