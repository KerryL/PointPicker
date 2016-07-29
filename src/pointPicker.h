// File:  pointPicker.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Object for picking points from images.

#ifndef POINT_PICKER_H_
#define POINT_PICKER_H_

class PointPicker
{
public:
	PointPicker();

	void AddPoint(const double& rawX, const double& rawY,
		const double& xScale, const double& yScale,
		const double& xOffset, const double& yOffset);

	enum ClipboardMode
	{
		ClipNone,
		ClipX,
		ClipY,
		ClipBoth
	};

	void SetClipboardMode(const ClipboardMode& mode) { clipMode = mode; }

private:
	static double ScaleOrdinate(const double& value,
		const double& scale, const double& offset);

	ClipboardMode clipMode;

	void HandleClipboardMode(const double& x, const double& y) const;
};

#endif// POINT_PICKER_H_
