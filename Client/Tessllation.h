#pragma once
#include "CustomObject.h"
class Tessllation :public CustomObject
{
public:
	Tessllation();
	virtual ~Tessllation();

public:

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

};

