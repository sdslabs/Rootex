#include "maths.h"

Matrix Interpolate(Matrix& left, Matrix& right, float lerpFactor)
{
	Vector3 tLeft;
	Quaternion qLeft;
	Vector3 sLeft;
	left.Decompose(sLeft, qLeft, tLeft);

	Vector3 tRight;
	Quaternion qRight;
	Vector3 sRight;
	right.Decompose(sRight, qRight, tRight);

	Vector3 scale = Vector3::Lerp(sLeft, sRight, lerpFactor);
	Quaternion rotation = Quaternion::Slerp(qLeft, qRight, lerpFactor);
	Vector3 translation = Vector3::Lerp(tLeft, tRight, lerpFactor);

	return Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(translation);
}
