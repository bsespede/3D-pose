#pragma once

#include <string>
#include "CaptureMode.h"

class CalibrationMode
{
public:
	enum Value : unsigned int
	{
		INTRINSICS,
		EXTRINSICS
	};

	CalibrationMode() = default;
	constexpr CalibrationMode(Value calibrationMode) : value(calibrationMode) { }
	bool operator==(CalibrationMode other) const { return value == other.value; }
	bool operator!=(CalibrationMode other) const { return value != other.value; }
	std::string toString() const { return value == INTRINSICS ? "intrinsic" : "extrinsic"; }
	CaptureMode getCaptureMode() const { return value == INTRINSICS ? CaptureMode::GRAYSCALE : CaptureMode::PRECISION; }
private:
	Value value;
};