#ifndef CUIMATH_H
#define CUIMATH_H

template<class T>
struct TVector2D
{
public:
	T X;
	T Y;

	TVector2D() : X(0), Y(0) {}
	TVector2D(T InXY) : X(InXY), Y(InXY) {}
	TVector2D(T InX, T InY) : X(InX), Y(InY) {}

	TVector2D<T>& operator+=(const TVector2D<T>& rhs) { X += rhs.X; Y += rhs.Y;	return *this; }
	TVector2D<T> operator+(const TVector2D<T>& rhs) const { TVector2D<T> Result = *this; Result += rhs; return Result; }
	TVector2D<T>& operator-=(const TVector2D<T>& rhs) { X -= rhs.X; Y -= rhs.Y;	return *this; }
	TVector2D<T> operator-(const TVector2D<T>& rhs) const { TVector2D<T> Result = *this; Result -= rhs; return Result; }
	bool operator!=(const TVector2D<T>& rhs) const { return X != rhs.X || Y != rhs.Y; }

	void Clamp(const TVector2D<T>& Min, const TVector2D<T>& Max)
	{
		X = std::max(std::min(X, Max.X), Min.X);
		Y = std::max(std::min(Y, Max.Y), Min.Y);
	}
};
typedef TVector2D<int> SVector2i;
typedef TVector2D<float> SVector2f;

struct SRect
{
	SVector2i TopLeft;
	SVector2i BottomRight;
	bool bIsValid;

	SRect() : bIsValid(false) {}
	SRect(const SVector2i& InTopLeft, const SVector2i& InBottomRight) : TopLeft(InTopLeft), BottomRight(InBottomRight), bIsValid(true) {}

	bool operator!=(const SRect& rhs) const { return TopLeft != rhs.TopLeft || BottomRight != rhs.BottomRight; }

	void ExtendTo(const SVector2i& InPoint);
	void ExtendTo(const SRect& RhsRect);

	bool Intersercts(const SRect& RhsRect) const;
	bool Contains(const SVector2i& Point) const;
	void Clamp(const SVector2i& Min, const SVector2i& Max);
};

#endif