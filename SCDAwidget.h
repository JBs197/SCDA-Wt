#pragma once
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WSpinBox.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WTable.h>
#include <Wt/WTableCell.h>
#include <Wt/WPanel.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLength.h>
#include <Wt/WTabWidget.h>
#include <Wt/WImage.h>
#include "SCDAserver.h"

using namespace std;
extern const string sroot;

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	string activeDesc, activeDiv, activeRegion, activeYear;
	vector<int> cbActive;
	string db_path = sroot + "\\SCDA.db";
	vector<Wt::WString> defNames;
	JFUNC jf;
	JTREE jt;
	enum Layer { Root, Year, Description, Region, Division };
	unordered_map<string, int> mapTok;
	unordered_map<wstring, vector<int>> mapTree;
	const int num_filters = 3;
	Wt::WString selectedRegion, selectedFolder;
	int selectedRow = -1;
	string sessionID;
	vector<int> treeActive;
	enum treeType { Tree, Subtree };
	const Wt::WString wsAll = Wt::WString("All");

	WTFUNC *wtMap;
	Wt::WColor colourSelected, colourWhite;
	Wt::WContainerWidget *boxControl, *boxTreelist, *boxTable, *boxText, *boxButtonTest, *boxButtonTable, *boxLineEdit, *boxButtonMap, *boxMap, *boxMapControl;
	Wt::WComboBox *cbYear, *cbDesc, *cbRegion, *cbDiv;
	Wt::WImage* imgMap;
	Wt::WLineEdit* lineEdit;
	//Wt::WPaintedWidget* wtMap;
	Wt::WPanel *panelYear, *panelDesc, *panelRegion, *panelDiv;
	Wt::WPushButton *pbTest, *pbTable, *pbMap;
	Wt::WSelectionBox* sbList;
	Wt::WSpinBox* spinBoxMapX, *spinBoxMapY, *spinBoxMapRot;
	SCDAserver& sRef;
	Wt::WTable* wtTable;
	Wt::WTabWidget* treeTab;
	Wt::WText* textTest, *tableTitle, *mapTitle, *textSelRegion, *textSBX, *textSBY, *textSBRot;
	Wt::WTree *treeCata, *treeData;
	Wt::WTreeNode* treeRoot;

	void cbDescClicked();
	void cbDivClicked();
	void cbRegionClicked();
	void cbYearClicked();
	void connect();
	void folderClicked(Wt::WString wsTable);
	void init();
	void initUI(int);
	void makeUI();
	void pbMapClicked();
	void pbTestClicked();
	void pbTableClicked();
	void processDataEvent(const DataEvent& event);
	void selectTableRow(int iRow);
	void setLayer(Layer layer, vector<string> prompt);
	void setTable(vector<string> prompt);
	void tableClicked(Wt::WString wsTable);

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver) 
	{ 
		init();
	}

	Wt::WLength len5p = Wt::WLength("5px");

	// TEMPLATES

	
};

