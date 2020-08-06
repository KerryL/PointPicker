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

// Eigen headers
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4018)// signed/unsigned mismatch
#pragma warning(disable:4456)// declaration hides previous local declaration
#pragma warning(disable:4714)// function marked as __forceinline not inlined
#pragma warning(disable:4800)// forcing value to bool 'true' or 'false'
#endif
#include <Eigen/Eigen>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

class PointPicker
{
public:
	PointPicker();

	void AddPoint(const double& rawX, const double& rawY,
		const double& xScale, const double& yScale,
		const double& xOffset, const double& yOffset);

	enum class ClipboardMode
	{
		None,
		X,
		Y,
		Both
	};

	enum class DataExtractionMode
	{
		None,
		References,
		Curve
	};

	void SetClipboardMode(const ClipboardMode& mode) { clipMode = mode; }
	void SetDataExtractionMode(const DataExtractionMode& mode) { dataMode = mode; }
	void SetCurveIndex(const unsigned int& curve) { curveIndex = curve; }

	DataExtractionMode GetDataExtractionMode() const { return dataMode; }
	unsigned int GetCurveIndex() const { return curveIndex; }

	void RemoveReference(const unsigned int& i);
	void ResetReferences();
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

		double x, y;

		Point& operator=(const Point& p)
		{
			if (&p == this)
				return *this;

			x = p.x;
			y = p.y;

			return *this;
		}
	};

	Point GetNewestPoint() const { return lastPoint; }
	std::vector<Point> GetReferences() const;

	std::vector<std::vector<PointPicker::Point>> GetCurveData() const;
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
	void ResetErrorString() const;

	struct ReferencePair
	{
		ReferencePair() {}
		ReferencePair(const Point& i, const Point& v)
		{
			imageCoords = i;
			valueCoords = v;
		}

		Point imageCoords;
		Point valueCoords;
	};

	std::vector<ReferencePair> referencePoints;
	std::vector<std::vector<Point>> curvePoints;

	Point lastPoint;

	void HandleClipboardMode(const double& x, const double& y) const;
	void HandleDataMode(const double& x, const double& y);
	void UpdateTransformation();

	Eigen::Matrix3d transformationMatrix;
	bool xIsLogarithmic;
	bool yIsLogarithmic;

	Point ScalePoint(const Point& imagePointIn) const;

	enum class PlotScaling
	{
		Linear,
		SemiLogX,
		SemiLogY,
		LogLog
	};

	static Eigen::Matrix3d ComputeTransformation(const std::vector<ReferencePair>& pairs,
		const PlotScaling& scaling, double& error);
};

#endif// POINT_PICKER_H_
