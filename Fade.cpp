#include "Fade.h"
#include <algorithm>

using namespace KamataEngine;

void Fade::Initialize() {

	spriteL_ = KamataEngine::Sprite::Create(TextureManager::Load("black1x1.png"), {});
	spriteL_->SetPosition({-640.0f, 0.0f}); // 画面から左外側
	spriteL_->SetSize({640.0f, 720.0f}); // 画面の半分サイズ
	spriteL_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});

	spriteR_ = KamataEngine::Sprite::Create(TextureManager::Load("black1x1.png"), {});
	spriteR_->SetPosition({640.0f, 0.0f}); // 画面から右外側
	spriteR_->SetSize({640.0f, 720.0f}); // 画面の半分サイズ
	spriteR_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
}

void Fade::Update() {
	float t;
	float easedT;
	float pos;
	// フェード状態による分岐
	switch (status_) {
	case Status::None:
		// 何もしない
		return;
	case Status::FadeIn: // 見えてくる
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 10.0f; // 1/6秒
		// フェード持続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}

		t = counter_ / duration_;
		easedT = t;
		easedT = t * t * t;
		// start*(1.0f-t)+goal*t;
		pos = 0.0f * (1.0f - easedT) + -640.0f * easedT;
		spriteL_->SetPosition({pos, 0.0f});
		pos = 640.0f * (1.0f - easedT) + 1280.0f * easedT;
		spriteR_->SetPosition({pos, 0.0f});

		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を小さくする
		//spriteL_->SetColor({0, 0, 0, std::clamp(1.0f - (t), 0.0f, 1.0f)});
		break;
	case Status::FadeOut: // 見えなくなる
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 30.0f;
		// フェード持続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}

		t = counter_ / duration_;
		const float n1 = 7.5625f;
		const float d1 = 2.75f;
		if (t < 1.0f / d1) {
			easedT = n1 * t * t;
		} else if (t < 2.0f / d1) {
			easedT = n1 * (t -= 1.5f / d1) * t + 0.75f;
		} else if (t < 2.5f / d1) {
			easedT = n1 * (t -= 2.25f / d1) * t + 0.9375f;
		} else {
			easedT = n1 * (t -= 2.625f / d1) * t + 0.984375f;
		}
		pos = -640.0f * (1.0f - easedT) + 0.0f * easedT;
		spriteL_->SetPosition({pos, 0.0f});
		pos = 1280.0f * (1.0f - easedT) + 640.0f * easedT;
		spriteR_->SetPosition({pos, 0.0f});

		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		//spriteL_->SetColor({0, 0, 0, std::clamp(t, 0.0f, 1.0f)});
		break;
	}
}

void Fade::Draw() {
	if (status_ == Status::None) {
		return;
	}
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	Sprite::PreDraw(dxCommon_->GetCommandList());
	spriteL_->Draw();
	spriteR_->Draw();
	Sprite::PostDraw();
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
	if (status_ == Status::FadeIn) {
		//spriteL_->SetColor({0, 0, 0, 1.0f});
		spriteL_->SetPosition({-640.0f, 0.0f});
		spriteR_->SetPosition({1280.0f, 0.0f});
	} else if (status_ == Status::FadeOut) {
		//spriteL_->SetColor({0, 0, 0, 0.0f});
		spriteL_->SetPosition({0.0f, 0.0f});
		spriteR_->SetPosition({640.0f, 0.0f});
	}
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinished() const {
	// フェード状態による分岐
	switch (status_) {
	case Status::FadeIn:
		if (counter_ < duration_) {
			return false;
		}
	case Status::FadeOut:
		if (counter_ < duration_) {
			return false;
		}
	}
	return true;
}
