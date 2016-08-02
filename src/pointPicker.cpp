// File:  pointPicker.cpp
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Object for picking points from images.

// Standard C++ headers
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <limits>

// wxWidgets headers
#include <wx/clipbrd.h>

// Eigen headers
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4018)// signed/unsigned mismatch
#pragma warning(disable:4456)// declaration hides previous local declaration
#pragma warning(disable:4714)// function marked as __forceinline not inlined
#pragma warning(disable:4800)// forcing value to bool 'true' or 'false'
#endif
#include <Eigen/Dense>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// Local headers
#include "pointPicker.h"
#include "pointEntryDialog.h"

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
	ResetErrorString();
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

		lastPoint.x = x;
		lastPoint.y = y;
		curvePoints[curveIndex].push_back(lastPoint);
		return;
	}

	PointEntryDialog dialog(NULL, wxID_ANY, _T("Coordinate Input"));
	if (dialog.ShowModal() == wxID_CANCEL)
		return;

	if (dataMode == DataReferences)
	{
		referencePoints.push_back(ReferencePair(Point(x, y), dialog.GetPoint()));
		UpdateTransformation();
	}
}

//==========================================================================
// Class:			PointPicker
// Function:		UpdateTransformation
//
// Description:		Updates the transformation matrix according to all stored
//					reference points.
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
void PointPicker::UpdateTransformation()
{
	if (referencePoints.size() < 4)
	{
		ResetErrorString();
		return;
	}
	else
		errorString.clear();

	// Use the Direct Linear Transform approach to solve for the unknown
	// projective transforms.  Solve for the transforms assuming all different
	// combination of linear and logarithmic scales in order to determine
	// which combination provides the lowest error.
	double xLinYLinError;
	Eigen::MatrixXd xLinYLinTransformation(ComputeTransformation(referencePoints, xLinYLinError));
	if (referencePoints.size() == 4)// Not enough information to determine if scaling is logarithmic
	{
		transformationMatrix = xLinYLinTransformation;
		xIsLogarithmic = false;
		yIsLogarithmic = false;
		return;
	}

	std::vector<ReferencePair> xLinYLogReferencePoints(referencePoints);
	std::vector<ReferencePair> xLogYLinReferencePoints(referencePoints);
	std::vector<ReferencePair> xLogYLogReferencePoints(referencePoints);

	bool xLogIsOption(true);
	bool yLogIsOption(true);

	unsigned int i;
	for (i = 0; i < referencePoints.size(); i++)
	{
		if (referencePoints[i].valueCoords.x <= 0.0)
			xLogIsOption = false;
		if (referencePoints[i].valueCoords.y <= 0.0)
			yLogIsOption = false;

		if (yLogIsOption)
			xLinYLogReferencePoints[i].valueCoords.y = log10(referencePoints[i].valueCoords.y);
		if (xLogIsOption)
			xLogYLinReferencePoints[i].valueCoords.x = log10(referencePoints[i].valueCoords.x);

		xLogYLogReferencePoints[i].valueCoords.x = xLogYLinReferencePoints[i].valueCoords.x;
		xLogYLogReferencePoints[i].valueCoords.y = xLinYLogReferencePoints[i].valueCoords.y;
	}

	double xLinYLogError(std::numeric_limits<double>::max());
	double xLogYLinError(std::numeric_limits<double>::max());
	double xLogYLogError(std::numeric_limits<double>::max());

	Eigen::MatrixXd xLinYLogTransformation;
	Eigen::MatrixXd xLogYLinTransformation;
	Eigen::MatrixXd xLogYLogTransformation;

	if (yLogIsOption)
	{
		xLinYLogTransformation = ComputeTransformation(xLinYLogReferencePoints, xLinYLogError);
		if (xLogIsOption)
			xLogYLogTransformation = ComputeTransformation(xLogYLogReferencePoints, xLogYLogError);
	}

	if (xLogIsOption)
		xLogYLinTransformation = ComputeTransformation(xLogYLinReferencePoints, xLogYLinError);

	if (xLinYLinError < xLinYLogError &&
		xLinYLinError < xLogYLinError &&
		xLinYLinError < xLogYLogError)
	{
		transformationMatrix = xLinYLinTransformation;
		xIsLogarithmic = false;
		yIsLogarithmic = false;
	}
	else if (xLinYLogError < xLogYLinError &&
		xLinYLogError < xLogYLogError)
	{
		transformationMatrix = xLinYLogTransformation;
		xIsLogarithmic = false;
		yIsLogarithmic = true;
	}
	else if (xLogYLinError < xLogYLogError)
	{
		transformationMatrix = xLogYLinTransformation;
		xIsLogarithmic = true;
		yIsLogarithmic = false;
	}
	else
	{
		transformationMatrix = xLogYLogTransformation;
		xIsLogarithmic = true;
		yIsLogarithmic = true;
	}
}

//==========================================================================
// Class:			PointPicker
// Function:		ComputeTransformation
//
// Description:		Computes the transformation for the specified correspondances.
//
// Input Arguments:
//		paris	= const std::vector<ReferencePair>&
//
// Output Arguments:
//		error	= double&
//
// Return Value:
//		Eigen::Matrix3d
//
//==========================================================================
Eigen::Matrix3d PointPicker::ComputeTransformation(
	const std::vector<ReferencePair>& pairs, double& error)
{
	Eigen::Matrix<double, Eigen::Dynamic, 9> model(2 * pairs.size(), 9);

	// Set up columns that will all have the same value
	model.block(0,2,pairs.size(),1).setOnes();
	model.block(0,3,pairs.size(),3).setZero();
	model.block(pairs.size(),0,pairs.size(),3).setZero();
	model.block(pairs.size(),5,pairs.size(),1).setOnes();

	unsigned int i;
	for (i = 0; i < pairs.size(); i++)
	{
		// X-ordinate
		model(i,0) = pairs[i].imageCoords.x;
		model(i,1) = pairs[i].imageCoords.y;

		model(i,6) = -pairs[i].valueCoords.x * pairs[i].imageCoords.x;
		model(i,7) = -pairs[i].valueCoords.x * pairs[i].imageCoords.y;
		model(i,8) = -pairs[i].valueCoords.x;

		// Y-ordinate
		model(i + pairs.size(),3) = pairs[i].imageCoords.x;
		model(i + pairs.size(),4) = pairs[i].imageCoords.y;

		model(i + pairs.size(),6) = -pairs[i].valueCoords.y * pairs[i].imageCoords.x;
		model(i + pairs.size(),7) = -pairs[i].valueCoords.y * pairs[i].imageCoords.y;
		model(i + pairs.size(),8) = -pairs[i].valueCoords.y;
	}

	Eigen::JacobiSVD<Eigen::Matrix<double, Eigen::Dynamic, 9> > svd(model, Eigen::ComputeFullV);
	Eigen::Matrix<double, 9, 1> nullspace(svd.matrixV().col(8));
	Eigen::Matrix3d transform;
	transform.row(0) = nullspace.head<3>();
	transform.row(1) = nullspace.segment<3>(3);
	transform.row(2) = nullspace.tail<3>();

	Eigen::VectorXd errorVector(model * nullspace);
	error = errorVector.dot(errorVector);

	return transform;
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
// Function:		ResetReferences
//
// Description:		Resets the stored reference data.
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
void PointPicker::ResetReferences()
{
	referencePoints.clear();
	ResetErrorString();
}

//==========================================================================
// Class:			PointPicker
// Function:		ResetCurveData
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
	ResetReferences();

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
	if (!errorString.empty())
		return std::vector<std::vector<Point> >(0);

	std::vector<std::vector<Point> > data(curvePoints);
	unsigned int i, j;
	for (i = 0; i < curvePoints.size(); i++)
	{
		for (j = 0; j < curvePoints[i].size(); j++)
			data[i][j] = ScalePoint(curvePoints[i][j]);
	}

	return data;
}

//==========================================================================
// Class:			PointPicker
// Function:		ScalePoint
//
// Description:		Converts the specified point from image to plot coordinates.
//
// Input Arguments:
//		imagePointIn	= const Point&
//
// Output Arguments:
//		None
//
// Return Value:
//		PointPicker::Point
//
//==========================================================================
PointPicker::Point PointPicker::ScalePoint(const Point& imagePointIn) const
{
	Eigen::Vector3d imagePoint(imagePointIn.x, imagePointIn.y, 1.0);
	Eigen::Vector3d plotPoint(transformationMatrix * imagePoint);

	Point p(plotPoint(0) / plotPoint(2), plotPoint(1) / plotPoint(2));

	if (xIsLogarithmic)
		p.x = pow(10.0, p.x);

	if (yIsLogarithmic)
		p.y = pow(10.0, p.y);

	return p;
}

//==========================================================================
// Class:			PointPicker
// Function:		ScaleSinglePoint
//
// Description:		Processes data for single point.
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
//		x		= double&
//		y		= double&
//
// Return Value:
//		Point
//
//==========================================================================
PointPicker::Point PointPicker::ScaleSinglePoint(const double& rawX, const double& rawY,
	const double& xScale, const double& yScale,
	const double& xOffset, const double& yOffset, double& x, double& y) const
{
	x = ScaleOrdinate(rawX, xScale, xOffset);
	y = ScaleOrdinate(rawY, yScale, yOffset);

	if (!errorString.empty())
		return Point(0.0, 0.0);

	return ScalePoint(Point(x, y));
}

//==========================================================================
// Class:			PointPicker
// Function:		ResetErrorString
//
// Description:		Resets the error string.
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
void PointPicker::ResetErrorString() const
{
	errorString = _T("Not enough reference points");
}
