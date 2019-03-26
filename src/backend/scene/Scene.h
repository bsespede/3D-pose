#pragma once

#include <string>
#include "enum/SceneStatus.h"

class Scene
{
private:
	const std::string name;
	const SceneStatus status;
public:
	Scene(std::string name, SceneStatus status);
	std::string getName() const;
	SceneStatus getStatus() const;
};
