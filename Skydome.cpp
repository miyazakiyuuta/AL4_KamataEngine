#include "Skydome.h"

using namespace KamataEngine;

void Skydome::Initialize() { 
	model_ = Model::CreateFromOBJ("skydome", true);
	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize();
	worldTransform_->translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_->scale_ = {100.0f, 100.0f, 100.0f};
}

void Skydome::Update() {}

void Skydome::Draw(const KamataEngine::Camera& camera) { 
	if (model_) {
		model_->Draw(*worldTransform_, camera);
	}
}

Skydome::~Skydome() { delete model_; }
