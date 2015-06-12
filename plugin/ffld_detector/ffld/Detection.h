#ifndef FFLD_DETECTION_H
#define FFLD_DETECTION_H

#include "HOGPyramid.h"
#include "Rectangle.h"

namespace FFLD
{
struct Detection : public Rectangle {
	HOGPyramid::Scalar score;
	int x;
	int y;
	int z;

	Detection() : score(0), x(0), y(0), z(0)
	{
	}

	Detection(HOGPyramid::Scalar score, int x, int y, int z, Rectangle bndbox) : Rectangle(bndbox),
	score(score), x(x), y(y), z(z)
	{
	}

	bool operator<(const Detection & detection) const
	{
		return score > detection.score;
	}
};
}

#endif // FFLD_DETECTION_H
