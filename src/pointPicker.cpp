// File:  pointPicker.cpp
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Object for picking points from images.

// Standard C++ headers
#include <cstdlib>
#include <cassert>
#include <cmath>

// wxWidgets headers
#include <wx/clipbrd.h>
#include <wx/textdlg.h>

// Eigen headers
#include <Eigen/Dense>

// Local headers
#include "pointPicker.h"

//==========================================================================
// Class:			PointPicker
// Function:		PointPicker
//
// Description:		Constructor for PointPicker class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PointPicker::PointPicker()
{
	clipMode = ClipNone;
	dataMode = DataNone;
	curveIndex = 0;
}

//==========================================================================
// Class:			PointPicker
// Function:		AddPoint
//
// Description:		Processes new point data.
//
// Input Arguments:
//		rawX	= const double&
//		rawY	= const double&
//		xScale	= const double&
//		yScale	= const double&
//		xOffset	= const double&
//		yOffset	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PointPicker::AddPoint(const double& rawX, const double& rawY,
	const double& xScale, const double& yScale,
	const double& xOffset, const double& yOffset)
{
	const double x(ScaleOrdinate(rawX, xScale, xOffset));
	const double y(ScaleOrdinate(rawY, yScale, yOffset));

	HandleClipboardMode(x, y);
	HandleDataMode(x, y);
}

//==========================================================================
// Class:			PointPicker
// Function:		HandleClipboardMode
//
// Description:		Handles necessary clipboard interaction.
//
// Input Arguments:
//		x	= const double&
//		y	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PointPicker::HandleClipboardMode(const double& x, const double& y) const
{
	if (clipMode != ClipNone && wxTheClipboard->Open())
	{
		if (clipMode == ClipX)
			wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(_T("%f"), x)));
		else if (clipMode == ClipY)
			wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(_T("%f"), y)));
		else if (clipMode == ClipBoth)
			wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(_T("%f\t%f"), x, y)));
		else
			assert(false);

		wxTheClipboard->Close();
	}
}

//==========================================================================
// Class:			PointPicker
// Function:		HandleDataMode
//
// Description:		Handles necessary data collection.
//
// Input Arguments:
//		x	= const double&
//		y	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PointPicker::HandleDataMode(const double& x, const double& y)
{
	if (dataMode == DataNone)
		return;

	if (dataMode == DataCurve)
	{
		while (curvePoints.size() <= curveIndex)
			curvePoints.push_back(std::vector<Point>(0));

		curvePoints[curveIndex].push_back(Point(x, y));
		return;
	}

	double tickValue;
	wxString valueString;
	int result(0);
	while (result != wxID_CANCEL)
	{
		wxTextEntryDialog dialog(NULL, _T("Specify axis value:"), _T("Axis Value Input"), valueString);
		result = dialog.ShowModal();
		if (result == wxID_OK)
		{
			valueString = dialog.GetValue();
			if (valueString.ToDouble(&tickValue))
				break;
		}
	}

	if (dataMode == DataXAxis)
		xAxisPoints.push_back(Point(x, y, tickValue));
	else// if (dataMode == DataYAxis)
		yAxisPoints.push_back(Point(x, y, tickValue));
}

//==========================================================================
// Class:			PointPicker
// Function:		ScaleOrdinate
//
// Description:		Scales the value according to the specified length and offset.
//
// Input Arguments:
//		value	= const double&
//		scale	= const double&
//		offset	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PointPicker::ScaleOrdinate(const double& value, const double& scale,
	const double& offset)
{
	return value * scale + offset;
}

//==========================================================================
// Class:			PointPicker
// Function:		ResetXAxis
//
// Description:		Resets the stored x-axis data.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PointPicker::ResetXAxis()
{
	xAxisPoints.clear();
}

//==========================================================================
// Class:			PointPicker
// Function:		ResetYAxis
//
// Description:		Resets the stored y-axis data.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PointPicker::ResetYAxis()
{
	yAxisPoints.clear();
}

//==========================================================================
// Class:			PointPicker
// Function:		ResetXAxis
//
// Description:		Resets the stored curve data.
//
// Input Arguments:
//		curve	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PointPicker::ResetCurveData(const unsigned int& curve)
{
	curvePoints.erase(curvePoints.begin() + curve);
}

//==========================================================================
// Class:			PointPicker
// Function:		Reset
//
// Description:		Resets all stored data.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PointPicker::Reset()
{
	ResetXAxis();
	ResetYAxis();

	curvePoints.clear();
	curveIndex = 0;
}

//==========================================================================
// Class:			PointPicker
// Function:		GetCurveData
//
// Description:		Returns processed curve data.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		std::vector<std::vector<PointPicker::Point> >
//
//==========================================================================
std::vector<std::vector<PointPicker::Point> > PointPicker::GetCurveData() const
{
	errorString.clear();

	AxisInfo xInfo, yInfo;
	if (!GetBestFitAxis(xAxisPoints, xInfo))
		errorString += _T("\nNot enough unique points to estimate X-axis");
	if (!GetBestFitAxis(yAxisPoints, yInfo))
		errorString += _T("\nNot enough unique points to estimate Y-axis");

	if (!errorString.empty())
		return std::vector<std::vector<Point> >(0);

	// TODO:  Warning if axes are not perpendicular?
	// TODO:  Warning/error if axes are parallel?

	GetBestAxisScale(xAxisPoints, xInfo);
	GetBestAxisScale(yAxisPoints, yInfo);

	return ScaleCurvePoints(xInfo, yInfo);
}

//==========================================================================
// Class:			PointPicker
// Function:		GetBestFitAxis
//
// Description:		Computes best fit line for specified axis data.
//
// Input Arguments:
//		points	= const std::vector<Point>&
//
// Output Arguments:
//		info	= AxisInfo&
//
// Return Value:
//		bool
//
//==========================================================================
bool PointPicker::GetBestFitAxis(const std::vector<Point>& points, AxisInfo& info)
{
	if (points.size() < 2)
		return false;

	Eigen::MatrixXd x(points.size(), 2);
	Eigen::VectorXd y(points.size());

	// To ensure the matrices we are using have good condition numbers, find the
	// ordinate with the largest range (i.e. are we solving for x-axis or y-axis?)
	double xMin(points[0].x);
	double xMax(points[0].x);
	double yMin(points[0].y);
	double yMax(points[0].y);
	unsigned int i;
	for (i = 1; i < points.size(); i++)
	{
		if (points[i].x < xMin)
			xMin = points[i].x;
		else if (points[i].x > xMax)
			xMax = points[i].x;

		if (points[i].y < yMin)
			yMin = points[i].y;
		else if (points[i].y > yMax)
			yMax = points[i].y;
	}

	bool invertSlope = fabs(xMax - xMin) < fabs(yMax - yMin);

	for (i = 0; i < points.size(); i++)
	{
		if (invertSlope)
		{
			x(i,0) = points[i].y;
			y(i) = points[i].x;
		}
		else
		{
			x(i,0) = points[i].x;
			y(i) = points[i].y;
		}
		x(i,1) = 1.0;
	}

	Eigen::VectorXd coefficients(x.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(y));
	if (invertSlope)
	{
		info.angle = atan(1.0 / coefficients(0));
		info.intercept.x = coefficients(1);
		info.intercept.y = 0.0;
	}
	else
	{
		info.angle = atan(coefficients(0));
		info.intercept.x = 0.0;
		info.intercept.y = coefficients(1);
	}

	return true;
}

//==========================================================================
// Class:			PointPicker
// Function:		GetBestAxisScale
//
// Description:		Finds axis scaling based on best fit line and user-specified
//					tick values.
//
// Input Arguments:
//		points	= const std::vector<Point>&
//
// Output Arguments:
//		info	= AxisInfo&
//
// Return Value:
//		None
//
//==========================================================================
void PointPicker::GetBestAxisScale(const std::vector<Point>& points, AxisInfo& info)
{
	// Find points on best-fit line closest to user-input points
	Eigen::MatrixXd model(points.size(), 2);
	Eigen::VectorXd linearValues(points.size());
	Eigen::VectorXd logarithmicValues(points.size());

	Point nearest;
	const double dx(cos(info.angle));
	const double dy(sin(info.angle));
	double distance;
	bool logIsOption(points[0].aux > 0.0);
	unsigned int i;
	for (i = 0; i < points.size(); i++)
	{
		nearest = GetNearestPoint(points[i], info);
		distance = (nearest.x - info.intercept.x) * dx + (nearest.y - info.intercept.y) * dy;
		model(i, 0) = distance;

		model(i, 1) = 1.0;
		linearValues(i) = points[i].aux;
		logarithmicValues(i) = log(points[i].aux);
		if (points[i].aux <= 0.0)
			logIsOption = false;
	}

	// Perform regressions for both linear and logarithmic models
	Eigen::VectorXd linearCoefficients(model.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(linearValues));
	Eigen::VectorXd logarithmicCoefficients(model.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(logarithmicValues));

	// Choose result based on lowest error
	Eigen::VectorXd linearError(model * linearCoefficients - linearValues);
	Eigen::VectorXd logarithmicError(model * logarithmicCoefficients - logarithmicValues);
	if (linearError.dot(linearError) < logarithmicError.dot(logarithmicError) || !logIsOption)
	{
		info.isLogarithmic = false;
		info.scale = linearCoefficients(0);
		info.zero = linearCoefficients(1);
	}
	else
	{
		info.isLogarithmic = true;
		info.scale = logarithmicCoefficients(0);
		info.zero = logarithmicCoefficients(1);
	}
}

//==========================================================================
// Class:			PointPicker
// Function:		GetNearestPoint
//
// Description:		Returns the point on the line closest to the specified point.
//					Implementation is based on find the intersection of two
//					parametric lines given a point on the line and a direction
//					vector.  The point on the axis is the intercept point and
//					the point on the perpendicular line is the argument "point."
//					The slope of the second line is perpendicular to the axis
//					(to ensure minimum distance from point to axis), so we only
//					need one value of nx and ny to define both lines.
//
// Input Arguments:
//		point	= const Point&
//		info	= AxisInfo&
//
// Output Arguments:
//		None
//
// Return Value:
//		Point
//
//==========================================================================
PointPicker::Point PointPicker::GetNearestPoint(const Point& point, const AxisInfo& info)
{
	Point p;
	double t;
	const double nx(cos(info.angle));
	const double ny(sin(info.angle));
	if (fabs(nx) > fabs(ny))
	{
		t = (info.intercept.y - point.y - ny / nx * (info.intercept.x - point.x))
			/ (nx + ny * ny / nx);
		p.x = point.x - t * ny;
		p.y = point.y + t * nx;
	}
	else
	{
		t = (point.y - info.intercept.y - nx / ny * (point.x - info.intercept.x))
			/ (ny + nx * nx / ny);
		p.x = info.intercept.x + t * nx;
		p.y = info.intercept.y + t * ny;
	}

	return p;
}

//==========================================================================
// Class:			PointPicker
// Function:		GetCurveData
//
// Description:		Returns processed curve data.
//
// Input Arguments:
//		points	= const std::vector<Point>&
//
// Output Arguments:
//		info	= AxisInfo&
//
// Return Value:
//		None
//
//==========================================================================
std::vector<std::vector<PointPicker::Point> > PointPicker::ScaleCurvePoints(
	const AxisInfo& xInfo, const AxisInfo& yInfo) const
{
	// The two axes form a basis for the plot space.  So both the x and y
	// components of a point contribute to both the x and y values of the
	// scaled point.
	const double nxX(cos(xInfo.angle) * xInfo.scale);
	const double nyX(sin(xInfo.angle) * xInfo.scale);
	const double nxY(cos(yInfo.angle) * yInfo.scale);
	const double nyY(sin(yInfo.angle) * yInfo.scale);

	std::vector<std::vector<Point> > scaledCurves(curvePoints);
	unsigned int i, j;
	for (i = 0; i < scaledCurves.size(); i++)
	{
		if (xInfo.isLogarithmic)
		{
			for (j = 0; j < scaledCurves[i].size(); j++)
				scaledCurves[i][j].x = pow(10.0, scaledCurves[i][j].x * nxX + scaledCurves[i][j].y * nyX + xInfo.zero);
		}
		else
		{
			for (j = 0; j < scaledCurves[i].size(); j++)
				scaledCurves[i][j].x = scaledCurves[i][j].x * nxX + scaledCurves[i][j].y * nyX + xInfo.zero;
		}

		if (yInfo.isLogarithmic)
		{
			for (j = 0; j < scaledCurves[i].size(); j++)
				scaledCurves[i][j].y = pow(10.0, scaledCurves[i][j].x * nxY + scaledCurves[i][j].y * nyY + yInfo.zero);
		}
		else
		{
			for (j = 0; j < scaledCurves[i].size(); j++)
				scaledCurves[i][j].y = scaledCurves[i][j].x * nxY + scaledCurves[i][j].y * nyY + yInfo.zero;
		}
	}

	return scaledCurves;
}

