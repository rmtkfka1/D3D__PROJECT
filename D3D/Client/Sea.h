#pragma once
#include "CustomObject.h"
class Sea : public CustomObject
{
public:
	using Super = CustomObject;

	Sea();
	virtual ~Sea();

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

	void SetMove(bool bMove) { _bMove = bMove; }
private:

	bool _bMove = true;

};

