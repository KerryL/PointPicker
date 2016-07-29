// File:  pointPicker.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Object for picking points from images.

#ifndef POINT_PICKER_H_
#define POINT_PICKER_H_

// Local forward declarations
class ImageObject;

class PointPicker
{
public:
	PointPicker();

	void AddPoint(const double& rawX, const double& rawY,
		const double& xScale, const double& yScale,
		const double& xOffset, const double& yOffset);
	inline void SetImageObject(const ImageObject *i) { image = i; }

private:
	const ImageObject *image;

	static double ScaleOrdinate(const double& value,
		const double& scale, const double& offset);
};

#endif// POINT_PICKER_H_
