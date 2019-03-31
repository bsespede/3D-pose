#pragma once

#include <string>

class CaptureMode
{
public:
	enum Value : unsigned int
	{
		PRECISION,
		GRAYSCALE
	};

	CaptureMode() = default;
	constexpr CaptureMode(Value captureMode) : value(captureMode) { }
	bool operator==(CaptureMode other) const { return value == other.value; }
	bool operator!=(CaptureMode other) const { return value != other.value; }

private:
	Value value;
};