#pragma once
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WSpinBox.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WTreeView.h>
#include <Wt/WTable.h>
#include <Wt/WTableCell.h>
#include <Wt/WPanel.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLength.h>
#include <Wt/WTabWidget.h>
#include <Wt/WImage.h>
#include <Wt/WEvent.h>
#include "SCDAserver.h"

using namespace std;
extern const string sroot;

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	string activeCategory, activeColTopic, activeRowTopic, activeYear;
	vector<int> cbActive;
	vector<string> commMap;
	string db_path = sroot + "\\SCDA.db";
	vector<Wt::WString> mapRegionList;
	JFUNC jf;
	JTREE jt;
	enum Layer { Root, Year, Description, Region, Division };
	unordered_map<string, int> mapTok;
	unordered_map<wstring, vector<int>> mapTree;
	const int num_filters = 3;
	Wt::WString selectedRegion, selectedFolder;
	vector<int> selectedCell = { -1,-1 };
	int selectedRow = -1;
	string sessionID, mapRegion;
	vector<int> treeActive;
	enum treeType { Tree, Subtree };
	const Wt::WString wsAll = Wt::WString("All");

	WTPAINT* wtMap;
	Wt::WColor colourSelected, colourWhite;
	Wt::WComboBox *cbCategory, *cbColTopic, *cbColTopicTable, *cbRowTopic, *cbRowTopicTable;
	Wt::WComboBox *cbYear;
	Wt::WImage* imgMap;
	Wt::WVBoxLayout* layoutConfig;
	Wt::WLineEdit* lineEditTest, *lineEditSearch;
	Wt::WPanel *panelCategory, *panelColTopic, *panelRowTopic, *panelYear;
	Wt::WPushButton *pbColTopic, *pbMobile, *pbRowTopic;
	Wt::WSelectionBox* sbList;
	Wt::WSpinBox* spinBoxMapX, *spinBoxMapY, *spinBoxMapRot;
	SCDAserver& sRef;
	Wt::WTable* tableData;
	Wt::WTabWidget* tabData;
	Wt::WText* textMessage, *tableTitle, *textTable, *textList;
	Wt::WTree *treeCata;
	Wt::WTreeNode* treeRoot;

	vector<Wt::WComboBox*> varCB;
	vector<Wt::WPanel*> varPanel;

	void cbCategoryClicked();
	void cbColRowClicked(string id);
	void cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem);
	void cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem, string selItem);
	void cbVariableClicked();
	void cbYearClicked();
	void connect();
	string getVariable();
	void init();
	void initUI();
	void makeUI();
	void mouseMapClick(const Wt::WMouseEvent& e);
	void processDataEvent(const DataEvent& event);
	void selectTableCell(int iRow, int iCol);
	void selectTableRow(int iRow);
	void setTable(vector<string> prompt);
	void tableClicked(Wt::WString wsTable);
	void updateMapRegionList(vector<string>& sList, vector<Wt::WString>& wsList, int mode);

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver) 
	{ 
		init();
	}

	Wt::WLength len5p = Wt::WLength("5px");

	// TEMPLATES

	
};

