#include "SCDAwidget.h"

void SCDAwidget::addVariable(vector<string>& vsVariable)
{
	// This variant is for known values. vsVariable form [title, MID].
	if (vsVariable.size() != 2) { jf.err("Missing prompt-SCDAwidget.addVariable"); }
	Wt::WString wTemp;
	string temp;
	int titleIndex = -1, MIDIndex = -1;
	vector<string> vsTitle(vvsParameter.size());
	for (int ii = 0; ii < vvsParameter.size(); ii++)
	{
		vsTitle[ii] = vvsParameter[ii].back();
		if (vsTitle[ii] == vsVariable[0]) { titleIndex = ii; }
	}
	if (titleIndex < 0) { jf.err("Parameter title not found-SCDAwidget.addVariable"); }
	vector<string> vsMID(vvsParameter[titleIndex].size() - 1);
	for (int ii = 0; ii < vsMID.size(); ii++)
	{
		vsMID[ii] = vvsParameter[titleIndex][ii];
		if (vsMID[ii] == vsVariable[1]) { MIDIndex = ii; }
	}
	if (MIDIndex < 0) { jf.err("Parameter MID not found-SCDAwidget.addVariable"); }

	int index = varPanel.size();
	auto varPanelUnique = make_unique<Wt::WPanel>();
	varPanel.push_back(varPanelUnique.get());
	temp = varPanelUnique->id();
	mapVarIndex.emplace(temp, index);
	auto varBoxUnique = make_unique<Wt::WContainerWidget>();
	auto varVLayoutUnique = make_unique<Wt::WVBoxLayout>();

	int indexTest = varTitle.size();
	if (indexTest != index) { jf.err("varTitle size mismatch-SCDAwidget.addVariable"); }
	auto varCBTitleUnique = make_unique<Wt::WComboBox>();
	varTitle.push_back(varCBTitleUnique.get());
	temp = varCBTitleUnique->id();
	mapVarIndex.emplace(temp, index);
	function<void()> fnVarTitle = bind(&SCDAwidget::cbVarTitleClicked, this, temp);
	varTitle[index]->changed().connect(fnVarTitle);

	indexTest = varMID.size();
	if (indexTest != index) { jf.err("varMID size mismatch-SCDAwidget.addVariable"); }
	auto varCBMIDUnique = make_unique<Wt::WComboBox>();
	varMID.push_back(varCBMIDUnique.get());
	temp = varCBMIDUnique->id();
	mapVarIndex.emplace(temp, index);
	function<void()> fnVarMID = bind(&SCDAwidget::cbVarMIDClicked, this, temp);
	varMID[index]->changed().connect(fnVarMID);

	varPanel[index]->setTitle("Parameter");
	cbRenew(varTitle[index], vsTitle);
	varTitle[index]->setCurrentIndex(titleIndex);
	cbRenew(varMID[index], vsMID);
	varMID[index]->setCurrentIndex(MIDIndex);

	varVLayoutUnique->addWidget(move(varCBTitleUnique));
	varVLayoutUnique->addWidget(move(varCBMIDUnique));
	varBoxUnique->setLayout(move(varVLayoutUnique));
	varPanelUnique->setCentralWidget(move(varBoxUnique));
	layoutConfig->addWidget(move(varPanelUnique));
}
void SCDAwidget::addVariable(vector<vector<string>>& vvsCandidate)
{
	// This variant is for undetermined possibilities. 
	// vvsCandidate form [variable index][
}
void SCDAwidget::cbCategoryClicked()
{
	// Populate tableData with its "input form", containing options for the user
	// to choose column and row topics. 
	resetVariables();
	resetMap();
	resetTable();
	resetTree();
	resetTopicSel();
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(5);
	activeColTopic = "*";
	activeRowTopic = "*";
	int index = cbCategory->currentIndex();
	if (index == 0)
	{
		activeCategory = "*";
		panelColTopic->setHidden(1);
		panelRowTopic->setHidden(1);
		return;
	}
	else
	{
		Wt::WString wsTemp = cbCategory->currentText();
		activeCategory = wsTemp.toUTF8();
	}
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCategory;
	prompt[3] = "*";  // Column topic.
	prompt[4] = "*";  // Row topic.
	sRef.pullTopic(prompt);
}
void SCDAwidget::cbColRowSelClicked()
{
	// Update the data table's selected cell. 
	int numRow = tableData->rowCount();
	int numCol = tableData->columnCount();
	if ( numRow < 1 || numCol < 1) { return; }
	Wt::WText *wText;
	Wt::WString wTemp;
	Wt::WString wsRowSel = cbRowTopicSel->currentText();
	Wt::WString wsColSel = cbColTopicSel->currentText();
	int iRowSel = -1, iColSel = -1;
	if (wsRowSel != wsNoneSel)
	{
		for (int ii = 1; ii < numRow; ii++)
		{
			wText = (Wt::WText*)tableData->elementAt(ii, 0)->widget(0);
			wTemp = wText->text();
			if (wTemp == wsRowSel)
			{
				iRowSel = ii;
				break;
			}
		}
	}
	if (wsColSel != wsNoneSel)
	{
		for (int ii = 1; ii < numCol; ii++)
		{
			wText = (Wt::WText*)tableData->elementAt(0, ii)->widget(0);
			wTemp = wText->text();
			if (wTemp == wsColSel)
			{
				iColSel = ii;
				break;
			}
		}
	}

	if (iRowSel > 0 && iColSel > 0)
	{
		tableDoubleClicked(iRowSel, iColSel);
	}
	else if (iRowSel > 0 || iColSel > 0)
	{
		tableDeselect(selectedRow, selectedCol);
		tableSelect(iRowSel, iColSel);
	}
}
void SCDAwidget::cbColRowTitleClicked(string id)
{
	// When a column or row topic is specified, obtain an updated listing for
	// its opposite (column/row), and for its mirror (side panel/input table).
	resetVariables();
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(5);
	Wt::WString wTemp;
	int index;
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCategory;
	if (id == "panelCol")
	{
		index = cbColTopicTitle->currentIndex();
		if (index == 0)
		{
			activeColTopic = "*";
			activeRowTopic = "*";
		}
		else
		{
			wTemp = cbColTopicTitle->currentText();
			activeColTopic = wTemp.toUTF8();
		}
	}
	else if (id == "panelRow")
	{
		index = cbRowTopicTitle->currentIndex();
		if (index == 0)
		{
			activeRowTopic = "*";
			activeColTopic = "*";
		}
		else
		{
			wTemp = cbRowTopicTitle->currentText();
			activeRowTopic = wTemp.toUTF8();
		}
	}
	else { jf.err("Unknown input id-SCDAwidget-cbColRowClicked"); }
	prompt[3] = activeColTopic;
	prompt[4] = activeRowTopic;

	if (activeRowTopic == "*")
	{
		cbRowTopicSel->clear();
		cbRowTopicSel->setHidden(1);
	}
	if (activeColTopic == "*")
	{
		cbColTopicSel->clear();
		cbColTopicSel->setHidden(1);
	}
	if (activeRowTopic == "*" && activeColTopic == "*")
	{
		resetMap();
		resetTable();
		resetTree();
	}

	// Query the server for the next stage, depending on the current state of specificity.
	if (prompt[3] == "*" || prompt[4] == "*")
	{
		sRef.pullTopic(prompt);  // Rows or columns not yet determined.
	}
	else
	{
		vector<vector<string>> vvsDummy;
		sRef.pullVariable(prompt, vvsDummy);
	}
}
void SCDAwidget::cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem)
{
	// Repopulate the combobox with the given list, showing the top item. 
	cb->clear();
	for (int ii = 0; ii < vsItem.size(); ii++)
	{
		cb->addItem(vsItem[ii]);
	}
	cb->setCurrentIndex(0);
}
void SCDAwidget::cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem)
{
	cbRenew(cb, sTop, vsItem, sTop);  // Default is top item shown. 
}
void SCDAwidget::cbRenew(Wt::WComboBox*& cb, string sTop, vector<vector<string>>& vvsItem)
{
	// Populates the combobox with sTop, followed by the final elements of vvsItem.
	cb->clear();
	cb->addItem(sTop);
	for (int ii = 0; ii < vvsItem.size(); ii++)
	{
		cb->addItem(vvsItem[ii].back());
	}
	cb->setCurrentIndex(0);
}
void SCDAwidget::cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem, string selItem)
{
	// Repopulate the combobox with the selected item shown. 
	int index = -1;
	cb->clear();
	cb->addItem(sTop.c_str());
	for (int ii = 0; ii < vsItem.size(); ii++)
	{
		cb->addItem(vsItem[ii].c_str());
		if (vsItem[ii] == selItem) { index = ii; }
	}
	cb->setCurrentIndex(index + 1);
}
void SCDAwidget::cbVarMIDClicked(string id)
{
	Wt::WApplication* app = Wt::WApplication::instance();
	string sRegion;
	int geoCode;
	if (activeCata.size() > 0)
	{
		treeClicked();
	}
	else
	{
		int bbq = 1;
		/*
		vector<vector<string>> vvsVariable = getVariable();
		vector<string> prompt(5);
		prompt[0] = app->sessionId();
		prompt[1] = activeYear;
		prompt[2] = activeCategory;
		prompt[3] = activeColTopic;
		prompt[4] = activeRowTopic;
		sRef.pullVariable(prompt, vvsVariable);
		*/
	}

}
void SCDAwidget::cbVarTitleClicked(string id)
{
	int removeVar = -1;
	int index = mapVarIndex.at(id);
	Wt::WString wTemp = varTitle[index]->currentText();
	string sTitle;
	string sTitleClicked = wTemp.toUTF8();
	string sTop = "[None selected]";
	if (sTitleClicked == sTop)
	{
		varPanel[index]->setTitle("Select a parameter ...");
		varMID[index]->clear();
		varMID[index]->addItem(sTop);
		varMID[index]->setEnabled(0);
		return;
	}
	else  // If this title already exists in a panel, delete that panel. (There can be only one ... !)
	{
		auto boxChildren = boxConfig->children();
		for (int ii = numPreVariable; ii < boxChildren.size(); ii++)
		{
			if (ii - numPreVariable == index) { continue; }
			wTemp = varTitle[ii - numPreVariable]->currentText();
			sTitle = wTemp.toUTF8();
			if (sTitle == sTitleClicked)
			{
				removeVar = ii - numPreVariable;
				break;
			}
		}
	}
	vector<string> vsMID;
	for (int ii = 0; ii < vvsParameter.size(); ii++)
	{
		if (vvsParameter[ii].back() == sTitleClicked)
		{
			vsMID = vvsParameter[ii];
			break;
		}
	}
	vsMID.pop_back();
	varMID[index]->setEnabled(1);
	cbRenew(varMID[index], vsMID);
	varPanel[index]->setTitle("Parameter");
	cbVarMIDClicked("");
	if (removeVar >= 0)
	{
		Wt::WApplication* app = Wt::WApplication::instance();
		app->processEvents();
		removeVariable(removeVar);
	}
}
void SCDAwidget::cbYearClicked()
{
	Wt::WString wsYear = cbYear->currentText();
	string sYear = wsYear.toUTF8();
	// INCOMPLETE: finish later if more years are to be added.
}
void SCDAwidget::connect()
{
	if (sRef.connect(this, bind(&SCDAwidget::processDataEvent, this, placeholders::_1)))
	{
		Wt::WApplication::instance()->enableUpdates(1);
		sessionID = Wt::WApplication::instance()->sessionId();
	}
}
int SCDAwidget::getHeight()
{
	const Wt::WEnvironment& env = Wt::WApplication::instance()->environment();
	int iHeight = env.screenHeight();
	if (iHeight < 0) { jf.err("Failed to obtain widget dimensions-wtpaint.getDimensions"); }
	return iHeight;
}
Wt::WString SCDAwidget::getTextLegend()
{
	Wt::WString wsLegend = cbColTopicTitle->currentText();
	Wt::WString wsTemp = cbColTopicSel->currentText();
	string temp;
	vector<string> vsSel = { wsTemp.toUTF8() };
	size_t pos1 = vsSel[0].find("Total "), pos2;
	if (pos1 == 0) 
	{ 
		temp = vsSel[0];
		vsSel[0] = "Total"; 
		pos1 = temp.find("($)");
		pos2 = temp.find("(%)");
		if (pos1 < temp.size()) { vsSel[0] += " ($)"; }
		else if (pos2 < temp.size()) { vsSel[0] += " (%)"; }
		else { vsSel[0] += " (# of persons)"; }
	}
	else
	{
		pos1 = vsSel[0].find("($)");
		pos2 = vsSel[0].find("(%)");
		if (pos1 > vsSel[0].size() && pos2 > vsSel[0].size())
		{ 
			vsSel[0] += " (# of persons)";
		}
	}

	int countLast = 0, countTemp, index;
	while (vsSel[0][0] == '+') 
	{ 
		countLast++;
		vsSel[0].erase(vsSel[0].begin());
	}
	countLast--;
	index = cbColTopicSel->currentIndex();
	while (countLast > 0)
	{
		index--;
		wsTemp = cbColTopicSel->itemText(index);
		temp = wsTemp.toUTF8();
		countTemp = 0;
		while (temp[0] == '+')
		{
			countTemp++;
			temp.erase(temp.begin());
		}
		if (countTemp == countLast)
		{
			vsSel.push_back(temp);
			countLast--;
		}
	}

	for (int ii = vsSel.size() - 1; ii >= 0; ii--)
	{
		wsLegend += " | " + vsSel[ii];
	}

	return wsLegend;
}
vector<vector<string>> SCDAwidget::getVariable()
{
	// Checks the active state of each "variable" panel and returns each title and MID.
	int numVar = varPanel.size();
	vector<vector<string>> vvsVariable(numVar);
	string sTitle, sMID;
	Wt::WString wTemp;
	for (int ii = 0; ii < numVar; ii++)
	{
		wTemp = varTitle[ii]->currentText();
		sTitle = wTemp.toUTF8();
		wTemp = varMID[ii]->currentText();
		sMID = wTemp.toUTF8();
		vvsVariable[ii] = { sTitle, sMID };
	}
	return vvsVariable;
}
int SCDAwidget::getWidth()
{
	const Wt::WEnvironment& env = Wt::WApplication::instance()->environment();
	int iWidth = env.screenWidth();
	if (iWidth < 0) { jf.err("Failed to obtain widget dimensions-wtpaint.getDimensions"); }
	return iWidth;
}
void SCDAwidget::init()
{
	connect();
	makeUI();
	initUI();
}
void SCDAwidget::initUI()
{
	Wt::WString wstemp;
	string temp;
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt = { app->sessionId() };
	string sPath = app->docRoot();
	sPath += "\\SCDA-Wt.css";
	auto cssLink = Wt::WLink(sPath);
	app->useStyleSheet(cssLink);

	// Colourful things.
	colourSelectedWeak.setRgb(200, 200, 255);
	colourSelectedStrong.setRgb(150, 150, 192);
	colourWhite.setRgb(255, 255, 255, 255);

	// Initial values for widget sizes.
	boxConfig->setMaximumSize(len200p, wlAuto);
	boxConfig->setMinimumSize(len300p, wlAuto);
	boxMap->setStyleClass("paintMap");
	boxMap->setStyleClass("box");
	//boxData->setMinimumSize(len800p, len800p);
	//boxMap->setMinimumSize(len700p, len700p);

	// Initial values for cbYear.
	activeYear = "2016";  // Default for now, as it is the only possibility.
	panelYear->setTitle("Census Year");
	cbYear->addItem(activeYear);
	cbYear->setCurrentIndex(0);
	cbYear->setEnabled(0);
	cbYear->changed().connect(this, &SCDAwidget::cbYearClicked);

	// Initial values for the category panel.
	panelCategory->setTitle("Topical Category");
	vector<string> vsTopic = sRef.getTopicList(activeYear);
	cbCategory->addItem("[Choose a topical category]");
	for (int ii = 0; ii < vsTopic.size(); ii++)
	{
		cbCategory->addItem(vsTopic[ii].c_str());
	}
	cbCategory->changed().connect(this, &SCDAwidget::cbCategoryClicked);

	// Initial values for the row panel.
	panelRowTopic->setTitle("Table Row Topic");
	temp = "panelRow";
	function<void()> fnRowTopicTitle = bind(&SCDAwidget::cbColRowTitleClicked, this, temp);
	cbRowTopicTitle->changed().connect(fnRowTopicTitle);
	panelRowTopic->setHidden(1);
	cbRowTopicSel->changed().connect(this, &SCDAwidget::cbColRowSelClicked);

	// Initial values for the column panel.
	panelColTopic->setTitle("Table Column Topic");
	temp = "panelCol";
	function<void()> fnColTopicTitle = bind(&SCDAwidget::cbColRowTitleClicked, this, temp);
	cbColTopicTitle->changed().connect(fnColTopicTitle);
	panelColTopic->setHidden(1);
	cbColTopicSel->changed().connect(this, &SCDAwidget::cbColRowSelClicked);

	// Initial values for the tab widget.
	tabData->setTabEnabled(0, 0);
	tabData->setTabEnabled(1, 0);
	tabData->setTabEnabled(2, 0);
	tabData->setCurrentIndex(0);

	// Initial values for the region tree widget.
	treeRegion->itemSelectionChanged().connect(this, &SCDAwidget::treeClicked);

	// Initial values for the buttons.
	pbMobile->setEnabled(0);
	pbMobile->clicked().connect(this, &SCDAwidget::test);

}
void SCDAwidget::makeUI()
{
	this->clear();
	auto hLayout = make_unique<Wt::WHBoxLayout>();

	auto boxConfigUnique = make_unique<Wt::WContainerWidget>();
	boxConfig = boxConfigUnique.get();
	auto vLayoutConfig = make_unique<Wt::WVBoxLayout>();
	layoutConfig = vLayoutConfig.get();
	auto boxTestUnique = make_unique<Wt::WContainerWidget>();
	boxTest = boxTestUnique.get();
	auto hLayoutTestUnique = make_unique<Wt::WHBoxLayout>();
	auto pbMobileUnique = make_unique<Wt::WPushButton>("Toggle Mobile Version");
	pbMobile = pbMobileUnique.get();
	auto leTestUnique = make_unique<Wt::WLineEdit>();
	leTest = leTestUnique.get();
	auto panelYearUnique = make_unique<Wt::WPanel>();
	panelYear = panelYearUnique.get();
	auto cbYearUnique = make_unique<Wt::WComboBox>();
	cbYear = cbYearUnique.get();
	auto panelCategoryUnique = make_unique<Wt::WPanel>();
	panelCategory = panelCategoryUnique.get();
	auto cbCategoryUnique = make_unique<Wt::WComboBox>();
	cbCategory = cbCategoryUnique.get();
	auto panelRowTopicUnique = make_unique<Wt::WPanel>();
	panelRowTopic = panelRowTopicUnique.get();
	auto boxRowTopicUnique = make_unique<Wt::WContainerWidget>();
	auto cbRowTopicTitleUnique = make_unique<Wt::WComboBox>();
	cbRowTopicTitle = cbRowTopicTitleUnique.get();
	auto cbRowTopicSelUnique = make_unique<Wt::WComboBox>();
	cbRowTopicSel = cbRowTopicSelUnique.get();
	auto panelColTopicUnique = make_unique<Wt::WPanel>();
	panelColTopic = panelColTopicUnique.get();
	auto boxColTopicUnique = make_unique<Wt::WContainerWidget>();
	auto cbColTopicTitleUnique = make_unique<Wt::WComboBox>();
	cbColTopicTitle = cbColTopicTitleUnique.get();
	auto cbColTopicSelUnique = make_unique<Wt::WComboBox>();
	cbColTopicSel = cbColTopicSelUnique.get();

	auto boxDataUnique = make_unique<Wt::WContainerWidget>();
	boxData = boxDataUnique.get();
	auto tabDataUnique = make_unique<Wt::WTabWidget>();
	tabData = tabDataUnique.get();
	auto treeRegionUnique = make_unique<Wt::WTree>();
	treeRegion = treeRegionUnique.get();
	auto tableDataUnique = make_unique<Wt::WTable>();
	tableData = tableDataUnique.get();
	auto boxMapUnique = make_unique<Wt::WContainerWidget>();
	boxMap = boxMapUnique.get();

	hLayoutTestUnique->addWidget(move(pbMobileUnique));
	hLayoutTestUnique->addWidget(move(leTestUnique), 1);
	boxTestUnique->setLayout(move(hLayoutTestUnique));

	panelYearUnique->setCentralWidget(move(cbYearUnique));
	panelCategoryUnique->setCentralWidget(move(cbCategoryUnique));

	boxRowTopicUnique->addWidget(move(cbRowTopicTitleUnique));
	boxRowTopicUnique->addWidget(move(cbRowTopicSelUnique));
	panelRowTopicUnique->setCentralWidget(move(boxRowTopicUnique));

	boxColTopicUnique->addWidget(move(cbColTopicTitleUnique));
	boxColTopicUnique->addWidget(move(cbColTopicSelUnique));
	panelColTopicUnique->setCentralWidget(move(boxColTopicUnique));

	vLayoutConfig->addWidget(move(boxTestUnique));
	vLayoutConfig->addWidget(move(panelYearUnique));
	vLayoutConfig->addWidget(move(panelCategoryUnique));
	vLayoutConfig->addWidget(move(panelRowTopicUnique));
	vLayoutConfig->addWidget(move(panelColTopicUnique));
	numPreVariable = vLayoutConfig->count();
	boxConfigUnique->setLayout(move(vLayoutConfig));

	tabDataUnique->addTab(move(treeRegionUnique), "Geographic Region");
	tabDataUnique->addTab(move(tableDataUnique), "Data Table");
	tabDataUnique->addTab(move(boxMapUnique), "Data Map");
	boxDataUnique->addWidget(move(tabDataUnique));

	hLayout->addWidget(move(boxConfigUnique), 1);
	hLayout->addWidget(move(boxDataUnique), 2);
	this->setLayout(move(hLayout));
}
void SCDAwidget::mapAreaClicked(int areaIndex)
{
	string sRegion = wtMap->areaClicked(areaIndex);
	string sID = jtRegion.mapSID.at(sRegion);
	Wt::WTreeNode* nodeSel = (Wt::WTreeNode*)treeRegion->findById(sID);
	if (!treeRegion->isSelected(nodeSel)) { treeRegion->select(nodeSel); }
	treeClicked();
}
void SCDAwidget::populateTree(JTREE& jt, Wt::WTreeNode*& node)
{
	// Recursive function that takes an existing node and makes its children.
	vector<string> vsChildren;
	vector<int> viChildren;
	Wt::WString wTemp = node->label()->text();
	string sNode = wTemp.toUTF8();
	int iNode = jt.getIName(sNode);
	string sID = node->id();
	jt.mapSID.emplace(sNode, sID);
	jt.listChildren(sNode, viChildren, vsChildren);
	for (int ii = 0; ii < vsChildren.size(); ii++)
	{
		wTemp = Wt::WString::fromUTF8(vsChildren[ii]);
		auto childUnique = make_unique<Wt::WTreeNode>(wTemp);
		auto child = childUnique.get();
		populateTree(jt, child);
		node->addChildNode(move(childUnique));
	}
}
void SCDAwidget::processDataEvent(const DataEvent& event)
{
	jf.timerStart();
	Wt::WApplication* app = Wt::WApplication::instance();
	string sPath = app->docRoot();
	sPath += "/SCDA-Wt.css";
	auto cssLink = Wt::WLink(sPath);
	app->useStyleSheet(cssLink);
	app->triggerUpdate();
	vector<vector<vector<double>>> areas;
	string temp;
	string sessionID = app->sessionId(), nameAreaSel;
	vector<string> slist, listCol, listRow, vsDIMIndex;
	vector<vector<string>> table, vvsCata, vvsVariable;
	long long timer;
	vector<int> tempIndex;
	vector<double> regionData;
	int inum, maxCol;

	int etype = event.type();
	switch (etype)
	{
	case 0:  // Connect.
		break;
	case 1:  // Label: display it.
	{
		break;
	}
	case 2:  // Map: display it on the painter widget.
	{
		slist = event.get_list();  // vsRegionName.
		areas = event.get_areas();  // Border coords.
		regionData = event.get_regionData();  // vdData.
		boxMap->clear();
		auto mapVLayout = make_unique<Wt::WVBoxLayout>();
		auto wtMapUnique = make_unique<WTPAINT>();
		wtMap = mapVLayout->addWidget(move(wtMapUnique));
		vector<Wt::WPolygonArea*> area = wtMap->drawMap(areas, slist, regionData);
		for (int ii = 0; ii < area.size(); ii++)
		{
			function<void()> fnArea = bind(&SCDAwidget::mapAreaClicked, this, ii);
			area[ii]->clicked().connect(fnArea);
		}

		Wt::WString wsLegend = getTextLegend();
		auto textLegendUnique = make_unique<Wt::WText>(wsLegend);
		textLegendUnique->setTextAlignment(Wt::AlignmentFlag::Center);
		textLegend = mapVLayout->addWidget(move(textLegendUnique));
		boxMap->setLayout(move(mapVLayout));
		tabData->setTabEnabled(2, 1);
		tabData->setCurrentIndex(2);
		break;
	}
	case 3:  // Table: populate the widget with data.
	{
		table = event.getTable();
		listCol = event.getListCol();
		listRow = event.getListRow();
		activeTableColTitle = listCol.back();
		listCol.pop_back();
		activeTableRowTitle = listRow.back();
		listRow.pop_back();

		bool addTopicSel = 0;
		if (cbRowTopicSel->isHidden() || cbColTopicSel->isHidden())
		{
			cbRowTopicSel->setHidden(0);
			cbColTopicSel->setHidden(0);
			addTopicSel = 1;
		}
		
		Wt::WString wTemp;
		tableData->clear();
		tableData->setHeaderCount(1, Wt::Orientation::Horizontal);
		tableData->setHeaderCount(1, Wt::Orientation::Vertical);
		for (int ii = 0; ii < listCol.size(); ii++)
		{
			tableData->elementAt(0, ii)->addNew<Wt::WText>(listCol[ii]);
			if (addTopicSel && ii > 0) { cbColTopicSel->addItem(listCol[ii]); }
		}
		for (int ii = 0; ii < table.size(); ii++)
		{
			tableData->elementAt(ii + 1, 0)->addNew<Wt::WText>(listRow[ii]);
			if (addTopicSel) { cbRowTopicSel->addItem(listRow[ii]); }
			for (int jj = 0; jj < table[ii].size(); jj++)
			{
				auto textUnique = make_unique<Wt::WText>(table[ii][jj]);
				function<void()> fnSingle = bind(&SCDAwidget::tableClicked, this, ii + 1, jj + 1);
				textUnique->clicked().connect(fnSingle);
				function<void()> fnDouble = bind(&SCDAwidget::tableDoubleClicked, this, ii + 1, jj + 1);
				textUnique->doubleClicked().connect(fnDouble);
				tableData->elementAt(ii + 1, jj + 1)->addWidget(move(textUnique));
			}
		}
		tabData->setTabEnabled(1, 1);
		temp = "Data Table (" + listCol[0] + ")";
		wTemp = Wt::WString::fromUTF8(temp);
		auto tab = tabData->itemAt(1);
		tab->setText(wTemp);
		cbColRowSelClicked();
		break;
	}
	case 4:  // Topic: update the GUI with options.
	{
		listCol = event.getListCol();
		listRow = event.getListRow();

		string colDefault = "[Choose a column topic]";
		string rowDefault = "[Choose a row topic]";
		if (listCol.size() == 1)
		{
			activeColTopic = listCol[0];
		}
		cbRenew(cbColTopicTitle, colDefault, listCol, activeColTopic);
		if (listRow.size() == 1)
		{
			activeRowTopic = listRow[0];
		}
		cbRenew(cbRowTopicTitle, rowDefault, listRow, activeRowTopic);

		if (listCol.size() == 1 && listRow.size() == 1)
		{
			vector<vector<string>> vvsVariable;
			vector<string> prompt(5);
			prompt[0] = app->sessionId();
			prompt[1] = activeYear;
			prompt[2] = activeCategory;
			prompt[3] = activeColTopic;
			prompt[4] = activeRowTopic;
			sRef.pullVariable(prompt, vvsVariable);
		}
		break;
	}
	case 5:  // Tree: populate the tree tab using the JTREE object.
	{
		jtRegion.clear();
		jtRegion = event.getTree();
		string sRoot = jtRegion.getRootName();
		Wt::WString wTemp = Wt::WString::fromUTF8(sRoot);
		auto treeRootUnique = make_unique<Wt::WTreeNode>(wTemp);
		treeRootUnique->setLoadPolicy(Wt::ContentLoading::Eager);
		auto treeRoot = treeRootUnique.get();
		populateTree(jtRegion, treeRoot);
		treeRegion->setSelectionMode(Wt::SelectionMode::Single);
		treeRegion->setTreeRoot(move(treeRootUnique));
		tabData->setTabEnabled(0, 1);
		treeRoot = treeRegion->treeRoot();
		treeRoot->expand();
		treeRegion->select(treeRoot);
		break;
	}
	case 6:  // Variable: create new panels with options for the user to specify.
	{
		vvsVariable = event.getVariable();  // Form [variable index][MID0, MID1, ..., variable title].
		if (vvsVariable.size() < 1) { jf.err("Variable event missing input-SCDAwidget.processDataEvent"); }
		vvsCata = event.getCata();
		vsDIMIndex = event.get_list();
		int numCata = 0;
		for (int ii = 0; ii < vvsCata.size(); ii++)
		{
			numCata += vvsCata[ii].size() - 1;
		}

		int index = varPanel.size();
		int numVar = -1;
		if (numCata == 1)
		{
			// If only one catalogue satisfies the conditions, then load all variables locally.
			activeCata = vvsCata[0][1];
			if (mapNumVar.count(vvsCata[0][1]))
			{
				numVar = mapNumVar.at(vvsCata[0][1]);
			}
			else
			{
				numVar = vsDIMIndex.size() - 2;
				mapNumVar.emplace(vvsCata[0][1], numVar);
			}
			vector<string> vsTemp(2);
			if (!index) { vvsParameter = vvsVariable; }
			for (int ii = 0; ii < vvsVariable.size(); ii++)
			{
				vsTemp[0] = vvsVariable[ii].back();  // Title.
				vsTemp[1] = vvsVariable[ii][0];  // Default MID.
				addVariable(vsTemp);
			}
			for (int ii = index; ii < varTitle.size(); ii++)
			{
				varTitle[ii]->setEnabled(0);
			}
		}
		else
		{
			// Multiple catalogues satisfy the Year, Category, Row, and Column criteria.
			// Offer the user one parameter at a time, until only a single catalogue remains.
			int bbq = 1;
		}

		// If there are no more unspecified variables, populate the region tree tab.
		if (varPanel.size() == numVar)
		{
			app->processEvents();
			activeYear = vvsCata[0][0];
			activeCata = vvsCata[0][1];
			vector<string> promptTree(3);
			promptTree[0] = app->sessionId();
			promptTree[1] = vvsCata[0][0];
			promptTree[2] = vvsCata[0][1];
			sRef.pullTree(promptTree);
		}

		break;
	}
	}
	timer = jf.timerStop();
	jf.logTime("processDataEvent#" + to_string(etype), timer);
}
void SCDAwidget::removeVariable(int varIndex)
{
	// Remove the given variable's panel widget. Update mapVarIndex, varPanel, varTitle, varMID;
	int index;
	string id;
	Wt::WString wTemp;
	auto varPanelTemp = varPanel;
	varPanel.clear();
	auto varTitleTemp = varTitle;
	varTitle.clear();
	auto varMIDTemp = varMID;
	varMID.clear();
	mapVarIndex.clear();
	for (int ii = 0; ii < varPanelTemp.size(); ii++)
	{
		if (ii == varIndex) { continue; }
		index = varPanel.size();
		varPanel.push_back(varPanelTemp[ii]);
		id = varPanel[index]->id();
		mapVarIndex.emplace(id, index);
		varTitle.push_back(varTitleTemp[ii]);
		id = varTitle[index]->id();
		mapVarIndex.emplace(id, index);
		varMID.push_back(varMIDTemp[ii]);
		id = varMID[index]->id();
		mapVarIndex.emplace(id, index);
	}
	auto boxChildren = boxConfig->children();
	boxConfig->removeWidget(boxChildren[numPreVariable + varIndex]);
	Wt::WApplication* app = Wt::WApplication::instance();
	app->processEvents();
}
void SCDAwidget::resetMap()
{
	boxMap->clear();
	tabData->setTabEnabled(2, 0);
	tabData->setCurrentIndex(0);
}
void SCDAwidget::resetTable()
{
	tableData->clear();
	Wt::WString wTemp("Data Table");
	auto tab = tabData->itemAt(1);
	tab->setText(wTemp);
	tabData->setTabEnabled(1, 0);
	tabData->setCurrentIndex(0);
}
void SCDAwidget::resetTopicSel()
{
	panelRowTopic->setHidden(0);
	cbRowTopicSel->clear();
	cbRowTopicSel->setHidden(1);
	panelColTopic->setHidden(0);
	cbColTopicSel->clear();
	cbColTopicSel->setHidden(1);
}
void SCDAwidget::resetTree()
{
	jtRegion.clear();
	treeRegion->setTreeRoot(make_unique<Wt::WTreeNode>(""));
}
void SCDAwidget::resetVariables()
{
	// Remove all "variable" panels and clear all "variable" buffers.
	auto boxChildren = boxConfig->children();
	for (int ii = numPreVariable; ii < boxChildren.size(); ii++)
	{
		boxConfig->removeWidget(boxChildren[ii]);
	}
	activeCata.clear();
	vvsParameter.clear();
	varPanel.clear();
	varTitle.clear();
	varMID.clear();
	mapVarIndex.clear();
	Wt::WApplication* app = Wt::WApplication::instance();
	app->processEvents();
}
void SCDAwidget::setTable(int geoCode, string sRegion)
{
	// This variant loads a table using a selected region from the tree. 
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(5);
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCata;
	prompt[3] = to_string(geoCode);
	prompt[4] = sRegion;
	vector<vector<string>> variable = getVariable();
	sRef.pullTable(prompt, variable);
	tabData->setTabEnabled(1, 1);
}
void SCDAwidget::tableCBUpdate(int iRow, int iCol)
{
	int count;
	Wt::WText *wText;
	Wt::WString wsCB, wsTable;
	if (iRow < 0) { cbRowTopicSel->setCurrentIndex(0); }
	else
	{
		wText = (Wt::WText*)tableData->elementAt(iRow, 0)->widget(0);
		wsTable = wText->text();
		count = cbRowTopicSel->count();
		for (int ii = 0; ii < count; ii++)
		{
			wsCB = cbRowTopicSel->itemText(ii);
			if (wsCB == wsTable)
			{
				cbRowTopicSel->setCurrentIndex(ii);
				break;
			}
		}
	}
	if (iCol < 0) { cbColTopicSel->setCurrentIndex(0); }
	else
	{
		wText = (Wt::WText*)tableData->elementAt(0, iCol)->widget(0);
		wsTable = wText->text();
		count = cbColTopicSel->count();
		for (int ii = 0; ii < count; ii++)
		{
			wsCB = cbColTopicSel->itemText(ii);
			if (wsCB == wsTable)
			{
				cbColTopicSel->setCurrentIndex(ii);
				break;
			}
		}
	}
}
void SCDAwidget::tableClicked(int iRow, int iCol)
{
	if (iRow <= 0 || iCol <= 0) { return; }
	tableDeselect(selectedRow, selectedCol);
	tableSelect(iRow, iCol);
	tableCBUpdate(iRow, iCol);
}
void SCDAwidget::tableDeselect(int iRow, int iCol)
{
	// Remove highlighting from the given row and column. 
	if (iRow >= 0)
	{
		int numCol = tableData->columnCount();
		for (int ii = 0; ii < numCol; ii++)
		{
			tableData->elementAt(iRow, ii)->decorationStyle().setBackgroundColor(colourWhite);
		}
		selectedRow = -1;
	}
	if (iCol >= 0)
	{
		int numRow = tableData->rowCount();
		for (int ii = 0; ii < numRow; ii++)
		{
			tableData->elementAt(ii, iCol)->decorationStyle().setBackgroundColor(colourWhite);
		}
		selectedCol = -1;
	}
	if (iRow >= 0 && iCol >= 0)
	{
		Wt::WBorder border = Wt::WBorder();
		tableData->elementAt(iRow, iCol)->decorationStyle().setBorder(border);
	}
}
void SCDAwidget::tableDoubleClicked(int iRow, int iCol)
{
	tableClicked(iRow, iCol);  // Highlight.
	Wt::WText* cell = (Wt::WText*)tableData->elementAt(0, 0)->widget(0);
	Wt::WString wTemp = cell->text();
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(4);
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCata;
	prompt[3] = wTemp.toUTF8();  // Parent region name. 

	vector<vector<string>> variable = getVariable();
	cell = (Wt::WText*)tableData->elementAt(iRow, 0)->widget(0);
	wTemp = cell->text();
	string temp = wTemp.toUTF8();
	variable.push_back({ activeTableRowTitle, temp });
	variable.push_back({ activeTableColTitle, to_string(iCol) });
	sRef.pullMap(prompt, variable);
}
void SCDAwidget::tableSelect(int iRow, int iCol)
{
	// Highlight the given row and column. 
	if (iRow >= 0)
	{
		int numCol = tableData->columnCount();
		for (int ii = 0; ii < numCol; ii++)
		{
			tableData->elementAt(iRow, ii)->decorationStyle().setBackgroundColor(colourSelectedWeak);
		}
		selectedRow = iRow;
	}
	if (iCol >= 0)
	{
		int numRow = tableData->rowCount();
		for (int ii = 0; ii < numRow; ii++)
		{
			tableData->elementAt(ii, iCol)->decorationStyle().setBackgroundColor(colourSelectedWeak);
		}
		selectedCol = iCol;
	}
	if (iRow >= 0 && iCol >= 0)
	{
		Wt::WBorder border = Wt::WBorder(Wt::BorderStyle::Inset, Wt::BorderWidth::Medium, colourSelectedStrong);
		tableData->elementAt(iRow, iCol)->decorationStyle().setBorder(border);
	}
}
void SCDAwidget::test()
{

}
void SCDAwidget::treeClicked()
{
	auto selSet = treeRegion->selectedNodes();
	if (selSet.size() < 1) { return; }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	string sRegion = wTemp.toUTF8();
	int geoCode = jtRegion.getIName(sRegion);
	setTable(geoCode, sRegion);
}