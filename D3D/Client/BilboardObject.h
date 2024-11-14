#pragma once
#include "CustomObject.h"
class BilboardObject :	public CustomObject
{
public:
	BilboardObject();
	virtual ~BilboardObject();

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

public:

	bool _useWithHeightMap = false;
};

