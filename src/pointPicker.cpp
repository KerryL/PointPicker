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
#include <iostream>// TODO:  Remove
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

	GetBestAxisScale(xAxisPoints, xInfo);
	GetBestAxisScale(yAxisPoints, yInfo);

std::cout << "xScale = " << xInfo.scale << " unit/px" << std::endl;
std::cout << "yScale = " << yInfo.scale << " unit/px" << std::endl;
std::cout << "xZero = " << xInfo.zero << std::endl;
std::cout << "yZero = " << yInfo.zero << std::endl;
std::cout << "x is log = " << (int)xInfo.isLogarithmic << std::endl;
std::cout << "y is log = " << (int)yInfo.isLogarithmic << std::endl;

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
	Eigen::MatrixXd linearModel(points.size(), 2);
	Eigen::MatrixXd logarithmicModel(points.size(), 2);
	Eigen::VectorXd values(points.size());

	Point reference(GetNearestPoint(points[0], info));
	Point nearest;
	const double dx(cos(info.angle));
	const double dy(sin(info.angle));
	double distance;
	bool logIsOption(points[0].aux > 0.0);
	unsigned int i;
	for (i = 1; i < points.size(); i++)
	{
		nearest = GetNearestPoint(points[i], info);
		distance = (nearest.x - reference.x) * dx + (nearest.y - reference.y) * dy;
		linearModel(i, 0) = distance;
		logarithmicModel(i, 0) = pow(10.0, distance);

		linearModel(i, 1) = 1.0;
		logarithmicModel(i, 1) = 1.0;
		values(i) = points[i].aux - points[0].aux;
		if (points[i].aux <= 0.0)
			logIsOption = false;
	}

	// Perform regressions for both linear and logarithmic models
	Eigen::VectorXd linearCoefficients(linearModel.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(values));
	Eigen::VectorXd logarithmicCoefficients(logarithmicModel.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(values));

	// Choose result based on lowest error
	Eigen::VectorXd linearError(linearModel * linearCoefficients - values);
	Eigen::VectorXd logarithmicError(logarithmicModel * logarithmicCoefficients - values);
std::cout << "lin coef = " << linearCoefficients << std::endl;
std::cout << "log coef = " << logarithmicCoefficients << std::endl;
std::cout << "lin error = " << linearError.dot(linearError) << std::endl;
std::cout << "log error = " << logarithmicError.dot(logarithmicError) << std::endl;
// TODO:  Issue here is that log error is sometimes NaN, so this test returns false
	if (linearError.dot(linearError) < logarithmicError.dot(logarithmicError) && logIsOption)
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
//
// Input Arguments:
//		point	= const Point&
//		info	= AxisInfo&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PointPicker::Point PointPicker::GetNearestPoint(const Point& point, const AxisInfo& info)
{
	return point;
/*	Point p;

	// TODO:  Improve this.  Either find an algorithm that is numerically superior, or clean this up to make it easier to follow

	// Points point and p2 are on the line perpendicular to the axis
	// Point l is the axis (as is info.intercept)
	Point p2, l;
	p2.x = point.x + sin(info.angle);
	p2.y = point.y + cos(info.angle);
	l.x = info.intercept.x + cos(info.angle);
	l.y = info.intercept.y + sin(info.angle);
	p.x = ((point.x * p2.y - point.y * p2.x) * (info.intercept.x - l.x)
		- (point.x - p2.x) * (info.intercept.x * l.y - info.intercept.y * l.x))
		/ ((point.x - p2.y) * (info.intercept.y - l.y)
		- (point.y - p2.y) * (info.intercept.x - l.x));
	p.y = ((point.x * p2.y - point.y * p2.x) * (info.intercept.y - l.y)
		- (point.y - p2.y) * (info.intercept.x * l.y - info.intercept.y * l.x))
		/ ((point.x - p2.y) * (info.intercept.y - l.y)
		- (point.y - p2.y) * (info.intercept.x - l.x));

	return p;*/
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
	std::vector<std::vector<Point> > scaledCurves(curvePoints);
	unsigned int i, j;
	for (i = 0; i < scaledCurves.size(); i++)
	{
		for (j = 0; j < scaledCurves[i].size(); j++)
		{
			// TODO:  Implement
		}
	}

	return scaledCurves;
}

