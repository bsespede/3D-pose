#pragma once

#include <string>

using namespace std;

class Operation
{
public:
	enum Value : unsigned int
	{
		INTRINSICS,
		EXTRINSICS,
		SCENE
	};

	Operation() = default;

	constexpr Operation(Value operation) : value(operation) { }

	bool operator==(Operation other) const
	{ 
		return value == other.value;
	}

	bool operator!=(Operation other) const
	{ 
		return value != other.value;
	}

	string toString() const
	{ 
		return value == INTRINSICS ? "intrinsics" : value == EXTRINSICS ? "extrinsics" : "scene";
	}

private:
	Value value;
};
