#include "g_pch.h"
#include "Transform.h"

namespace Cryptic
{
	Transform::Transform()
	{
		matrix = Math::M4x4::Identity();
	}

	Transform::~Transform()
	{
	}
}
