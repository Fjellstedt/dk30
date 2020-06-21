#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

namespace Cryptic
{
	class Transform
	{
	public:
		Transform();
		~Transform();
		Math::M4x4 matrix;
	};
}
#endif //!_TRANSFORM_H_