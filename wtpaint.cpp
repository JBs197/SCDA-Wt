#include "wtpaint.h"

void WTPAINT::areaClicked(int index)
{
	indexAreaPrevious = indexAreaSel;
	indexAreaSel = index;
	nameAreaSel = areaNames[index];	
	m_map.lock();
	commWidget.push_back(nameAreaSel);
	m_map.unlock();
	update(Wt::PaintFlag::Update);
}
void WTPAINT::clearAreas()
{
	auto listAreas = Wt::WPaintedWidget::areas();
	for (int ii = 0; ii < listAreas.size(); ii++)
	{
		Wt::WPaintedWidget::removeArea(listAreas[ii]);
	}
}
vector<string> WTPAINT::delinearizeTitle(string& linearTitle)
{
	vector<string> title;
	string temp;
	size_t pos1 = 0;
	size_t pos2 = linearTitle.find('@');
	while (pos2 < linearTitle.size())
	{
		temp = linearTitle.substr(pos1, pos2 - pos1);
		title.push_back(temp);
		pos1 = pos2 + 1;
		pos2 = linearTitle.find('@', pos1);
	}
	temp = linearTitle.substr(pos1);
	title.push_back(temp);
	return title;
}
void WTPAINT::drawMap(vector<vector<Wt::WPointF>>& Areas, vector<string>& sidAreaNames, vector<double>& regionData)
{
	areas = Areas;  // Form [parent, children].
	dataName = delinearizeTitle(sidAreaNames.back());
	sidAreaNames.pop_back();
	areaNames.assign(sidAreaNames.begin() + 1, sidAreaNames.end()); // Form [parent, children].
	areaData = regionData;
	areaDataKids.assign(regionData.begin() + 1, regionData.end());
	updateAreaColour();
	indexAreaSel = -1;
	indexAreaPrevious = -1;
	update(); 
}
void WTPAINT::err(string func)
{
	jf.err(func);
}
vector<vector<int>> WTPAINT::getFrame(vector<Wt::WPointF>& path)
{
	vector<vector<double>> TLBRd(2, vector<double>(2));
	TLBRd[0][0] = path[0].x();
	TLBRd[1][0] = path[0].x();
	TLBRd[0][1] = path[0].y();
	TLBRd[1][1] = path[0].y();
	for (int ii = 1; ii < path.size(); ii++)
	{
		if (path[ii].x() < TLBRd[0][0]) { TLBRd[0][0] = path[ii].x(); }
		else if (path[ii].x() > TLBRd[1][0]) { TLBRd[1][0] = path[ii].x(); }
		if (path[ii].y() < TLBRd[0][1]) { TLBRd[0][1] = path[ii].y(); }
		else if (path[ii].y() > TLBRd[1][1]) { TLBRd[1][1] = path[ii].y(); }
	}
	vector<vector<int>> TLBR;
	jf.toInt(TLBRd, TLBR);
	return TLBR;
}
vector<int> WTPAINT::getScaleValues(int numTicks)
{
	vector<int> ticks(numTicks);
	vector<int> indexMinMax = jf.minMax(areaData);
	double min = areaData[indexMinMax[0]], dTemp;
	double max = areaData[indexMinMax[1]];
	double bandWidth = (max - min) / (double)(numTicks - 1);
	for (int ii = 0; ii < numTicks; ii++)
	{
		dTemp = (double)ii * bandWidth;
		ticks[ii] = int(round(min + dTemp));
	}
	return ticks;
}
void WTPAINT::initAreas(vector<int>& indexOrder)
{
	for (int ii = 0; ii < indexOrder.size(); ii++)
	{
		mapAreas.emplace(areaNames[indexOrder[ii]], indexOrder[ii]);
		auto wpArea = make_unique<Wt::WPolygonArea>();
		wpArea->setPoints(areas[indexOrder[ii]]);
		function<void()> fn = bind(&WTPAINT::areaClicked, this, indexOrder[ii]);
		wpArea->clicked().connect(fn);
		this->addArea(move(wpArea));
	}
	if (indexOrder.size() == areas.size() - 1)
	{
		mapAreas.emplace(areaNames[0], 0);
		double widthD = wlWidth.value() - 2.0;
		double heightD = wlHeight.value() - 2.0;
		auto wpArea = make_unique<Wt::WRectArea>(1.0, 1.0, widthD, heightD);
		function<void()> fn = bind(&WTPAINT::areaClicked, this, 0);
		wpArea->clicked().connect(fn);
		this->addArea(move(wpArea));
	}
}
void WTPAINT::initColour()
{
	keyColour.resize(6);
	keyColour[0] = { 255, 0, 0 };  // Red
	keyColour[1] = { 255, 255, 0 };  // Yellow
	keyColour[2] = { 0, 255, 0 };  // Green
	keyColour[3] = { 0, 255, 255 };  // Teal
	keyColour[4] = { 0, 0, 255 };  // Blue
	keyColour[5] = { 127, 0, 255 };  // Violet
	extraColour = { 255, 0, 127 };  // Pink
}
void WTPAINT::init_proj_dir(string exec_dir)
{
	size_t pos1 = exec_dir.rfind('\\');  // Debug or release.
	pos1 = exec_dir.rfind('\\', pos1 - 1);  // Project folder.
	proj_dir = exec_dir.substr(0, pos1);
}
void WTPAINT::initSize(double w, double h)
{
	wlHeight = Wt::WLength(h);
	wlWidth = Wt::WLength(w);
	resize(wlWidth, wlHeight);
}
vector<Wt::WPointF> WTPAINT::makeWPPath(vector<vector<int>>& frameTLBR, vector<vector<int>>& border, vector<double>& windowDimPosition)
{
	int numPoints = border.size();
	vector<vector<double>> borderD(numPoints, vector<double>(2));
	for (int ii = 0; ii < numPoints; ii++)
	{
		borderD[ii][0] = (double)(border[ii][0] - frameTLBR[0][0]);
		borderD[ii][1] = (double)(border[ii][1] - frameTLBR[0][1]);
	}
	vector<double> mapDim(2);
	double xRatio, yRatio, ratio, myScale, xShift, yShift;
	if (windowDimPosition.size() == 2)  // Indicates the vector contains window 
	{                                   // dimensions (w,h).
		mapDim[0] = (double)(frameTLBR[1][0] - frameTLBR[0][0]);
		mapDim[1] = (double)(frameTLBR[1][1] - frameTLBR[0][1]);
		xRatio = windowDimPosition[0] / mapDim[0];
		yRatio = windowDimPosition[1] / mapDim[1];
		if (xRatio < yRatio) { ratio = xRatio; }
		else { ratio = yRatio; }
		for (int ii = 0; ii < numPoints; ii++)
		{
			borderD[ii][0] *= ratio;
			borderD[ii][1] *= ratio;
		}
		windowDimPosition = { ratio };
	}
	else if (windowDimPosition.size() == 3) // Indicates the vector contains position. 
	{                                       // Form [xShift, yShift, myRatio].
		//xShift = windowDimPosition[0] * windowDimPosition[3];
		//yShift = windowDimPosition[1] * windowDimPosition[4];
		for (int ii = 0; ii < numPoints; ii++)
		{
			borderD[ii][0] *= windowDimPosition[2];
			borderD[ii][1] *= windowDimPosition[2];
			borderD[ii][0] += windowDimPosition[0];
			borderD[ii][1] += windowDimPosition[1];
		}
	}
	else { jf.err("windowDimScaling-wtf.makeWPPath"); }

	vector<Wt::WPointF> area;
	area.resize(numPoints);
	for (int ii = 0; ii < numPoints; ii++)
	{
		area[ii] = Wt::WPointF(borderD[ii][0], borderD[ii][1]);
	}
	return area;
}
void WTPAINT::paintEvent(Wt::WPaintDevice* paintDevice)
{
	Wt::WPainter painter(paintDevice);
	Wt::WLength penWidth(2.0, Wt::LengthUnit::Pixel);
	Wt::WPen pen(Wt::PenStyle::SolidLine);
	pen.setWidth(penWidth);
	painter.setPen(pen);
	Wt::WBrush brush(Wt::BrushStyle::Solid);
	painter.setBrush(brush);
	if (areas.size() < 1) { return; }

	vector<int> indexOrder(areas.size() - 1);
	if (indexAreaSel < 0)  // Nothing selected, so all child regions are drawn.
	{
		mapAreas.clear();
		clearAreas();
		for (int ii = 0; ii < areas.size() - 1; ii++)
		{
			indexOrder[ii] = ii + 1;
		}
		initAreas(indexOrder);
		paintRegionAll(painter, indexOrder, colourDimPercent);
		paintLegendBarV(painter);
		paintLegendBox(painter, 0);
	}
	else if (indexAreaPrevious < 0)
	{
		if (indexAreaSel != 0) { paintRegion(painter, indexAreaSel, 1.0); }
		paintLegendBox(painter, indexAreaSel);
	}
	else
	{
		if (indexAreaPrevious != 0) { paintRegion(painter, indexAreaPrevious, colourDimPercent); }
		if (indexAreaSel != 0) { paintRegion(painter, indexAreaSel, 1.0); }
		paintLegendBox(painter, indexAreaSel);
	}
}
void WTPAINT::paintLegendBarV(Wt::WPainter& painter)
{
	int numColour = numColourBands + 1;
	if (keyColour.size() != numColour) { initColour(); }
	double dLen = wlHeight.value() - (2.0 * barThickness);
	Wt::WRectF rectV(barThickness, barThickness, barThickness, dLen);
	Wt::WGradient wGrad;
	Wt::WColor wColour;
	Wt::WString wsTemp;
	string temp;
	vector<int> scaleValues = getScaleValues(numColour);
	double wGradX = 1.5 * barThickness;
	double wGradY0 = dLen + barThickness - 1.0;
	double wGradY1 = barThickness + 1.0;
	wGrad.setLinearGradient(wGradX, wGradY0, wGradX, wGradY1);
	double bandWidth = 1.0 / (double)(numColourBands);
	double tickX = 2.0 * barThickness, tickY, dTemp, xCoord, yCoord;
	for (int ii = 0; ii < numColour; ii++)
	{
		dTemp = (double)ii * bandWidth;
		wColour.setRgb(keyColour[ii][0], keyColour[ii][1], keyColour[ii][2]);
		wGrad.addColorStop(dTemp, wColour);
		tickY = wGradY0 - (dTemp * dLen) + 1.0;
		painter.drawLine(tickX, tickY, tickX + (barThickness / 2.0), tickY);
		temp = to_string(scaleValues[ii]);
		wsTemp = Wt::WString::fromUTF8(temp);
		xCoord = tickX + barThickness;
		if (ii == 0) { yCoord = tickY - 14.0; }
		else if (ii == numColour - 1) { yCoord = tickY; }
		else { yCoord = tickY - 7.0; }
		painter.drawText(xCoord, yCoord, 100.0, barThickness, Wt::AlignmentFlag::Left, wsTemp);
	}
	Wt::WBrush wBrush(wGrad);
	painter.setBrush(wBrush);
	painter.drawRect(rectV);
}
void WTPAINT::paintLegendBox(Wt::WPainter& painter, int index)
{
	string shortName, sBase, temp;
	vector<string> boxText;
	size_t pos1 = dataName[0].find("Total - "), indexBox = 0;
	if (pos1 < dataName[0].size())
	{
		boxText.resize(5);
		boxText[0] = "Showing";
		pos1 += 8;
		sBase = dataName[0].substr(pos1);
		for (int ii = 1; ii < dataName.size() - 1; ii++)
		{
			temp = dataName[ii];
			while (temp[0] == '+') { temp.erase(temp.begin()); }
			sBase += "->" + temp;
		}
		boxText[1] = sBase + " WITH";
		boxText[2] = dataName.back();
		indexBox = 3;
	}
	else
	{
		boxText.resize(dataName.size() + 3);
		boxText[0] = "Showing";
		for (int ii = 0; ii < dataName.size(); ii++)
		{
			boxText[ii + 1] = dataName[ii];
		}
		indexBox = dataName.size() + 1;
	}
	boxText[indexBox] = "for the region";

	double dTemp = wlWidth.value();
	double dHeight = 21.0 * (double)boxText.size();
	Wt::WRectF box(dTemp - legendBoxDim[0] - 15.0, 1.0, legendBoxDim[0], dHeight);
	Wt::WBrush wBrush(Wt::StandardColor::White);
	painter.setBrush(wBrush);
	painter.drawRect(box);
	pos1 = areaNames[index].find(" or ");
	if (pos1 < areaNames[index].size()) 
	{
		shortName = areaNames[index].substr(0, pos1);
	}
	else { shortName = areaNames[index]; }
	pos1 = shortName.find("(Canada)");
	if (pos1 < shortName.size()) { shortName.resize(pos1); }
	boxText[indexBox + 1] = shortName + " = " + to_string(int(areaData[index]));
	Wt::WString wsTemp; 
	dTemp = box.y() - 15.0;
	for (int ii = 0; ii < boxText.size(); ii++)
	{
		wsTemp = Wt::WString::fromUTF8(boxText[ii]);
		dTemp += 20.0;
		box.setY(dTemp);
		painter.drawText(box, Wt::AlignmentFlag::Center, Wt::TextFlag::SingleLine, wsTemp);
	}
}
void WTPAINT::paintRegion(Wt::WPainter& painter, int index, double percent)
{
	Wt::WPainterPath wpPath = Wt::WPainterPath(areas[index][0]);
	for (int jj = 1; jj < areas[index].size(); jj++)
	{
		wpPath.lineTo(areas[index][jj]);
	}
	wpPath.closeSubPath();
	Wt::WColor wColour(Wt::StandardColor::White);
	Wt::WBrush wBrush(wColour);
	painter.setBrush(wBrush);
	painter.drawPath(wpPath);
	setWColour(wColour, areaColour[index], percent);
	wBrush.setColor(wColour);
	painter.setBrush(wBrush);
	painter.drawPath(wpPath);
}
void WTPAINT::paintRegionAll(Wt::WPainter& painter, vector<int> indexOrder, double percent)
{
	Wt::WColor wColour(Wt::StandardColor::White);
	Wt::WBrush wBrush(wColour);
	painter.setBrush(wBrush);
	Wt::WPainterPath wpPath = Wt::WPainterPath(areas[0][0]);
	for (int jj = 1; jj < areas[0].size(); jj++)
	{
		wpPath.lineTo(areas[0][jj]);
	}
	wpPath.closeSubPath();
	painter.drawPath(wpPath);
	for (int ii = 0; ii < indexOrder.size(); ii++)
	{
		setWColour(wColour, areaColour[indexOrder[ii]], percent);
		wBrush.setColor(wColour);
		painter.setBrush(wBrush);
		Wt::WPainterPath wpPath = Wt::WPainterPath(areas[indexOrder[ii]][0]);
		for (int jj = 1; jj < areas[indexOrder[ii]].size(); jj++)
		{
			wpPath.lineTo(areas[indexOrder[ii]][jj]);
		}
		wpPath.closeSubPath();
		painter.drawPath(wpPath);		
	}
}
void WTPAINT::setWColour(Wt::WColor& wColour, vector<int> rgb, double percent)
{
	double dTemp = 255.0 * percent;
	wColour.setRgb(rgb[0], rgb[1], rgb[2], int(dTemp));
}
bool WTPAINT::testExcludeParent()
{
	vector<int> minMaxIndex = jf.minMax(areaDataKids);
	double percent = areaDataKids[minMaxIndex[1]] / areaData[0]; 
	if (percent < defaultParentExclusionThreshold) { return 1; }
	return 0;
}
void WTPAINT::updateAreaColour()
{
	size_t numArea = areas.size();
	if (numArea != areaData.size()) { jf.err("Area size mismatch-wtf.updateAreaColour"); }
	if (keyColour.size() != numColourBands + 1) { initColour(); }
	areaColour.resize(numArea, vector<int>(3));
	double percentage, dR, dG, dB, remains, dMin, dMax;
	int floor, indexStart;
	vector<int> indexMinMax;
	if (testExcludeParent())
	{
		indexMinMax = jf.minMax(areaDataKids);
		dMin = areaDataKids[indexMinMax[0]];
		dMax = areaDataKids[indexMinMax[1]];
		areaColour[0] = extraColour;  // Parent.
		indexStart = 1;
	}
	else
	{
		indexMinMax = jf.minMax(areaData);
		dMin = areaData[indexMinMax[0]];
		dMax = areaData[indexMinMax[1]];
		indexStart = 0;
	}
	for (int ii = indexStart; ii < numArea; ii++)
	{
		if (areaData[ii] == -1.0)  // Data is missing, so draw as grey.
		{
			areaColour[ii][0] = 180;
			areaColour[ii][1] = 180;
			areaColour[ii][2] = 180;
			continue;
		}
		percentage = (areaData[ii] - dMin) / dMax;
		if (percentage > 0.9999) { percentage = 0.9999; }
		percentage *= (double)numColourBands;
		floor = int(percentage);
		remains = percentage - (double)floor;
		dR = (keyColour[floor + 1][0] - keyColour[floor][0]) * remains;
		dG = (keyColour[floor + 1][1] - keyColour[floor][1]) * remains;
		dB = (keyColour[floor + 1][2] - keyColour[floor][2]) * remains;
		areaColour[ii][0] = keyColour[floor][0] + (int)dR;
		areaColour[ii][1] = keyColour[floor][1] + (int)dG;
		areaColour[ii][2] = keyColour[floor][2] + (int)dB;
	}

}
