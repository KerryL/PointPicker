// File:  pointPicker.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Object for picking points from images.

#ifndef POINT_PICKER_H_
#define POINT_PICKER_H_

// Standard C++ headers
#include <vector>

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

	enum DataExtractionMode
	{
		DataNone,
		DataXAxis,
		DataYAxis,
		DataCurve
	};

	void SetClipboardMode(const ClipboardMode& mode) { clipMode = mode; }
	void SetDataExtractionMode(const DataExtractionMode& mode) { dataMode = mode; }
	void SetCurveIndex(const unsigned int& curve) { curveIndex = curve; }

	void ResetXAxis();
	void ResetYAxis();
	void ResetCurveData(const unsigned int& curve);
	void Reset();

	struct Point
	{
		Point() {}

		Point(const double& xIn, const double& yIn)
		{
			x = xIn;
			y = yIn;
		}

		Point(const double& xIn, const double& yIn, const double& auxIn)
		{
			x = xIn;
			y = yIn;
			aux = auxIn;
		}

		double x, y, aux;
	};

	std::vector<std::vector<PointPicker::Point> > GetCurveData() const;

private:
	static double ScaleOrdinate(const double& value,
		const double& scale, const double& offset);

	ClipboardMode clipMode;
	DataExtractionMode dataMode;
	unsigned int curveIndex;

	std::vector<Point> xAxisPoints;
	std::vector<Point> yAxisPoints;
	std::vector<std::vector<Point> > curvePoints;

	void HandleClipboardMode(const double& x, const double& y) const;
	void HandleDataMode(const double& x, const double& y);
};

#endif// POINT_PICKER_H_
