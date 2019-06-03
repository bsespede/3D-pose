#pragma once

#include <string>

using namespace std;

class CalibrationType
{
public:
	enum Value : unsigned int
	{
		INTRINSICS,
		EXTRINSICS
	};

	CalibrationType() = default;

	constexpr CalibrationType(Value calibrationType) : value(calibrationType) { }

	bool operator==(CalibrationType other) const
	{ 
		return value == other.value;
	}

	bool operator!=(CalibrationType other) const
	{ 
		return value != other.value;
	}

	string toString() const
	{ 
		return value == INTRINSICS ? "intrinsics" : "extrinsics";
	}

private:
	Value value;
};
