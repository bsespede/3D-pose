#pragma once

#include <Core/Frame.h>

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
	Core::eVideoMode toOptitrackMode() const { return value == PRECISION ? Core::eVideoMode::PrecisionMode : Core::eVideoMode::GrayscaleMode; }

private:
	Value value;
};
