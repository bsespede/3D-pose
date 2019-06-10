#pragma once

#include <string>

class CalibrationType
{
public:
	enum Value : unsigned int
	{
		INTRINSICS,
		EXTRINSICS,
		POSES
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

	std::string toString() const
	{ 
		return value == INTRINSICS ? "intrinsics" : value == EXTRINSICS? "extrinsics" : "poses";
	}

private:
	Value value;
};
