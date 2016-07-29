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

	inline void SetImageObject(const ImageObject *i) { image = i; }

private:
	const ImageObject *image;
};

#endif// POINT_PICKER_H_
