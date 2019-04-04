#pragma once

class Input
{
public:
	enum Value : unsigned int
	{
		CREATE,
		LOAD
	};

	Input() = default;
	constexpr Input(Value operation) : value(input) { }
	bool operator==(Input other) const { return value == other.value; }
	bool operator!=(Input other) const { return value != other.value; }

private:
	Value value;
};
