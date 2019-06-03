#pragma once

#include <string>

using namespace std;

class CaptureType
{
public:
	enum Value : unsigned int
	{
		CALIBRATION,
		MOCAP
	};

	CaptureType() = default;

	constexpr CaptureType(Value captureType) : value(captureType) { }

	bool operator==(CaptureType other) const
	{ 
		return value == other.value;
	}

	bool operator!=(CaptureType other) const
	{ 
		return value != other.value;
	}

	string toString() const
	{ 
		return value == CALIBRATION ? "calibration" : "mocap";
	}

private:
	Value value;
};
