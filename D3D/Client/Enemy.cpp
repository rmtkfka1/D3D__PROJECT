#include "pch.h"
#include "Enemy.h"
#include "TimeManager.h"
#include "Transform.h"
#include "TransformTree.h"
#include "Player.h"
#include "EnemyBullet.h"
#include "Model.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Scene.h"
Enemy::Enemy():HireacyObject(PlayerType::Enemy)
{


}

Enemy::~Enemy()
{


}

void Enemy::Init()
{
	Super::Init();
}

void Enemy::Update()
{
	Animation();
	LookatPlayer();
	Shot();

	Super::Update();
}

void Enemy::Render()
{
	Super::Render();
}

void Enemy::Animation()
{
	float dt = TimeManager::GetInstance()->GetDeltaTime();
	float rotation = 720.0f;

	float result = rotation * dt; //1초에 2바퀴 회전하도록

	_transform->findByName(L"Top_Rotor")->AddRotate(vec3(0, result, 0));
}

void Enemy::LookatPlayer()
{
    if (_player == nullptr)
        return;

    vec3 enemyPos = GetTransform()->GetLocalPosition();  // Enemy's position
    vec3 playerPos = _player->GetTransform()->GetLocalPosition(); // Player's position

    vec3 direction = playerPos - enemyPos;
    direction.Normalize();

    float yaw = atan2(direction.x, direction.z);

    float distance = sqrt(direction.x * direction.x + direction.z * direction.z); 
    float pitch = atan2(direction.y, distance); 

    vec3 now = GetTransform()->GetLocalRotation();
    GetTransform()->SetLocalRotation(vec3(XMConvertToDegrees(pitch), XMConvertToDegrees(yaw) + 180.0f, now.z));



}

void Enemy::Shot()
{
    static float sum = 0;

    sum += TimeManager::GetInstance()->GetDeltaTime();

    if (sum >= 3.0f)
    {
        sum = 0;
		shared_ptr<EnemyBullet> object = make_shared<EnemyBullet>();
		shared_ptr<Model> data = Model::ReadData(L"playerBullet/playerBullet", L"playerBullet");
		object->SetModel(data);
		object->_direction = GetTransform()->GetLook();
		auto& pos = this->GetTransform()->GetLocalPosition() + GetTransform()->GetLook() * 100.0f;
		object->GetTransform()->SetLocalPosition(pos);
		object->GetTransform()->SetLocalScale(vec3(0.1f, 0.1f, 0.1f));
		object->SetShader(ResourceManager::GetInstance()->Get<Shader>(L"deferred.hlsl"));
		object->AddCollider("enemyBullet", ColliderType::Box);
		SceneManager::GetInstance()->GetCurrentScene()->ReserveAddGameObject(object, RenderingType::Deferred);
    }

}

void Enemy::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}

void Enemy::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}

