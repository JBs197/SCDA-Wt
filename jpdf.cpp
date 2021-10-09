#include "jpdf.h"

void JPDFCELL::drawCellBar(HPDF_Page& page, double barWidth, JFUNC& jf)
{
	HPDF_STATUS error;
	int index = vBLTR.size() - 1;
	double barHeight = vBLTR[index][1][1] - vBLTR[index][0][1] - (2.0 * padding);
	double xCoord = vBLTR[index][0][0] + padding;
	for (int ii = 0; ii < vBarColour.size(); ii++)
	{
		error = HPDF_Page_SetRGBFill(page, vBarColour[ii][0], vBarColour[ii][1], vBarColour[ii][2]);
		if (error != HPDF_OK) { jf.err("SetRGBFill-jpdfcell.drawCellBar"); }
		error = HPDF_Page_Rectangle(page, xCoord, vBLTR[index][0][1] + padding, barWidth, barHeight);
		if (error != HPDF_OK) { jf.err("Rectangle-jpdfcell.drawCellBar"); }
		error = HPDF_Page_Fill(page);
		if (error != HPDF_OK) { jf.err("FillStroke-jpdfcell.drawCellBar"); }
		xCoord += barWidth;
	}
	xCoord += padding;
	vector<vector<double>> newBLTR = vBLTR[index];
	vBLTR[index][1][0] = xCoord;
	newBLTR[0][0] = xCoord;
	vBLTR.push_back(newBLTR);
}
void JPDFCELL::drawCellText(HPDF_Page& page, int indexText, JFUNC& jf)
{
	if (vsText.size() <= indexText) { return; }
	float textWidth;
	int index = vBLTR.size() - 1;
	float cellWidth = vBLTR[index][1][0] - vBLTR[index][0][0] - (2.0 * padding);
	vector<vector<double>> newBLTR = vBLTR[index];
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, vFontSize[indexText]);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfcell.drawCell"); }
	float charSpace = HPDF_Page_GetCharSpace(page);
	float wordSpace = HPDF_Page_GetWordSpace(page);
	HPDF_UINT length = vsText[indexText].size();
	HPDF_BYTE* buffer = new unsigned char[length];
	for (int ii = 0; ii < length; ii++)
	{
		buffer[ii] = (unsigned char)vsText[indexText][ii];
	}
	HPDF_UINT max = HPDF_Font_MeasureText(font, buffer, length, cellWidth, vFontSize[indexText], charSpace, wordSpace, 0, &textWidth);
	if (max >= length)
	{
		double bottomPadding = (vBLTR[index][1][1] - vBLTR[index][0][1] - vFontSize[indexText]);
		error = HPDF_Page_TextOut(page, vBLTR[index][0][0] + padding, vBLTR[index][0][1] + bottomPadding, vsText[indexText].c_str());
		if (error != HPDF_OK) { jf.err("TextOut-jpdfcell.drawCell"); }
		vBLTR[index][1][0] = vBLTR[index][0][0] + textWidth + (2.0 * padding);
		newBLTR[0][0] += textWidth + (2.0 * padding);
		vBLTR.push_back(newBLTR);
	}
	else
	{
		jf.err("Cell text too long-jpdfcell.drawCell");
	}
	delete[] buffer;
}
double JPDFCELL::getMaxFontHeight()
{
	int index = vBLTR.size() - 1;
	if (index < 0) { return -1.0; }
	double maxHeight = vBLTR[index][1][1] - vBLTR[index][0][1] - (2.0 * padding);
	return maxHeight;
}

void JPDFTABLE::addColourBars(vector<vector<double>>& vColour, int iRow, int iCol)
{
	// Adds a list of colours to a single table cell.
	vvCell[iRow][iCol].vBarColour = vColour;
}
void JPDFTABLE::addText(vector<string>& vsText, int fontSize)
{
	// Adds one string of text to each cell, in sequential order.
	int indexText = 0, max = vsText.size();
	double fontHeight;
	for (int ii = 0; ii < numCol; ii++)
	{
		for (int jj = 0; jj < numRow; jj++)
		{
			vvCell[jj][ii].vsText.push_back(vsText[indexText]);
			fontHeight = min((double)fontSize, vvCell[jj][ii].getMaxFontHeight());
			vvCell[jj][ii].vFontSize.push_back(fontHeight);
			indexText++;
			if (indexText == max) { return; }
		}
	}
}
void JPDFTABLE::addText(string& text, int iRow, int iCol, int fontSize)
{
	// Adds one string of text to a specific cell.
	vvCell[iRow][iCol].vsText.push_back(text);
	double fontHeight = min((double)fontSize, vvCell[iRow][iCol].getMaxFontHeight());
	vvCell[iRow][iCol].vFontSize.push_back(fontHeight);
}
void JPDFTABLE::addTextList(vector<string>& vsText, int iRow, int iCol, int fontSize)
{
	// Adds a list of strings to a specific cell.
	double fontHeight = min((double)fontSize, vvCell[iRow][iCol].getMaxFontHeight());
	for (int ii = 0; ii < vsText.size(); ii++)
	{
		vvCell[iRow][iCol].vsText.push_back(vsText[ii]);
		vvCell[iRow][iCol].vFontSize.push_back(fontHeight);
	}
}
void JPDFTABLE::drawBackgroundColour()
{
	HPDF_STATUS error;
	double width, height;
	for (int ii = 0; ii < numRow; ii++)
	{
		for (int jj = 0; jj < numCol; jj++)
		{
			width = vvCell[ii][jj].vBLTR[vvCell[ii][jj].vBLTR.size() - 1][1][0] - vvCell[ii][jj].vBLTR[0][0][0];
			height = vvCell[ii][jj].vBLTR[vvCell[ii][jj].vBLTR.size() - 1][1][1] - vvCell[ii][jj].vBLTR[0][0][1];
			error = HPDF_Page_SetRGBFill(page, vvCell[ii][jj].backgroundColour[0], vvCell[ii][jj].backgroundColour[1], vvCell[ii][jj].backgroundColour[2]);
			if (error != HPDF_OK) { jf.err("SetRGBFill-jpdftable.drawBackgroundColour"); }
			error = HPDF_Page_Rectangle(page, vvCell[ii][jj].vBLTR[0][0][0], vvCell[ii][jj].vBLTR[0][0][1], width, height);
			if (error != HPDF_OK) { jf.err("Rectangle-jpdftable.drawBackgroundColour"); }
			error = HPDF_Page_Fill(page);
			if (error != HPDF_OK) { jf.err("Fill-jpdftable.drawBackgroundColour"); }
		}
	}
}
void JPDFTABLE::drawColourBars(int barWidth)
{
	// Draw every cell's pre-loaded list of colours, as rectangles of text height and given width.
	double dWidth = (double)barWidth;
	for (int ii = 0; ii < numCol; ii++)
	{
		for (int jj = 0; jj < numRow; jj++)
		{
			vvCell[jj][ii].drawCellBar(page, dWidth, jf);
		}
	}
}
void JPDFTABLE::drawColSplit()
{
	// Draw a thin vertical line through every cell, at the beginning of its last vBLTR.
	double xMax, xPos;
	int index;
	vector<vector<double>> startStop(2, vector<double>(2));
	startStop[0][1] = boxBLTR[0][1];
	startStop[1][1] = boxBLTR[1][1];
	for (int ii = 0; ii < numCol; ii++)
	{
		xMax = 0.0;
		for (int jj = 0; jj < numRow; jj++)
		{
			index = vvCell[jj][ii].vBLTR.size() - 1;
			xPos = vvCell[jj][ii].vBLTR[index][0][0];
			if (xPos > xMax) { xMax = xPos; }
		}
		startStop[0][0] = xMax;
		startStop[1][0] = xMax;
		drawLine(startStop, colourListDouble[0], 1.0);
	}
}
void JPDFTABLE::drawFrames()
{
	// Draws the value box frame, as well as the column divisor lines.
	double thickness = max(borderThickness, 1.0);
	drawRect(boxBLTR, colourListDouble[0], thickness);

	thickness = max(borderThickness - 1.0, 1.0);
	vector<vector<double>> startStop(2, vector<double>(2));
	startStop[0][1] = boxBLTR[0][1];
	startStop[1][1] = boxBLTR[1][1];
	for (int ii = 1; ii < numCol; ii++)
	{
		startStop[0][0] = vvCell[0][ii].vBLTR[0][0][0];
		startStop[1][0] = startStop[0][0];
		drawLine(startStop, colourListDouble[0], thickness);
	}

	thickness = max(borderThickness - 2.0, 1.0);
	int numSplits;
	for (int ii = 0; ii < numCol; ii++)
	{
		numSplits = vvCell[0][ii].vBLTR.size() - 1;
		for (int jj = 0; jj < numSplits; jj++)
		{
			startStop[0][0] = vvCell[0][ii].vBLTR[jj][1][0];
			startStop[1][0] = startStop[0][0];
			drawLine(startStop, colourListDouble[0], thickness);
		}
	}
}
void JPDFTABLE::drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness)
{
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdftable.drawLine"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdftable.drawLine"); }
	error = HPDF_Page_MoveTo(page, startStop[0][0], startStop[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdftable.drawLine"); }
	error = HPDF_Page_LineTo(page, startStop[1][0], startStop[1][1]);
	if (error != HPDF_OK) { jf.err("LineTo-jpdftable.drawLine"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdftable.drawLine"); }
}
void JPDFTABLE::drawRect(vector<vector<double>> rectBLTR, vector<double> colour, double thickness)
{
	// This variant will only draw the perimeter.
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdftable.drawRect"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdftable.drawRect"); }
	error = HPDF_Page_MoveTo(page, rectBLTR[0][0], rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdftable.drawRect"); }
	error = HPDF_Page_Rectangle(page, rectBLTR[0][0], rectBLTR[0][1], rectBLTR[1][0] - rectBLTR[0][0], rectBLTR[1][1] - rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("Rectangle-jpdftable.drawRect"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawRect"); }
}
vector<vector<double>> JPDFTABLE::drawTable()
{
	// Returns the table's BLTR.
	drawBackgroundColour();
	drawFrames();
	drawTitle();
	return tableBLTR;
}
void JPDFTABLE::drawText(int index)
{
	// Index refers to each cell's vsText index.
	HPDF_STATUS error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);  // Black.
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdftable.drawValues"); }
	HPDF_Page_BeginText(page);
	for (int ii = 0; ii < numCol; ii++)
	{
		for (int jj = 0; jj < numRow; jj++)
		{
			vvCell[jj][ii].drawCellText(page, index, jf);
		}
	}
	HPDF_Page_EndText(page);
}
void JPDFTABLE::drawTitle()
{
	if (title.size() < 1) { return; }
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, fontTitle, fontSizeTitle);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdftable.drawTitle"); }
	error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdftable.drawTitle"); }
	HPDF_Page_BeginText(page);
	error = HPDF_Page_TextOut(page, titleBLTR[0][0] + 2.0, titleBLTR[0][1] + 3.0, title.c_str());
	if (error != HPDF_OK) { jf.err("TextOut-jpdftable.drawTitle"); }
	error = HPDF_Page_EndText(page);
}
void JPDFTABLE::initTitle(string sTitle, HPDF_Font& font, int fontSize)
{
	// tableBLTR defines the entire object, while boxTLBR is for the rows/columns only;
	title = sTitle;
	fontTitle = font;
	fontSizeTitle = fontSize;
	titleBLTR.resize(2, vector<double>(2));
	titleBLTR[1] = tableBLTR[1];
	titleBLTR[0][0] = tableBLTR[0][0];
	titleBLTR[0][1] = tableBLTR[1][1] - (fontSizeTitle + 4.0);
	boxBLTR.resize(2, vector<double>(2));
	boxBLTR[0] = tableBLTR[0];
	boxBLTR[1][0] = tableBLTR[1][0];
	boxBLTR[1][1] = titleBLTR[0][1];
}
void JPDFTABLE::setColourBackground(vector<vector<int>> vvColourIndex)
{
	// If a dimension of vvColourIndex is too small, it will restart from its beginning.
	int rowIndex = 0, colIndex = 0;
	for (int ii = 0; ii < numRow; ii++)
	{
		colIndex = 0;
		for (int jj = 0; jj < numCol; jj++)
		{
			vvCell[ii][jj].backgroundColour = colourListDouble[vvColourIndex[rowIndex][colIndex]];
			if (colIndex < vvColourIndex[rowIndex].size() - 1) { colIndex++; }
			else { colIndex = 0; }
		}
		if (rowIndex < vvColourIndex.size() - 1) { rowIndex++; }
		else { rowIndex = 0; }
	}
}
void JPDFTABLE::setRowCol(int row, int col, vector<double>& vRowHeight)
{
	numRow = row;
	numCol = col;
	vvCell.resize(numRow, vector<JPDFCELL>(numCol));
	double cellWidth = floor((boxBLTR[1][0] - boxBLTR[0][0]) / (double)numCol);
	vector<vector<double>> cellBLTR(2, vector<double>(2));
	cellBLTR[0][1] = boxBLTR[1][1];
	cellBLTR[1][1] = cellBLTR[0][1] + vRowHeight[0];
	for (int ii = 0; ii < numRow; ii++)
	{
		cellBLTR[0][1] -= vRowHeight[ii];
		cellBLTR[1][1] -= vRowHeight[ii];
		cellBLTR[1][0] = boxBLTR[0][0];
		cellBLTR[0][0] = cellBLTR[1][0] - cellWidth;
		for (int jj = 0; jj < numCol; jj++)
		{
			vvCell[ii][jj] = JPDFCELL();
			vvCell[ii][jj].font = fontTitle;
			vvCell[ii][jj].rowIndex = ii;
			vvCell[ii][jj].colIndex = jj;
			cellBLTR[0][0] += cellWidth;
			cellBLTR[1][0] += cellWidth;
			vvCell[ii][jj].vBLTR.push_back(cellBLTR);
		}
	}
}

int JPDF::addTable(int numCol, vector<double> vRowHeight, string title, double fontSizeTitle)
{
	// Will populate the table going top->bottom, then left->right. Table's BL corner 
	// will be the current cursor position. Return the new table's index within the vector.
	int indexTable = vTable.size();
	int numRow = vRowHeight.size();
	double height = 0.0; 
	for (int ii = 0; ii < numRow; ii++)
	{
		height += vRowHeight[ii];
	}
	height += fontSizeTitle + 4.0;
	vector<vector<double>> tableBLTR(2, vector<double>(2));
	tableBLTR[0] = cursor;
	tableBLTR[1][0] = HPDF_Page_GetWidth(page) - margin;
	tableBLTR[1][1] = tableBLTR[0][1] + height;
	if (fontAdded) { vTable.push_back(JPDFTABLE(page, tableBLTR, title, fontAdded, fontSizeTitle)); }
	else { vTable.push_back(JPDFTABLE(page, tableBLTR, title, fontDefault, fontSizeTitle)); }
	if (fontAddedItalic) { vTable[indexTable].fontItalic = fontAddedItalic; }
	vTable[indexTable].setRowCol(numRow, numCol, vRowHeight);
	vTable[indexTable].colourListDouble = colourListDouble;
	return indexTable;
}
float JPDF::breakListFitWidth(vector<string>& vsList, float textWidth, vector<vector<string>>& vvsList)
{
	// For a given vsList and max width of text, split each list item as necessary into
	// multiple lines such that the max width is never exceeded. Return the total height.
	vvsList.resize(vsList.size(), vector<string>());
	int index, numLines = 0;
	HPDF_UINT maxChars;
	for (int ii = 0; ii < vsList.size(); ii++)
	{
		index = 0;
		vvsList[ii] = { vsList[ii] };
		maxChars = HPDF_Page_MeasureText(page, vvsList[ii][index].c_str(), textWidth, HPDF_TRUE, NULL);
		while (maxChars < vvsList[ii][index].size())
		{
			vvsList[ii].push_back(vvsList[ii][index].substr(maxChars));
			vvsList[ii][index].resize(maxChars - 1);
			index++;
			maxChars = HPDF_Page_MeasureText(page, vvsList[ii][index].c_str(), textWidth, HPDF_TRUE, NULL);
		}
		numLines += vvsList[ii].size();
	}
	float textHeight = (float)numLines * lineHeight;
	return textHeight;
}
void JPDF::drawCircle(vector<double> coordCenter, double radius, vector<double> colour, double thickness)
{
	// This variant will only draw the perimeter.
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawCircle"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawCircle"); }
	error = HPDF_Page_MoveTo(page, coordCenter[0], coordCenter[1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawCircle"); }
	error = HPDF_Page_Circle(page, coordCenter[0], coordCenter[1], radius);
	if (error != HPDF_OK) { jf.err("Circle-jpdf.drawCircle"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawCircle"); }
}
void JPDF::drawCircle(vector<double> coordCenter, double radius, vector<vector<double>> vColour, double thickness)
{
	// This variant will draw the perimeter using the first colour, and fill the 
	// circle using the second colour. 
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawCircle"); }
	error = HPDF_Page_SetRGBStroke(page, vColour[0][0], vColour[0][1], vColour[0][2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawCircle"); }
	error = HPDF_Page_SetRGBFill(page, vColour[1][0], vColour[1][1], vColour[1][2]);
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.drawCircle"); }
	error = HPDF_Page_MoveTo(page, coordCenter[0], coordCenter[1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawCircle"); }
	error = HPDF_Page_Circle(page, coordCenter[0], coordCenter[1], radius);
	if (error != HPDF_OK) { jf.err("Circle-jpdf.drawCircle"); }
	error = HPDF_Page_FillStroke(page);
	if (error != HPDF_OK) { jf.err("FillStroke-jpdf.drawCircle"); }
}
void JPDF::drawGradientH(vector<float> heightStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos)
{
	if (vColour.size() != vColourPos.size()) { jf.err("Size mismatch-jpdf.drawGradientH"); }
	int colourIndex = -1;
	int xPos = vColourPos[0];
	int xStop = vColourPos[vColourPos.size() - 1];
	int frontier = vColourPos[colourIndex + 1];
	int xInc = 1;
	if (xStop < xPos) { xInc = -1; }
	float dR = 0.0, dG = 0.0, dB = 0.0;
	float r = (float)vColour[0][0];
	float g = (float)vColour[0][1];
	float b = (float)vColour[0][2];
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, 1.0);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawGradientH"); }

	while (xPos != xStop)
	{
		if (xPos >= frontier)
		{
			colourIndex++;
			frontier = vColourPos[colourIndex + 1];
			dR = (float)((vColour[colourIndex + 1][0] - vColour[colourIndex][0]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
			dG = (float)((vColour[colourIndex + 1][1] - vColour[colourIndex][1]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
			dB = (float)((vColour[colourIndex + 1][2] - vColour[colourIndex][2]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
		}

		r += dR;
		if (r > 1.0) { r = 1.0; }
		if (r < 0.0) { r = 0.0; }
		g += dG;
		if (g > 1.0) { g = 1.0; }
		if (g < 0.0) { g = 0.0; }
		b += dB;
		if (b > 1.0) { b = 1.0; }
		if (b < 0.0) { b = 0.0; }

		error = HPDF_Page_SetRGBStroke(page, r, g, b);
		if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawGradientH"); }
		error = HPDF_Page_MoveTo(page, (float)xPos, heightStartStop[0]);
		if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientH"); }
		error = HPDF_Page_LineTo(page, (float)xPos, heightStartStop[1]);
		if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientH"); }
		error = HPDF_Page_Stroke(page);
		if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawGradientH"); }

		xPos += xInc;
	}
	error = HPDF_Page_SetRGBStroke(page, r, g, b);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawGradientH"); }
	error = HPDF_Page_MoveTo(page, (float)xPos, heightStartStop[0]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientH"); }
	error = HPDF_Page_LineTo(page, (float)xPos, heightStartStop[1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientH"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawGradientH"); }

}
void JPDF::drawGradientV(vector<float> widthStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos)
{
	if (vColour.size() != vColourPos.size()) { jf.err("Size mismatch-jpdf.drawGradientV"); }
	int colourIndex = -1;
	int yPos = vColourPos[0];
	int yStop = vColourPos[vColourPos.size() - 1];
	int frontier = vColourPos[colourIndex + 1];
	int yInc = 1;
	if (yStop < yPos) { yInc = -1; }
	float dR, dG, dB;
	float r = (float)vColour[0][0];
	float g = (float)vColour[0][1];
	float b = (float)vColour[0][2];
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, 1.0);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawGradientV"); }

	while (yPos != yStop)
	{
		if (yPos >= frontier)
		{
			colourIndex++;
			frontier = vColourPos[colourIndex + 1];
			dR = (float)((vColour[colourIndex + 1][0] - vColour[colourIndex][0]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
			dG = (float)((vColour[colourIndex + 1][1] - vColour[colourIndex][1]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
			dB = (float)((vColour[colourIndex + 1][2] - vColour[colourIndex][2]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
		}

		r += dR;
		if (r > 1.0) { r = 1.0; }
		if (r < 0.0) { r = 0.0; }
		g += dG;
		if (g > 1.0) { g = 1.0; }
		if (g < 0.0) { g = 0.0; }
		b += dB;
		if (b > 1.0) { b = 1.0; }
		if (b < 0.0) { b = 0.0; }

		error = HPDF_Page_SetRGBStroke(page, r, g, b);
		if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawGradientV"); }
		error = HPDF_Page_MoveTo(page, widthStartStop[0], (float)yPos);
		if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientV"); }
		error = HPDF_Page_LineTo(page, widthStartStop[1], (float)yPos);
		if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientV"); }
		error = HPDF_Page_Stroke(page);
		if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawGradientV"); }

		yPos += yInc;
	}
	error = HPDF_Page_SetRGBStroke(page, r, g, b);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawGradientV"); }
	error = HPDF_Page_MoveTo(page, widthStartStop[0], (float)yPos);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientV"); }
	error = HPDF_Page_LineTo(page, widthStartStop[1], (float)yPos);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientV"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawGradientV"); }

}
void JPDF::drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness)
{
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawLine"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawLine"); }
	error = HPDF_Page_MoveTo(page, startStop[0][0], startStop[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawLine"); }
	error = HPDF_Page_LineTo(page, startStop[1][0], startStop[1][1]);
	if (error != HPDF_OK) { jf.err("LineTo-jpdf.drawLine"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawLine"); }
}
void JPDF::drawRect(vector<vector<double>> rectBLTR, vector<double> colour, double thickness)
{
	// This variant will only draw the perimeter.
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawRect"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawRect"); }
	error = HPDF_Page_MoveTo(page, rectBLTR[0][0], rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawRect"); }
	error = HPDF_Page_Rectangle(page, rectBLTR[0][0], rectBLTR[0][1], rectBLTR[1][0] - rectBLTR[0][0], rectBLTR[1][1] - rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("Rectangle-jpdf.drawRect"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawRect"); }
}
void JPDF::drawRect(vector<vector<double>> rectBLTR, vector<vector<double>> vColour, double thickness)
{
	// This variant will draw the perimeter using the first colour, and fill the 
	// rectangle using the second colour. 
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawRect"); }
	error = HPDF_Page_SetRGBStroke(page, vColour[0][0], vColour[0][1], vColour[0][2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawRect"); }
	error = HPDF_Page_SetRGBFill(page, vColour[1][0], vColour[1][1], vColour[1][2]);
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.drawRect"); }
	error = HPDF_Page_MoveTo(page, rectBLTR[0][0], rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawRect"); }
	error = HPDF_Page_Rectangle(page, rectBLTR[0][0], rectBLTR[0][1], rectBLTR[1][0] - rectBLTR[0][0], rectBLTR[1][1] - rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("Rectangle-jpdf.drawRect"); }
	error = HPDF_Page_FillStroke(page);
	if (error != HPDF_OK) { jf.err("FillStroke-jpdf.drawRect"); }
}
void JPDF::drawRegion(vector<vector<double>>& vvdPath, vector<double> colour)
{
	// This variant will fill the inside of the path without drawing a border.
	HPDF_STATUS error = HPDF_Page_SetRGBFill(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.drawRegion"); }
	error = HPDF_Page_SetLineWidth(page, 0.0);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawRegion"); }
	error = HPDF_Page_MoveTo(page, vvdPath[0][0], vvdPath[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawRegion"); }
	for (int ii = 1; ii < vvdPath.size(); ii++)
	{
		error = HPDF_Page_LineTo(page, vvdPath[ii][0], vvdPath[ii][1]);
		if (error != HPDF_OK) { jf.err("LineTo-jpdf.drawRegion"); }
	}
	error = HPDF_Page_ClosePathFillStroke(page);
	if (error != HPDF_OK) { jf.err("ClosePath-jpdf.drawRegion"); }
}
vector<double> JPDF::getPageDimensions()
{
	// Returns the (width, height) of the active page (inside the margins), in pixels.
	vector<double> vdDim(2);
	vdDim[0] = (double)(HPDF_Page_GetWidth(page) - (2.0 * margin));
	if (vdDim[0] <= 0.0) { jf.err("GetWidth-jpdf.getPageDimensions"); }
	vdDim[1] = (double)(HPDF_Page_GetHeight(page) - (2.0 * margin));
	if (vdDim[1] <= 0.0) { jf.err("GetHeight-jpdf.getPageDimensions"); }
	return vdDim;
}
HPDF_UINT32 JPDF::getPDF(string& sPDF)
{
	// Return the complete PDF as a string. 
	sPDF.clear();
	HPDF_STATUS error = HPDF_SaveToStream(pdf);
	if (error != HPDF_OK) { jf.err("SaveToStream-jpdf.getPDF"); }
	HPDF_UINT32 streamSize = HPDF_GetStreamSize(pdf);
	if (!streamSize) { jf.err("GetStreamSize-jpdf.getPDF"); }
	auto buffer = new HPDF_BYTE[streamSize];
	error = HPDF_ReadFromStream(pdf, buffer, &streamSize);
	if (error != HPDF_OK) { jf.err("ReadFromStream-jpdf.getPDF"); }
	sPDF.resize(streamSize);
	for (int ii = 0; ii < streamSize; ii++)
	{
		sPDF[ii] = (char)buffer[ii];
	}
	delete[] buffer;
	error = HPDF_ResetStream(pdf);
	if (error != HPDF_OK) { jf.err("ResetStream-jpdf.getPDF"); }
	return streamSize;
}
int JPDF::getNumLines(string& text, double width)
{
	int numLines = 0;
	HPDF_UINT maxChars = 0;
	size_t pos1 = 0;
	string temp;
	float fontHeight = (float)fontSize;
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, fontAdded, fontHeight);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.getNumLines"); }
	do
	{
		numLines++;
		temp = text.substr(pos1);
		maxChars = HPDF_Page_MeasureText(page, temp.c_str(), width, HPDF_TRUE, NULL);
		if (maxChars < temp.size()) { pos1 += maxChars; }
	} while (maxChars < temp.size());
	return numLines;
}
bool JPDF::hasFont()
{
	if (fontAdded) { return 1; }
	return 0;
}
void JPDF::init()
{
	initColour();

	pdf = HPDF_New(error_handler, NULL);
	if (!pdf) { jf.err("Failed to create pdf object-jpdf.init"); }
	HPDF_UseUTFEncodings(pdf);
	HPDF_SetCurrentEncoder(pdf, "UTF-8");
	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
	fontDefault = HPDF_GetFont(pdf, "Helvetica", NULL); 
	page = HPDF_AddPage(pdf);
	HPDF_STATUS error = HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);
	if (error != HPDF_OK) { jf.err("Page_SetSize-jpdf.init"); }
	cursor = { margin, HPDF_Page_GetHeight(page) - margin };  // Top-left corner within the default margin.
	lineHeight = (float)fontSize + 4.0;
}
void JPDF::initColour()
{
	colourList.resize(5);
	colourList[0] = { 0, 0, 0, 255 };  // Black
	colourList[1] = { 255, 255, 255, 255 };  // White
	colourList[2] = { 210, 210, 210, 255 };  // Light Grey
	colourList[3] = { 200, 200, 255, 255 };  // SelectedWeak
	colourList[4] = { 150, 150, 192, 255 };  // SelectedStrong

	colourListDouble.resize(colourList.size());
	for (int ii = 0; ii < colourListDouble.size(); ii++)
	{
		colourListDouble[ii] = jf.rgbxToDouble(colourList[ii]);
	}
}
void JPDF::parameterSectionBottom(vector<string>& vsParameter, vector<int>& vColour)
{
	// Will print the parameter list at the bottom of the active page. 
	// At the end, the cursor value is set to the bottom-left corner of the page's 
	// remaining usable space. 

	// Load fonts and prepare the page.
	HPDF_UINT maxChars;
	HPDF_Font font, fontItalic;
	if (fontAdded) { font = fontAdded; }
	else { font = fontDefault; }
	if (fontAddedItalic) { fontItalic = fontAddedItalic; }
	else { fontItalic = NULL; }
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, fontSize);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.appendParameterSection"); }

	// Break the parameter list into lines of text, and determine the total height.
	vector<vector<string>> vvsParameter;
	float textWidth = HPDF_Page_GetWidth(page) - (2.0 * margin) - 4.0;
	float textHeight = breakListFitWidth(vsParameter, textWidth, vvsParameter);

	// Write the list of parameters, assigning background colours from vColour.
	vector<string> vsTemp;
	vector<double> vRGBA;
	size_t pos1, pos2;
	int index, italic, numLines;
	float parameterHeight;
	cursor[1] = margin + 2.0;  // Bottom of text box.
	error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);  // Black.
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.appendParameterSection"); }
	HPDF_Page_BeginText(page);
	for (int ii = vvsParameter.size() - 1; ii >= 0; ii--)
	{
		// If the assigned background colour is not white, colour it.
		cursor[0] = margin;
		numLines = vvsParameter[ii].size();
		if (vColour[ii] != 1)
		{
			HPDF_Page_EndText(page);
			parameterHeight = (float)numLines * lineHeight;
			vRGBA = colourListDouble[vColour[ii]];
			error = HPDF_Page_SetRGBFill(page, vRGBA[0], vRGBA[1], vRGBA[2]);
			if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.appendParameterSection"); }
			error = HPDF_Page_MoveTo(page, cursor[0], cursor[1]);
			if (error != HPDF_OK) { jf.err("MoveTo-jpdf.appendParameterSection"); }
			error = HPDF_Page_Rectangle(page, cursor[0], cursor[1], textWidth + 3.0, parameterHeight);
			if (error != HPDF_OK) { jf.err("Rectangle-jpdf.appendParameterSection"); }
			error = HPDF_Page_Fill(page);
			if (error != HPDF_OK) { jf.err("Fill-jpdf.appendParameterSection"); }
			error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);  // Black, for text.
			if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.appendParameterSection"); }
			HPDF_Page_BeginText(page);
		}

		// If italics are available, apply them to odd-numbered segments. Then, print the text.
		string temp, segmentor = " | ";
		if (fontItalic)
		{
			float segWidth = HPDF_Page_TextWidth(page, segmentor.c_str());
			cursor[1] += (float)numLines * lineHeight;
			italic = 0;
			for (int jj = 0; jj < vvsParameter[ii].size(); jj++)
			{
				cursor[0] = margin + 2.0;
				cursor[1] -= lineHeight;
				pos1 = vvsParameter[ii][jj].find_first_not_of(" |");
				pos2 = vvsParameter[ii][jj].find('|', pos1) - 1;
				while (pos2 < vvsParameter[ii][jj].size())
				{
					temp = vvsParameter[ii][jj].substr(pos1, pos2 - pos1);
					if (italic) { error = HPDF_Page_SetFontAndSize(page, fontItalic, fontSize); }
					else { error = HPDF_Page_SetFontAndSize(page, font, fontSize); }
					if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.appendParameterSection"); }

					error = HPDF_Page_TextOut(page, cursor[0], cursor[1] + 4.0, temp.c_str());
					if (error != HPDF_OK) { jf.err("TextOut-jpdf.appendParameterSection"); }
					cursor[0] += HPDF_Page_TextWidth(page, temp.c_str());

					if (italic)
					{
						error = HPDF_Page_SetFontAndSize(page, font, fontSize);
						if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.appendParameterSection"); }
						italic = 0;
					}
					else { italic = 1; }
					error = HPDF_Page_TextOut(page, cursor[0], cursor[1] + 4.0, segmentor.c_str());
					if (error != HPDF_OK) { jf.err("TextOut-jpdf.appendParameterSection"); }
					cursor[0] += segWidth;

					pos1 = vvsParameter[ii][jj].find_first_not_of(" |", pos2 + 1);
					if (pos1 > vvsParameter[ii][jj].size()) { break; }
					pos2 = vvsParameter[ii][jj].find('|', pos1) - 1;
				}
				if (pos1 < pos2)  // This line ends with a segment.
				{
					temp = vvsParameter[ii][jj].substr(pos1);
					if (italic) { error = HPDF_Page_SetFontAndSize(page, fontItalic, fontSize); }
					else { error = HPDF_Page_SetFontAndSize(page, font, fontSize); }
					if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.appendParameterSection"); }

					error = HPDF_Page_TextOut(page, cursor[0], cursor[1] + 4.0, temp.c_str());
					if (error != HPDF_OK) { jf.err("TextOut-jpdf.appendParameterSection"); }
				}
				else {}  // This line ends with a segmentor.
			}
			cursor[1] += (float)numLines * lineHeight;
		}
		else
		{
			cursor[1] += (float)numLines * lineHeight;
			for (int jj = 0; jj < vvsParameter[ii].size(); jj++)
			{
				cursor[0] = margin + 2.0;
				cursor[1] -= lineHeight;
				error = HPDF_Page_TextOut(page, cursor[0], cursor[1] + 4.0, vvsParameter[ii][jj].c_str());
				if (error != HPDF_OK) { jf.err("TextOut-jpdf.appendParameterSection"); }
			}
			cursor[1] += (float)numLines * lineHeight;
		}
	}
	HPDF_Page_EndText(page);

	// Draw a rectangle around the "parameter" section.
	cursor[0] = margin;
	cursor[1] = margin;
	error = HPDF_Page_SetLineWidth(page, 3.0);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.appendParameterSection"); }
	error = HPDF_Page_MoveTo(page, cursor[0], cursor[1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.appendParameterSection"); }
	error = HPDF_Page_Rectangle(page, cursor[0], cursor[1], HPDF_Page_GetWidth(page) - (float)(2 * margin), textHeight + 2.0);
	if (error != HPDF_OK) { jf.err("Rectangle-jpdf.appendParameterSection"); }
	error = HPDF_Page_ClosePathStroke(page);
	if (error != HPDF_OK) { jf.err("ClosePathStroke-jpdf.appendParameterSection"); }

	// Write the box title.
	cursor[1] = margin + textHeight + 4.0;
	error = HPDF_Page_SetFontAndSize(page, font, fontSize + 2);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.appendParameterSection"); }
	error = HPDF_Page_BeginText(page);
	error = HPDF_Page_TextOut(page, cursor[0], cursor[1], "Parameters");
	if (error != HPDF_OK) { jf.err("TextOut-jpdf.appendParameterSection"); }
	error = HPDF_Page_EndText(page);

	// Prepare the cursor for the next section.
	cursor[1] += (float)fontSize + 8.0;
}
void JPDF::sectionListBoxed(string sTitle, vector<string>& vsList)
{
	vector<int> vColour;
	vColour.assign(vsList.size(), 1);  // White background.
	sectionListBoxed(sTitle, vsList, vColour, 1);
}
void JPDF::sectionListBoxed(string sTitle, vector<string>& vsList, vector<int>& vColour)
{
	sectionListBoxed(sTitle, vsList, vColour, 1);
}
void JPDF::sectionListBoxed(string sTitle, vector<string>& vsList, vector<int>& vColour, int numCol)
{
	// Background colours will be applied section-wide, by cycling through the provided list.
	int indexText, myFontSize;
	int numCell = vsList.size();
	HPDF_UINT printed;
	HPDF_Font font;
	if (fontAdded) { font = fontAdded; }
	else { font = fontDefault; }

	float rowWidth = HPDF_Page_GetWidth(page) - (2.0 * margin);
	float colWidth = rowWidth / (float)numCol;

	int numRow = numCell / numCol;
	if (numCell % numCol > 0) { numRow++; }
	float sectionHeight = (float)numRow * lineHeight;
	cursor[1] += sectionHeight;

	// Write the list from top->bottom (for all columns), and assigning background colours.
	float textWidth;
	vector<double> vRGBA;
	int indexColour = 0;
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, fontSize);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.sectionListBoxed"); }
	for (int ii = 0; ii < numRow; ii++)
	{
		cursor[0] = margin;
		cursor[1] -= lineHeight;
		if (vColour[indexColour] != 1)  // If background is non-white, paint it.
		{
			vRGBA = colourListDouble[vColour[indexColour]];
			error = HPDF_Page_SetRGBFill(page, vRGBA[0], vRGBA[1], vRGBA[2]);
			if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.sectionListBoxed"); }
			error = HPDF_Page_Rectangle(page, cursor[0], cursor[1], rowWidth, lineHeight);
			if (error != HPDF_OK) { jf.err("Rectangle-jpdf.sectionListBoxed"); }
			error = HPDF_Page_Fill(page);
			if (error != HPDF_OK) { jf.err("Fill-jpdf.sectionListBoxed"); }
			error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
			if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.sectionListBoxed"); }
		}
		HPDF_Page_BeginText(page);
		for (int jj = 0; jj < numCol; jj++)
		{
			indexText = (jj * numRow) + ii;
			if ( indexText >= numCell) { continue; }
			if (vsList[indexText].size() < 1) { continue; }

			cursor[0] = margin + ((float)jj * colWidth);
			error = HPDF_Page_TextRect(page, cursor[0] + 2.0, cursor[1] + lineHeight, cursor[0] + colWidth, cursor[1], vsList[indexText].c_str(), HPDF_TALIGN_LEFT, &printed);
			if (error != HPDF_OK)
			{
				jf.err("Insufficient width-jpdf.sectionListBoxed");
			}
			if (printed != vsList[indexText].size())
			{
				jf.err("Insufficient width-jpdf.sectionListBoxed");
			}
		}
		HPDF_Page_EndText(page);
		if (indexColour == vColour.size() - 1) { indexColour = 0; }
		else { indexColour++; }
	}

	// Draw a box frame.
	cursor[0] = margin;
	error = HPDF_Page_SetLineWidth(page, 3.0);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.sectionListBoxed"); }
	error = HPDF_Page_Rectangle(page, cursor[0], cursor[1], rowWidth, sectionHeight);
	if (error != HPDF_OK) { jf.err("Rectangle-jpdf.sectionListBoxed"); }
	error = HPDF_Page_ClosePathStroke(page);
	if (error != HPDF_OK) { jf.err("ClosePathStroke-jpdf.sectionListBoxed"); }

	// Draw vertical lines between columns.
	vector<vector<double>> startStop(2, vector<double>(2));
	startStop[0][1] = cursor[1];
	startStop[1][1] = cursor[1] + sectionHeight;
	for (int ii = 1; ii < numCol; ii++)
	{
		startStop[0][0] = margin + ((float)ii * colWidth);
		startStop[1][0] = startStop[0][0];
		drawLine(startStop, colourListDouble[0], 2.0);
	}

	// Write the title.
	cursor[1] += sectionHeight;
	error = HPDF_Page_SetFontAndSize(page, font, fontSize + 2);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.sectionListBoxed"); }
	HPDF_Page_BeginText(page);
	error = HPDF_Page_TextOut(page, cursor[0], cursor[1] + 3.0, sTitle.c_str());
	if (error != HPDF_OK) { jf.err("TextOut-jpdf.sectionListBoxed"); }
	error = HPDF_Page_EndText(page);

	// Prepare the cursor for the next section.
	cursor[1] += (float)fontSize + 8.0;
}
void JPDF::setFont(string filePath)
{
	auto fontName = HPDF_LoadTTFontFromFile(pdf, filePath.c_str(), HPDF_TRUE);
	fontAdded = HPDF_GetFont(pdf, fontName, NULL);
}
void JPDF::setFont(string filePath, string filePathItalic)
{
	auto fontName = HPDF_LoadTTFontFromFile(pdf, filePath.c_str(), HPDF_TRUE);
	fontAdded = HPDF_GetFont(pdf, fontName, "UTF-8");
	auto fontNameItalic = HPDF_LoadTTFontFromFile(pdf, filePathItalic.c_str(), HPDF_TRUE);
	fontAddedItalic = HPDF_GetFont(pdf, fontNameItalic, "UTF-8");
}
void JPDF::setFontSize(int size)
{
	fontSize = size;
	lineHeight = (float)fontSize + 4.0;
}
void JPDF::textBox(vector<vector<double>> boxBLTR, string sText, string alignment)
{
	// This variant uses the default font size.
	textBox(boxBLTR, sText, alignment, fontSize);
}
void JPDF::textBox(vector<vector<double>> boxBLTR, string sText, string alignment, int fontsize)
{
	// Accepted alignment strings are "left", "right", "center", "justify".
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, fontDefault, fontsize);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.textBox"); }
	HPDF_Page_BeginText(page);
	if (alignment == "right")
	{
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_RIGHT, NULL);
	}
	else if (alignment == "center")
	{
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_CENTER, NULL);
	}
	else if (alignment == "justify")
	{
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_JUSTIFY, NULL);
	}
	else
	{
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_LEFT, NULL);
	}
	if (error != HPDF_OK) { jf.err("TextRect-jpdf.textBox"); }
	HPDF_Page_EndText(page);
}
