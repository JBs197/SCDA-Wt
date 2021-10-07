#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "jfunc.h"

using namespace std;
namespace
{
    void  __stdcall error_handler(HPDF_STATUS   error_no, HPDF_STATUS   detail_no, void* user_data)
    {
        printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
    }
}

struct JPDFCELL
{
    string alignment = "left";
    vector<double> backgroundColour;
    vector<vector<vector<double>>> vBLTR;  // Form [value index][BL, TR][xCoord, yCoord]
    HPDF_Font font;
    vector<double> vFontSize;
    double padding = 2.0;
    int rowIndex, colIndex;
    vector<string> vsValue;

    JPDFCELL() {}
    ~JPDFCELL() {}

    void drawCell(HPDF_Page& page, int index, JFUNC& jf);
};

class JPDFTABLE
{
    vector<vector<double>> boxBLTR, titleBLTR;
    int fontSizeTitle;
    HPDF_Font fontTitle;
    JFUNC jf;
    int numCol = 0;
    int numRow = 0;
    HPDF_Page& page;
    const vector<vector<double>> tableBLTR;  // Maximum area accorded to the table.
    string title;
    vector<vector<JPDFCELL>> vvCell;  // Form [row index][column value]

    void initTitle(string sTitle, HPDF_Font& font, int fontSize);

public:
    JPDFTABLE(HPDF_Page& pageRef, vector<vector<double>> bltr, string sTitle, HPDF_Font& font, int fontSize)
        : page(pageRef), tableBLTR(bltr) 
    { 
        initTitle(sTitle, font, fontSize); 
    }
    JPDFTABLE(HPDF_Page& pageRef, vector<vector<double>> bltr) : page(pageRef), tableBLTR(bltr)
    {
        boxBLTR = tableBLTR;
    }
    ~JPDFTABLE() {}

    double borderThickness = 3.0;  // Default, in units of pixels.
    vector<vector<double>> colourListDouble;  // Form [colour index][r, g, b, a].

    void addValues(vector<string>& vsValue);
    void drawBackgroundColour();
    void drawColSplit();
    void drawFrames();
    void drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness);
    void drawRect(vector<vector<double>> rectBLTR, vector<double> colour, double thickness);
    vector<vector<double>> drawTable();
    void drawTitle();
    void drawValues(int index);
    void setColourBackground(vector<vector<int>> vvColourIndex);
    void setRowCol(int row, int col);
};

class JPDF
{
    vector<vector<unsigned char>> colourList;  // Form [colour index][r, g, b, a].
    vector<vector<double>> colourListDouble;  // Form [colour index][r, g, b, a]. All values are inside the interval [0.0, 1.0].
    HPDF_Font fontAdded, fontAddedItalic, fontDefault;
    int fontSize = 18;  // Default.
    float lineHeight;  // For text.
    JFUNC jf;
    HPDF_Page page;
    HPDF_Doc pdf;

public:
    JPDF() { init(); }
    ~JPDF() {}

    vector<double> cursor;  // Form [xCoord, yCoord]. 
    double margin = 50.0;
    vector<JPDFTABLE> vTable;

    int addTable(int numCol, vector<string>& vsList, double rowHeight, string title, double fontSizeTitle);
    float breakListFitWidth(vector<string>& vsList, float textWidth, vector<vector<string>>& vvsList);
    void drawCircle(vector<double> coordCenter, double radius, vector<double> colour, double thickness);
    void drawCircle(vector<double> coordCenter, double radius, vector<vector<double>> vColour, double thickness);
    void drawGradientH(vector<float> heightStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos);
    void drawGradientV(vector<float> widthStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos);
    void drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness);
    void drawRect(vector<vector<double>> rectBLTR, vector<double> colour, double thickness);
    void drawRect(vector<vector<double>> rectBLTR, vector<vector<double>> vColour, double thickness);
    void drawRegion(vector<vector<double>>& vvdPath, vector<double> colour);
    bool hasFont();
    vector<double> getPageDimensions();
    HPDF_UINT32 getPDF(string& sPDF);
    void init();
    void initColour();
    void parameterSectionBottom(vector<string>& vsParameter, vector<int>& vColour);
    void sectionListBoxed(string sTitle, vector<string>& vsList);
    void sectionListBoxed(string sTitle, vector<string>& vsList, vector<int>& vColour);
    void sectionListBoxed(string sTitle, vector<string>& vsList, vector<int>& vColour, int numCol);
    void setFont(string filePath);
    void setFont(string filePath, string filePathItalic);
    void setFontSize(int size);
    void textBox(vector<vector<double>> boxBLTR, string sText, string alignment);
    void textBox(vector<vector<double>> boxBLTR, string sText, string alignment, int fontsize);
};

