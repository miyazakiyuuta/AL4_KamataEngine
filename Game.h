#pragma once

class SceneManager;

class Game {
public:
	void Initialize();
	void Update();
	void Draw();
	void Finalize();

private:
	SceneManager* sceneManager_ = nullptr;
};
