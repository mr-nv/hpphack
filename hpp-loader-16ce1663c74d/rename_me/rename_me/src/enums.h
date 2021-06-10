#pragma once

enum ImageSize : unsigned
{
	Small,
	Medium,
	Large
};

enum class InjectState
{
	None,
	Download,
	LookingProcess,
	Processing,
};