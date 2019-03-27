#pragma once

#include <string>

class CaptureMode
{
public:
	enum Value : unsigned int
	{
		UNCALIBRATED_CAPTURE,
		CALIBRATED_CAPTURE
	};

	CaptureMode() = default;
	constexpr CaptureMode(Value captureMode) : value(captureMode) { }
	bool operator==(CaptureMode other) const { return value == other.value; }
	bool operator!=(CaptureMode other) const { return value != other.value; }
	std::string toString() const { return value == UNCALIBRATED_CAPTURE? "calibration" : "capture"; }

private:
	Value value;
};