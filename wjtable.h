#pragma once
#include <Wt/WEnvironment.h>
#include <Wt/WApplication.h>
#include <Wt/WTableView.h>
#include <Wt/WCssStyleSheet.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/WStandardItem.h>
#include <Wt/WItemDelegate.h>
#include <Wt/WText.h>
#include "jfunc.h"

using namespace std;

class WJDELEGATE : public Wt::WItemDelegate
{
	double height;
	string styleCell = "line-height: 20px; white-space: normal; overflow-y: auto;";
	string temp;

public:
	WJDELEGATE(const double& h) : Wt::WItemDelegate(), height(h) {}
	~WJDELEGATE() {}

	unique_ptr<Wt::WWidget> update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags< Wt::ViewItemRenderFlag > flags)
	{
		int iRow = index.row();
		int iCol = index.column();
		auto widgetUnique = Wt::WItemDelegate::update(widget, index, flags);
		if (!widget)
		{
			if (iCol > 0)
			{
				temp = styleCell + " text-align: right;";
			}
			else
			{
				temp = styleCell + " text-align: left; font-weight: bold;";
			}
			widgetUnique->setAttributeValue("style", temp);
		}
		return widgetUnique;
	}
};
class WJHDELEGATE : public Wt::WItemDelegate
{
	double height;
	string styleCell = "line-height: 20px; white-space: normal; overflow-y: auto;";
	string temp;

	Wt::Signal<int, string> sigHeaderID_;

public:
	WJHDELEGATE(const double& h) : Wt::WItemDelegate(), height(h) {}
	~WJHDELEGATE() {}

	Wt::Signal<int, string>& sigHeaderID() { return sigHeaderID_; }

	unique_ptr<Wt::WWidget> update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags< Wt::ViewItemRenderFlag > flags)
	{
		int iCol = index.column();
		auto widgetUnique = Wt::WItemDelegate::update(widget, index, flags);
		if (!widget)
		{
			if (iCol > 0)
			{
				temp = styleCell + " text-align: left;";
			}
			else
			{
				int iNum = int(height / 2.0) - 10;
				temp = styleCell + " text-align: center;";
				temp += " padding-top: " + to_string(iNum) + "px;";
			}
			widgetUnique->setAttributeValue("style", temp);
			sigHeaderID_.emit(iCol, widgetUnique->id());
		}
		return widgetUnique;
	}
};

class WJTABLE : public Wt::WTableView
{
	const double heightCell = 68.0;
	const double heightHeader = 68.0;
	unordered_map<string, int> mapColIndex, mapRowIndex;  // sHeader -> col/row index
	unordered_map<int, string> mapColUnit, mapRowUnit;  // index -> sUnit (if known)
	unordered_map<int, string> mapHeaderID;  // colIndex -> headerID
	string scSelectedWeak, scSelectedStrong, scWhite;
	Wt::WModelIndex selectedIndex;
	Wt::Signal<int> vClick_;
	Wt::WBorder wbNone, wbSelected;
	Wt::WColor wcSelectedWeak, wcSelectedStrong, wcWhite;
	Wt::WLength wlAuto;

	shared_ptr<WJDELEGATE> wjDel;
	shared_ptr<WJHDELEGATE> wjhDel;

public:
	WJTABLE() : Wt::WTableView() { 
		setRowHeaderCount(1); 
		init();
	}
	~WJTABLE() {}
	enum virtualClickType { ColRowSel };

	JFUNC jf;
	shared_ptr<Wt::WStandardItemModel> model = nullptr;

	Wt::Signal<int>& vClick() { return vClick_; }

	void cellSelect(int iRow, int iCol);
	void cellSelect(Wt::WModelIndex wmIndex);
	void cellSelect(Wt::WModelIndex wmIndex, int iRow, int iCol);
	void display();
	string getCell(int iRow, int iCol);
	int getColIndex(string sHeader);
	vector<int> getRowColSel();
	int getRowIndex(string sHeader);
	string getUnit();
	string getUnit(int iRow, int iCol);
	string getUnit(string header);
	string getUnitParser(string header);
	void headerSelect(int iCol);
	void init();
	void modelSetCore(vector<vector<string>>& vvsData);
	void modelSetLeft(vector<string>& vsRow);
	void modelSetTop(vector<string>& vsCol);
	void modelSetTopLeft(string sRegion);
	void reset();
	void reset(int numRow, int numCol);
	void saveHeader(const int& iCol, const string& sID);
	void setColUnit(string& colHeader, int index);
	void setProperty(Wt::WWidget* widget, string property, string value);
	void setProperty(Wt::WWidget* widget, vector<string> vsProperty, vector<string> vsValue);
	void setRowUnit(string& rowHeader, int index);
	void tableClicked(const Wt::WModelIndex& wmIndex, const Wt::WMouseEvent& wmEvent);
	void tableHeaderClicked(const int& iCol, const Wt::WMouseEvent& wmEvent);
};