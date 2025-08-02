#include "ui/CMath.h"

#include <cmaths>

void SRect::ExtendTo(const SVector2i& InPoint)
{
	if (!bIsValid)
	{
		bIsValid = true;
		TopLeft = BottomRight = InPoint;
	}
	else
	{
		TopLeft.X = std::min(TopLeft.X, InPoint.X);
		TopLeft.Y = std::min(TopLeft.Y, InPoint.Y);

		BottomRight.X = std::max(BottomRight.X, InPoint.X);
		BottomRight.Y = std::max(BottomRight.Y, InPoint.Y);
	}
}

void SRect::ExtendTo(const SRect& RhsRect)
{
	if (!bIsValid)
	{
		TopLeft = RhsRect.TopLeft;
		BottomRight = RhsRect.BottomRight;
		bIsValid = RhsRect.bIsValid;
	}
	else
	{
		TopLeft.X = std::min(TopLeft.X, RhsRect.TopLeft.X);
		TopLeft.Y = std::min(TopLeft.Y, RhsRect.TopLeft.Y);

		BottomRight.X = std::max(BottomRight.X, RhsRect.BottomRight.X);
		BottomRight.Y = std::max(BottomRight.Y, RhsRect.BottomRight.Y);
	}
}

bool SRect::Intersercts(const SRect& RhsRect) const
{
	if (!bIsValid || !RhsRect.bIsValid)
		return false;

	if ((TopLeft.X > RhsRect.BottomRight.X) || (RhsRect.TopLeft.X > BottomRight.X))
		return false;

	if ((TopLeft.Y > RhsRect.BottomRight.Y) || (RhsRect.TopLeft.Y > BottomRight.Y))
		return false;

	return true;
}

bool SRect::Contains(const SVector2i& Point) const
{
	if (!bIsValid)
		return false;

	return Point.X >= TopLeft.X && Point.X <= BottomRight.X && Point.Y >= TopLeft.Y && Point.Y <= BottomRight.Y;
}

void SRect::Clamp(const SVector2i& Min, const SVector2i& Max)
{
	if (!bIsValid)
		return;

	BottomRight.Clamp(Min, Max);
	TopLeft.Clamp(Min, BottomRight);
}
