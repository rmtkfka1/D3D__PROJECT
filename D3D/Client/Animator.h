#pragma once


class Model;
class StructuredBuffer;

struct AnimationMatrix
{
	array<array<Matrix, MAX_BONE>, MAX_FRAME> transforms;
};

class Animator
{

public:
	void SetModel(shared_ptr<Model> model) { _model = model; }
	void Init();
	void PushData();

private:

	void CreateMatrix();

private:
	shared_ptr<AnimationMatrix>  _matrix;
	shared_ptr<Model> _model;
};

