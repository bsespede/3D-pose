#pragma once

#include <string>

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

	std::string toString() const
	{ 
		return value == CALIBRATION ? "calibration" : "mocap";
	}

private:
	Value value;
};
