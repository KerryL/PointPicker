// File:  pointPicker.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Object for picking points from images.

#ifndef POINT_PICKER_H_
#define POINT_PICKER_H_

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

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

	DataExtractionMode GetDataExtractionMode() const { return dataMode; }
	unsigned int GetCurveIndex() const { return curveIndex; }

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

	Point GetNewestPoint() const { return lastPoint; }

	std::vector<std::vector<PointPicker::Point> > GetCurveData() const;
	Point ScaleSinglePoint(const double& rawX, const double& rawY,
		const double& xScale, const double& yScale,
		const double& xOffset, const double& yOffset, double& x, double& y) const;

	wxString GetErrorString() const { return errorString; }

private:
	static double ScaleOrdinate(const double& value,
		const double& scale, const double& offset);

	ClipboardMode clipMode;
	DataExtractionMode dataMode;
	unsigned int curveIndex;

	mutable wxString errorString;

	std::vector<Point> xAxisPoints;
	std::vector<Point> yAxisPoints;
	std::vector<std::vector<Point> > curvePoints;

	Point lastPoint;

	void HandleClipboardMode(const double& x, const double& y) const;
	void HandleDataMode(const double& x, const double& y);

	struct AxisInfo
	{
		double angle;// [rad from horizontal]
		Point intercept;
		bool isLogarithmic;
		double scale;// [units/px]
		double zero;
	};

	static bool GetBestFitAxis(const std::vector<Point>& points, AxisInfo& info);
	static void GetBestAxisScale(const std::vector<Point>& points, AxisInfo& info);
	static Point GetNearestPoint(const Point& point, const AxisInfo& info);
	std::vector<std::vector<Point> > ScaleCurvePoints(const AxisInfo& xInfo, const AxisInfo& yInfo) const;
	static Point ScaleSinglePoint(const AxisInfo& xInfo, const AxisInfo& yInfo, const Point& point);

	static double DoLinearCalculation(const double& nx, const double& ny,
		const Point& p, const double& zero);
	static double DoLogarithmicCalculation(const double& nx, const double& ny,
		const Point& p, const double& zero);
};

#endif// POINT_PICKER_H_
