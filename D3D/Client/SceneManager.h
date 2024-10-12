#pragma once
#pragma once

enum class SceneType
{
	NONE,
	StartScene,
	STAGE1,
	STAGE2,

};

class Scene;

class SceneManager
{

public:

	static SceneManager* GetInstance()
	{

		static SceneManager instance;
		return &instance;
	}


public:
	void Init();
	void LateUpdate();
	void Run();

public:

	void ChangeScene(SceneType type);
	shared_ptr<Scene> GetCurrentScene() { return _scene; }
	SceneType GetSceneType() { return _sceneType; }

private:

	shared_ptr<Scene> _scene;
	SceneType _sceneType = SceneType::NONE;

};

