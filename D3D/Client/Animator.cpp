#include "pch.h"
#include "Animator.h"
#include "Model.h"

void Animator::Init()
{
	_matrix = make_shared<AnimationMatrix>();


}

void Animator::PushData()
{

}

void Animator::CreateMatrix()
{
	vector<Matrix> tempAnimBoneTransforms(MAX_BONE, Matrix::Identity);

	shared_ptr<ModelAnimation> animation = _model->GetAnimation();

	for (uint32 f = 0; f < animation->frameCount; f++)
	{
		for (uint32 b = 0; b < _model->GetBoneCount(); b++)
		{
			shared_ptr<ModelBone> bone = _model->GetBoneByIndex(b);

			Matrix matAnimation;

			shared_ptr<ModelKeyframe> frame = animation->GetKeyframe(bone->name);

			if (frame != nullptr)
			{
				ModelKeyframeData& data = frame->transforms[f];

				Matrix S, R, T;
				S = Matrix::CreateScale(data.scale.x, data.scale.y, data.scale.z);
				R = Matrix::CreateFromQuaternion(data.rotation);
				T = Matrix::CreateTranslation(data.translation.x, data.translation.y, data.translation.z);

				matAnimation = S * R * T;
			}
			else
			{
				matAnimation = Matrix::Identity;
			}


			Matrix toRootMatrix = bone->transformData;
			Matrix invGlobal = toRootMatrix.Invert();

			int32 parentIndex = bone->parentIndex;

			Matrix matParent = Matrix::Identity;
			if (parentIndex >= 0)
				matParent = tempAnimBoneTransforms[parentIndex];

			tempAnimBoneTransforms[b] = matAnimation * matParent;


			_matrix->transforms[f][b] = invGlobal * tempAnimBoneTransforms[b];
		}
	}
}
