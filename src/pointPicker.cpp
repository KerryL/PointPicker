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

	// Find the matrices that map source image points (in pixels) to basis
	// vectors (sourceTransformation) and plot points (in user-specified
	// values) to basis vectors (plotTransformation)
	Eigen::MatrixXd sourceTransformation(3, referencePoints.size() - 1);
	Eigen::MatrixXd plotTransformation(3, referencePoints.size() - 1);
	Eigen::Vector3d sourceKnowns;

	if (referencePoints.size() == 4)
	{
		unsigned int i;
		for (i = 0; i < referencePoints.size() - 1; i++)
		{
			sourceTransformation(0, i) = referencePoints[i].imageCoords.x;
			sourceTransformation(1, i) = referencePoints[i].imageCoords.y;
			sourceTransformation(2, i) = 1.0;

			// TODO:  Need to handle log values, too, and combinations of x log, y lin, etc.
			plotTransformation(0, i) = referencePoints[i].valueCoords.x;
			plotTransformation(1, i) = referencePoints[i].valueCoords.y;
			plotTransformation(2, i) = 1.0;
		}

		sourceKnowns(0) = referencePoints.back().imageCoords.x;
		sourceKnowns(1) = referencePoints.back().imageCoords.y;
		sourceKnowns(2) = 1.0;

		Eigen::Vector3d plotKnowns;
		plotKnowns(0) = referencePoints.back().valueCoords.x;
		plotKnowns(1) = referencePoints.back().valueCoords.y;
		plotKnowns(2) = 1.0;

		Eigen::VectorXd sourceScaleVector(sourceTransformation.jacobiSvd(
			Eigen::ComputeThinU | Eigen::ComputeThinV).solve(sourceKnowns));
		Eigen::VectorXd plotScaleVector(plotTransformation.jacobiSvd(
			Eigen::ComputeThinU | Eigen::ComputeThinV).solve(plotKnowns));

		// TODO:  Are we sure this check is good?
		// Check for zero values in the scale vectors
		if ((sourceScaleVector.array() == 0.0).any() ||
			(plotScaleVector.array() == 0.0).any())
		{
			errorString = _T("Reference points must span image and plot spaces");
			return;
		}

		for (i = 0; i < (unsigned int)sourceTransformation.cols(); i++)
		{
			sourceTransformation.col(i) *= sourceScaleVector(i);
			plotTransformation.col(i) *= plotScaleVector(i);
		}

		transformationMatrix = plotTransformation * PseudoInverse(sourceTransformation);

		// Not enough point provided to make a dermination about logarithmic scaling
		xIsLogarithmic = false;
		yIsLogarithmic = false;
		return;
	}
	
	Eigen::Vector3d xLinYLinPlotKnowns;
	Eigen::Vector3d xLinYLogPlotKnowns;
	Eigen::Vector3d xLogYLinPlotKnowns;
	Eigen::Vector3d xLogYLogPlotKnowns;

	unsigned int i;
	for (i = 0; i < referencePoints.size() - 1; i++)
	{
		sourceTransformation(0, i) = referencePoints[i].imageCoords.x;
		sourceTransformation(1, i) = referencePoints[i].imageCoords.y;
		sourceTransformation(2, i) = 1.0;

		// TODO:  Need to handle log values, too, and combinations of x log, y lin, etc.
		plotTransformation(0, i) = referencePoints[i].valueCoords.x;
		plotTransformation(1, i) = referencePoints[i].valueCoords.y;
		plotTransformation(2, i) = 1.0;
	}

	sourceKnowns(0) = referencePoints.back().imageCoords.x;
	sourceKnowns(1) = referencePoints.back().imageCoords.y;
	sourceKnowns(2) = 1.0;

	xLinYLinPlotKnowns(0) = referencePoints.back().valueCoords.x;
	xLinYLinPlotKnowns(1) = referencePoints.back().valueCoords.y;
	xLinYLinPlotKnowns(2) = 1.0;

	xLinYLogPlotKnowns(0) = referencePoints.back().valueCoords.x;
	xLinYLogPlotKnowns(1) = log10(referencePoints.back().valueCoords.y);
	xLinYLogPlotKnowns(2) = 1.0;

	xLogYLinPlotKnowns(0) = log10(referencePoints.back().valueCoords.x);
	xLogYLinPlotKnowns(1) = referencePoints.back().valueCoords.y;
	xLogYLinPlotKnowns(2) = 1.0;

	xLogYLogPlotKnowns(0) = log10(referencePoints.back().valueCoords.x);
	xLogYLogPlotKnowns(1) = log10(referencePoints.back().valueCoords.y);
	xLogYLogPlotKnowns(2) = 1.0;

	Eigen::VectorXd sourceScaleVector(sourceTransformation.jacobiSvd(
		Eigen::ComputeThinU | Eigen::ComputeThinV).solve(sourceKnowns));
	Eigen::VectorXd xLinYLinPlotScaleVector(plotTransformation.jacobiSvd(
		Eigen::ComputeThinU | Eigen::ComputeThinV).solve(xLinYLinPlotKnowns));
	Eigen::VectorXd xLinYLogPlotScaleVector(plotTransformation.jacobiSvd(
		Eigen::ComputeThinU | Eigen::ComputeThinV).solve(xLinYLogPlotKnowns));
	Eigen::VectorXd xLogYLinPlotScaleVector(plotTransformation.jacobiSvd(
		Eigen::ComputeThinU | Eigen::ComputeThinV).solve(xLogYLinPlotKnowns));
	Eigen::VectorXd xLogYLogPlotScaleVector(plotTransformation.jacobiSvd(
		Eigen::ComputeThinU | Eigen::ComputeThinV).solve(xLogYLogPlotKnowns));

	// TODO:  Are we sure this check is good?
	// Check for zero values in the scale vectors
	if ((sourceScaleVector.array() == 0.0).any() ||
		(xLinYLinPlotScaleVector.array() == 0.0).any() ||
		(xLinYLogPlotScaleVector.array() == 0.0).any() ||
		(xLogYLinPlotScaleVector.array() == 0.0).any() ||
		(xLogYLogPlotScaleVector.array() == 0.0).any())
	{
		errorString = _T("Reference points must span image and plot spaces");
		return;
	}

	Eigen::MatrixXd xLinYLinPlotTransformation(plotTransformation);
	Eigen::MatrixXd xLinYLogPlotTransformation(plotTransformation);
	Eigen::MatrixXd xLogYLinPlotTransformation(plotTransformation);
	Eigen::MatrixXd xLogYLogPlotTransformation(plotTransformation);
	for (i = 0; i < (unsigned int)sourceTransformation.cols(); i++)
	{
		sourceTransformation.col(i) *= sourceScaleVector(i);
		xLinYLinPlotTransformation.col(i) *= xLinYLinPlotScaleVector(i);
		xLinYLogPlotTransformation.col(i) *= xLinYLogPlotScaleVector(i);
		xLogYLinPlotTransformation.col(i) *= xLogYLinPlotScaleVector(i);
		xLogYLogPlotTransformation.col(i) *= xLogYLogPlotScaleVector(i);
	}

	Eigen::MatrixXd sourceInverse(PseudoInverse(sourceTransformation));
	xLinYLinPlotTransformation *= sourceInverse;
	xLinYLogPlotTransformation *= sourceInverse;
	xLogYLinPlotTransformation *= sourceInverse;
	xLogYLogPlotTransformation *= sourceInverse;

	Eigen::VectorXd xLinYLinError(xLinYLinPlotTransformation * sourceKnowns - xLinYLinPlotKnowns);
	Eigen::VectorXd xLinYLogError(xLinYLogPlotTransformation * sourceKnowns - xLinYLogPlotKnowns);
	Eigen::VectorXd xLogYLinError(xLogYLinPlotTransformation * sourceKnowns - xLogYLinPlotKnowns);
	Eigen::VectorXd xLogYLogError(xLogYLogPlotTransformation * sourceKnowns - xLogYLogPlotKnowns);

	const double xLinYLinErrorMag(xLinYLinError.dot(xLinYLinError));
	const double xLinYLogErrorMag(xLinYLogError.dot(xLinYLogError));
	const double xLogYLinErrorMag(xLogYLinError.dot(xLogYLinError));
	const double xLogYLogErrorMag(xLogYLogError.dot(xLogYLogError));

	if (xLinYLinErrorMag < xLinYLogErrorMag &&
		xLinYLinErrorMag < xLogYLinErrorMag &&
		xLinYLinErrorMag < xLogYLogErrorMag)
	{
		transformationMatrix = xLinYLinPlotTransformation;
		xIsLogarithmic = false;
		yIsLogarithmic = false;
	}
	else if (xLinYLogErrorMag < xLogYLinErrorMag &&
		xLinYLogErrorMag < xLogYLogErrorMag)
	{
		transformationMatrix = xLinYLogPlotTransformation;
		xIsLogarithmic = false;
		yIsLogarithmic = true;
	}
	else if (xLogYLinErrorMag < xLogYLogErrorMag)
	{
		transformationMatrix = xLogYLinPlotTransformation;
		xIsLogarithmic = true;
		yIsLogarithmic = false;
	}
	else
	{
		transformationMatrix = xLogYLogPlotTransformation;
		xIsLogarithmic = true;
		yIsLogarithmic = true;
	}
}

//==========================================================================
// Class:			PointPicker
// Function:		PseudoInverse
//
// Description:		Computes the Moore-Penrose pseudo inverse.
//
// Input Arguments:
//		m	= const Eigen::MatrixXd&
//
// Output Arguments:
//		None
//
// Return Value:
//		Eigen::MatrixXd
//
//==========================================================================
Eigen::MatrixXd PointPicker::PseudoInverse(const Eigen::MatrixXd& m)
{
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(m, Eigen::ComputeFullU | Eigen::ComputeFullV);
	// Sometimes we need to pad the singular value matrix with zeros in order to make the dimensions match
	// Ensure that it would fulfil the requirement to rebuild the original matrix:  m = U * s * V^T
	Eigen::MatrixXd sInv(Eigen::MatrixXd::Zero(svd.matrixV().rows(), svd.matrixU().rows()));
	int i;
	for (i = 0; i < svd.singularValues().count(); i++)
		sInv(i,i) = 1.0 / svd.singularValues()(i);
	return svd.matrixV() * sInv * svd.matrixU().transpose();
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
