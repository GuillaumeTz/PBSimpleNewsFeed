/* Copyright(C) 2025 guillaume.taze@proton.me

This program is free software : you can redistribute it and /or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < https://www.gnu.org/licenses/>.
*/

#include "CApp.h"
#include "CDownload.h"

#include "CUiFeedList.h"
#include "CUiReaderModeBrowser.h"
#include "CUiLastEntries.h"

#include "ui/CUiText.h"
#include "ui/CUiVerticalBox.h"
#include "ui/CUiButton.h"
#include "ui/CUiContextMenu.h"

#include "inkview.h"

#include <sys/stat.h>
#include <sstream>

CApp::CApp()
{
	WidgetPushDown = NULL;
	NbDownloadFinished = 0;
	NbTotalDownload = 0;
	bSyncAllInProgress = false;
}

CApp* CApp::Singleton = NULL;

#ifdef POCKETBOOK_PRO_FW5
	#define PB_BROWSER_SCRIPT SYSTEMDATA"/bin/webbrowser.sh"
	#define PB_BROWSER_EXEC_SCRIPT SYSTEMDATA"/bin/run_script"
#endif
#define PB_BROWSER_BINARY SYSTEMDATA"/bin/browser.app"
#define PB_BROWSER_EXEC SYSTEMDATA"/bin/openbook"

std::string char2hex( char dec )
{
	char dig1 = (dec&0xF0)>>4;
	char dig2 = (dec&0x0F);
	if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
	if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
	if ( 0<= dig2 && dig2<= 9) dig2+=48;
	if (10<= dig2 && dig2<=15) dig2+=97-10;
 
	std::string r;
	r.append( &dig1, 1);
	r.append( &dig2, 1);
	return r;
}

std::string urlencode(const std::string &c)
{
	std::string escaped="";
	int max = c.length();
	for(int i=0; i<max; i++)
	{
		if ( (48 <= c[i] && c[i] <= 57) ||//0-9
			 (65 <= c[i] && c[i] <= 90) ||//abc...xyz
			 (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
			 (c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'') //~!*()'
		)
		{
			escaped.append( &c[i], 1);
		}
		else
		{
			escaped.append("%");
			escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
		}
	}
	return escaped;
}
//-----------------------------------------------------------------------------


void pbLaunchWaitBinary(const char *binary, const char *param1, const char *param2, const char *param3) 
{
	pid_t cpid;
	pid_t child_pid;
	cpid = fork();

	switch (cpid) {
		case -1:
			std::cerr << "pbLaunchWaitBinary(): Fork failed!" << std::endl;
			break;

		case 0:
			child_pid = getpid();
			std::cerr << "pbLaunchWaitBinary(): Child: PID " << child_pid << std::endl;
			std::cerr << "pbLaunchWaitBinary(): Child: Launch " << binary << std::endl;
			execl(
				binary,
				binary,
				param1,
				param2,
				param3,
				NULL
				);
			exit(0);

		default:
			std::cerr << "pbLaunchWaitBinary(): Parent: Waiting for " << cpid << " to finish" << std::endl;
			waitpid(cpid, NULL, 0);
			std::cerr << "pbLaunchWaitBinary(): Parent: Returned from " << binary << std::endl;

			#ifdef POCKETBOOK_PRO_FW5

			if( isBrowserScriptSupported() ) {
				std::cerr << "pbLaunchWaitBinary(): Parent: Restore system panel state (failsafe)" << std::endl;
			}

			#endif

			ClearScreen();
			FullUpdate();
			CApp::Get()->Draw();
	}
}
void pbLaunchWaitBinary(const char *binary, const char *param1, const char *param2) 
{
	pbLaunchWaitBinary(binary, param1, param2, "");
}
void pbLaunchWaitBinary(const char *binary, const char *param) 
{
	pbLaunchWaitBinary(binary, param, "");
}
void pbLaunchWaitBinary(const char *binary) 
{
	pbLaunchWaitBinary(binary, "");
}

bool isBrowserSupported() 
{
	return access( PB_BROWSER_BINARY, F_OK ) != -1;
}

#ifdef POCKETBOOK_PRO_FW5
bool isBrowserScriptSupported() 
{
	return access( PB_BROWSER_EXEC_SCRIPT, F_OK ) != -1 && access( PB_BROWSER_SCRIPT, F_OK ) != -1;
}
#endif

void launchBrowser(const std::string& url) 
{
	std::string GoodUrl = url;

	std::cerr << "Launch browser : " << GoodUrl << std::endl;

//#ifdef POCKETBOOK_PRO_FW5
//	if( isBrowserScriptSupported() ) {
//		pbLaunchWaitBinary(PB_BROWSER_EXEC_SCRIPT, PB_BROWSER_SCRIPT, GoodUrl.c_str());
//		return;
//	}
//#endif
	if( isBrowserSupported() ) {
		pbLaunchWaitBinary(PB_BROWSER_BINARY, GoodUrl.c_str());
	}
	else {
		Message(ICON_WARNING,  const_cast<char*>("CoolReader"), "Couldn't find the browser binary @ " PB_BROWSER_BINARY, 2000);
	}
}

void CApp::msg(const char *s) 
{
	FillArea(350, 770, 250, 20, WHITE);
	SetFont(AppSettings.DefaultFont.Font, BLACK);
	DrawString(350, 770, s);
	PartialUpdate(350, 770, 250, 62);
}

int CApp::App_Handler(int type, int par1, int par2)
{
	//std::cerr << "Received " << type << " " << par1 << " " << par2;

	if (type == EVT_INIT)
	{
		std::cerr << " INIT";
		CApp::Get()->Init();
	}

	if (type == EVT_EXIT)
	{
		std::cerr << " EXIT";
		CApp::Get()->QuitApplication();
	}

	if (type == EVT_BACKGROUND)
	{
		std::cerr << " BACKGROUND";
		CApp::Get()->Save();
	}

	if (type == EVT_SHOW || type == EVT_REPAINT) 
	{
		std::cerr << " SHOW";
		CApp::Get()->Draw();
		FullUpdate();
	}

	if (type == EVT_KEYPRESS)
	{
		//std::stringstream stream;
		//stream << "Keypress " << par1 << " " << par2;
		//CApp::Get()->msg(stream.str().c_str());
		if (par1 == 20 || par1 == 25)
		{
			CApp::Get()->NextPage();
		}
		else if (par1 == 19 || par1 == 24)
		{
			CApp::Get()->PreviousPage();
		}
		else
		{
			//CApp::Get()->QuitApplication();
		}
	}

	if (type == EVT_TOUCHDOWN || type == EVT_POINTERDOWN)
	{
		CApp::Get()->OnTouchDown(SVector2i(par1, par2));
	}

	if (type == EVT_POINTERLONG)
	{
		CApp::Get()->OnTouchLong(SVector2i(par1, par2));
	}

	if (type == EVT_TOUCHUP || type == EVT_POINTERUP)
	{
		CApp::Get()->OnTouchUp(SVector2i(par1, par2));
	}

	if (type == EVT_NEXTPAGE)
	{
		CApp::Get()->NextPage();
	}
	else if (type == EVT_PREVPAGE)
	{
		CApp::Get()->PreviousPage();
	}

	//std::cerr << std::endl;

	return 0;
}

void CApp::Main()
{
	OpenScreen();

	InkViewMain(CApp::App_Handler);
}

CApp* CApp::Get()
{
	if (!Singleton)
	{
		Singleton = new CApp();
	}
	return Singleton;
}

void CApp::Init()
{
#ifndef IVSAPP
	iv_unlink(APP_FOLDER "/Error.txt");
	ErrorFile.open(APP_FOLDER "/Error.txt");
	std::cerr.rdbuf(ErrorFile.rdbuf()); //redirect std::cout to out.txt!
#endif

	AppSettings.LoadConfig();

	iv_buildpath(CACHE_FOLDER "/");
	CDownloadManager::Get()->Init();

	// Check if we need to reimport newly opml file
	FeedList.LoadDocument(AppSettings.PathToSavedOPML);

	CFeedList OriginalOPMLFile;
	OriginalOPMLFile.LoadDocument(AppSettings.PathToOPML);

	CFeedList LastImportedOPMLFile;
	LastImportedOPMLFile.LoadDocument(AppSettings.PathToLastImportedOPML);



	Viewport.RemoveAllOverlayWidgets();
	Viewport.AddOverlayWidget(&MainPage);

	if (AppSettings.bSynchronizeAtStart)
	{
		SyncAll();
	}
	else
	{
		MainPage.Refresh();
		SHistoryItem Item;
		History.push(Item);
	}

	//CDownload Download("http://request.urih.com/", CACHE_FOLDER "/requestheaders.html");
	//CDownloadManager::Get()->AddDownload(Download);
}

void CApp::SyncAll()
{
	if (bSyncAllInProgress)
		return;

	bSyncAllInProgress = true;
	std::vector<int> Path;
	Sync(Path);
}

void CApp::SyncCurrent()
{
	if (bSyncAllInProgress)
		return;

	std::vector<int> FeedPath;
	bSyncAllInProgress = true;
	if (!History.empty())
	{
		bSyncAllInProgress = false;
		SHistoryItem Item = History.top();
		FeedPath = Item.FeedPath;
	}

	Sync(FeedPath);
}

void CApp::Sync(std::vector<int> FeedPath)
{
	CNewsFeed* CurrentFeed = GetFeed(FeedPath);
	CDownloadGroup DownloadGroup;
	NbTotalDownload -= NbDownloadFinished;
	NbDownloadFinished = 0;

	DownloadGroup.Downloads = CurrentFeed->Sync();
	for (int Index = 0; Index < DownloadGroup.Downloads.size(); ++Index)
	{
		CDownload& Download = DownloadGroup.Downloads[Index];
		Download.OnStarted = std::tr1::bind(&CApp::OnDownloadStarted, this);
		Download.OnFinished = std::tr1::bind(&CApp::OnDownloadFinished, this);
		++NbTotalDownload;
	}
	DownloadGroup.OnGroupFinished = std::tr1::bind(&CApp::OnSyncFinished, this, FeedPath);
	CDownloadManager::Get()->AddDownloadGroup(DownloadGroup, true);

	MainPage.RefreshDownloadCounter();
	Draw(false);
	Viewport.PartialUpdateForWidget(MainPage.DownloadCounterText);
}

void CApp::DLReaderMode()
{
	if (bSyncAllInProgress)
		return;

	if (History.empty())
		return;

	SHistoryItem Item = History.top();
	if (Item.FeedPath.empty())
		return;

	CNewsFeed* CurrentFeed = GetFeed(Item.FeedPath);
	if (!CurrentFeed)
		return;

	std::vector<CNewsFeed*> Children = CurrentFeed->GetChildrenRecursive();
	if (Children.empty())
		return;

	CDownloadGroup DownloadGroup;
	NbTotalDownload -= NbDownloadFinished;
	NbDownloadFinished = 0;
	for (int Index = 0; Index < Children.size(); ++Index)
	{
		for (int EntryIndex = 0; EntryIndex < Children[Index]->Entries.size(); ++EntryIndex)
		{
			const CNewsEntry& NewsEntry(Children[Index]->Entries[EntryIndex]);

			std::string Link = NewsEntry.Link;
			if (Link.empty())
				continue;

			CDownload Download = GetReaderModeDownloadFor(Link, Children[Index]);
			Download.OnStarted = std::tr1::bind(&CApp::OnDownloadStarted, this);
			Download.OnFinished = std::tr1::bind(&CApp::OnDownloadFinished, this);
			DownloadGroup.Downloads.push_back(Download);
			NbTotalDownload++;
		}
	}
	DownloadGroup.OnGroupFinished = std::tr1::bind(&CApp::OnDLReaderModeFinished, this);
	CDownloadManager::Get()->AddDownloadGroup(DownloadGroup, true);
}

CDownload CApp::GetReaderModeDownloadFor(std::string InUrl, CNewsFeed* OwnerFeed)
{
	std::string HostName;
	std::string FeedName;
	if (OwnerFeed)
	{
		HostName = GetHostName(OwnerFeed->Url) + "/";
		FeedName = OwnerFeed->Title;
	}
	else
	{
		HostName = GetHostName(InUrl) + "/";
	}
	CApp::ConvertToValidFilename(FeedName);

	size_t StartOffset = 0;
	const size_t ProtocolEndPos = InUrl.find("://");
	if (ProtocolEndPos != std::string::npos)
	{
		StartOffset = ProtocolEndPos + 3;
	}
	std::string FilePath = InUrl.substr(StartOffset);
	CApp::ConvertToValidFilename(FilePath);
	FilePath = CACHE_FOLDER "/" + HostName + "/" + FeedName + "/" + FilePath + "_dl.html";

	if (InUrl.find("www.reddit.com") != std::string::npos)
	{
		size_t Pos = InUrl.find("/comments/") + strlen("/comments/");
		if (Pos != std::string::npos)
		{
			size_t CutPos = InUrl.find("/", Pos);
			if (CutPos != std::string::npos)
			{
				InUrl = InUrl.substr(0, CutPos) + "/.rss";
			}
			//CApp::ReplaceAll(InUrl, "https://", "http://");
		}
		//std::cerr << "Reddit post treated to " << InUrl << std::endl;
	}

	return CDownload(InUrl, FilePath);
}

bool CApp::IsFileValid(const std::string& FilePath)
{
	if (iv_access(FilePath.c_str(), F_OK) != -1)
	{
		struct stat Stat;
		iv_stat(FilePath.c_str(), &Stat);
		return Stat.st_size > 0;
	}
	return false;
}

void CApp::OnDLReaderModeFinished()
{
	
}

void CApp::OnDownloadStarted()
{
	
}

void CApp::OnDownloadFinished()
{
	NbDownloadFinished++;
	MainPage.RefreshDownloadCounter();

	if ((NbDownloadFinished % 10) == 0)
	{
		SetAutoPowerOff(0);
		SetAutoPowerOff(1);
		iv_sleepmode(0);
		iv_sleepmode(1);
	}

	if ((NbDownloadFinished % 50) == 0 || (NbDownloadFinished == NbTotalDownload))
	{
		Draw(false);
		Viewport.PartialUpdateForWidget(MainPage.DownloadCounterText);
	}
}

void CApp::OnSyncFinished(std::vector<int> FeedPath)
{
	CNewsFeed* CurrentFeed = GetFeed(FeedPath);
	if (!CurrentFeed)
		return;

	CurrentFeed->LoadDocument(true);

	if (!History.empty())
	{
		SHistoryItem Item = History.top();
		if (Item.FeedPath == FeedPath)
		{
			//refresh page
			History.pop();
			OpenFeed(Item.FeedPath, Item.PageIndex, Item.bIsLastEntriesDisplay);
		}
	}

	CDownloadGroup DownloadGroup;
	NbTotalDownload -= NbDownloadFinished;
	NbDownloadFinished = 0;

	DownloadGroup.Downloads = CurrentFeed->AdditionalSync();
	for (int Index = 0; Index < DownloadGroup.Downloads.size(); ++Index)
	{
		CDownload& Download = DownloadGroup.Downloads[Index];
		Download.OnStarted = std::tr1::bind(&CApp::OnDownloadStarted, this);
		Download.OnFinished = std::tr1::bind(&CApp::OnDownloadFinished, this);
		++NbTotalDownload;
	}
	DownloadGroup.OnGroupFinished = std::tr1::bind(&CApp::OnAdditionalSyncFinished, this, FeedPath);
	CDownloadManager::Get()->AddDownloadGroup(DownloadGroup, false);

	MainPage.RefreshDownloadCounter();
	Draw(false);
	Viewport.PartialUpdateForWidget(MainPage.DownloadCounterText);
}

void CApp::OnAdditionalSyncFinished(std::vector<int> FeedPath)
{
	bSyncAllInProgress = false;
}

void CApp::Draw(bool bUpdate)
{
	Viewport.Draw(bUpdate);
}

void CApp::RedrawWidget(CUiWidget* Widget)
{
	Draw(false);
	Viewport.PartialUpdateForWidget(Widget);
}

void CApp::OpenMainPage()
{
	std::cerr << "OpenMainPage" << std::endl;
	ClearScreen();
	Viewport.RemoveAllOverlayWidgets();
	Viewport.AddOverlayWidget(&MainPage);
	MainPage.Refresh();
	Draw();

	SHistoryItem Item;
	History.push(Item);
}

void CApp::OpenFeed(std::vector<int> FeedPath, int PageIndex, bool bShowLastEntries)
{
	if (FeedPath.empty())
	{
		std::cerr << "OpenFeed to main page" << std::endl;
	}
	else
	{
		std::cerr << "OpenFeed " << FeedPath.back() << std::endl;
	}
	
	ClearScreen();

	CNewsFeed* CurrentFeed = GetFeed(FeedPath);
	if (CurrentFeed->bIsFolder)
	{
		if (bShowLastEntries)
		{
			CUiLastEntries* LastEntries = new CUiLastEntries();
			LastEntries->Refresh(FeedPath);
			LastEntries->SetPageIndex(PageIndex);
			MainPage.SetMainElement(LastEntries);
		}
		else
		{
			CUiFeedList* FeedList = new CUiFeedList();
			MainPage.SetMainElement(FeedList);
			FeedList->SetPageIndex(PageIndex);
			FeedList->Refresh(FeedPath);
		}
	}
	else
	{
		CUiFeedPage* FeedPage = new CUiFeedPage(); 
		MainPage.SetMainElement(FeedPage);
		FeedPage->SetPageIndex(PageIndex);
		FeedPage->SetFeed(FeedPath);
	}

	std::cerr << "OpenFeed draw " << std::endl;
	MainPage.SetPath(FeedPath);
	Draw();

	SHistoryItem Item(FeedPath);
	if (bShowLastEntries)
	{
		Item.LastEntriesPageIndex = PageIndex;
	}
	else
	{
		Item.PageIndex = PageIndex;
	}
	Item.bIsLastEntriesDisplay = bShowLastEntries;
	History.push(Item);
}

void CApp::OpenEntry(std::vector<int> FeedPath, int EntryIndex, int PageIndex)
{
	std::cerr << "OpenEntry " << FeedPath.back() << " " << EntryIndex << std::endl;
	
	ClearScreen();
	CUiEntryPage* EntryPage = new CUiEntryPage();
	MainPage.SetMainElement(EntryPage);
	EntryPage->SetPageIndex(PageIndex);
	EntryPage->SetEntry(FeedPath, EntryIndex);

	MainPage.SetPath(FeedPath);
	Draw();

	SHistoryItem Item(FeedPath, EntryIndex);
	Item.PageIndex = PageIndex;
	History.push(Item);

	LastEntryRead = Item;
}

void CApp::OpenExternalLink(std::vector<int> FeedPath, int EntryIndex)
{
	CNewsFeed* Feed = GetFeed(FeedPath);
	std::cerr << "OpenExternalLink " << Feed << "  " << EntryIndex << std::endl;
	const CNewsEntry& NewsEntry = Feed->Entries[EntryIndex];
	launchBrowser(NewsEntry.Link);	
}

void CApp::OpenReaderMode(std::vector<int> FeedPath, int EntryIndex)
{
	ClearScreen();
	CNewsFeed* Feed = GetFeed(FeedPath);
	std::cerr << "OpenReaderMode " << Feed << "  " << EntryIndex << std::endl;
	const CNewsEntry& NewsEntry = Feed->Entries[EntryIndex];

	CUiReaderModeBrowser* ReaderMode = new CUiReaderModeBrowser();
	MainPage.SetMainElement(ReaderMode);
	MainPage.SetPath(FeedPath);
	ReaderMode->ShowUrl(NewsEntry.Link, 0, false, Feed);

	//Draw();
	History.push(SHistoryItem());
}

void CApp::OpenSettingsPage()
{
	std::cerr << "Open Settings Page" << std::endl;
	Viewport.RemoveAllOverlayWidgets();
	Viewport.AddOverlayWidget(&SettingsPage);
	SettingsPage.Refresh();

	ClearScreen();
	Draw();
}

void CApp::ShowFolders()
{
	SHistoryItem& HistItem = History.top();
	CNewsFeed* CurrentFeed = GetFeed(HistItem.FeedPath);

	if (!CurrentFeed || !CurrentFeed->bIsFolder)
		return;

	CUiFeedList* FeedList = new CUiFeedList();
	MainPage.SetMainElement(FeedList);
	FeedList->SetPageIndex(HistItem.PageIndex);
	FeedList->Refresh(HistItem.FeedPath);

	ClearScreen();
	Draw();

	HistItem.bIsLastEntriesDisplay = false;
}

void CApp::ShowLastEntries()
{
	SHistoryItem& HistItem = History.top();
	CNewsFeed* CurrentFeed = GetFeed(HistItem.FeedPath);

	if (!CurrentFeed || !CurrentFeed->bIsFolder)
		return;

	CUiLastEntries* LastEntries = new CUiLastEntries();
	LastEntries->Refresh(HistItem.FeedPath);
	LastEntries->SetPageIndex(HistItem.LastEntriesPageIndex);
	MainPage.SetMainElement(LastEntries);

	ClearScreen();
	Draw();

	HistItem.bIsLastEntriesDisplay = true;
}

void CApp::GoBack()
{
	if (History.empty())
	{
		OpenMainPage();
		return;
	}

	CNewsFeed* CurrentFeed = GetFeed(History.top().FeedPath);
	if (CurrentFeed && !CurrentFeed->bIsFolder)
	{
		CurrentFeed->SaveDocument();
	}

	History.pop();
	if (History.empty())
	{	
		OpenMainPage();
		return;
	}

	SHistoryItem Item = History.top();
	if (!Item.FeedPath.empty() && Item.EntryIndex >= 0)
	{
		OpenEntry(Item.FeedPath, Item.EntryIndex, Item.PageIndex);
		History.pop();
	}
	else
	{
		OpenFeed(Item.FeedPath, Item.PageIndex, Item.bIsLastEntriesDisplay);
		History.pop();
	}
}

void CApp::QuitApplication()
{
	Save();
	std::cerr << "Quit application" << std::endl;
	CloseApp();
}

void CApp::Save()
{
	FeedList.SaveDocument(AppSettings.PathToSavedOPML);
}

void CApp::ClearCache()
{
	pbLaunchWaitBinary("./rm", "--recursive --one-file-system --force ", CACHE_FOLDER);
}

CNewsFeed* CApp::GetFeed(const std::vector<int>& FeedPath)
{
	return GetFeed(FeedPath, FeedPath.size());
}

CNewsFeed* CApp::GetFeed(const std::vector<int>& FeedPath, int MaxSize)
{
	CNewsFeed* CurrentFeed = &FeedList.RootFeed;

	for (int Index = 0; Index < FeedPath.size() && Index < MaxSize; ++Index)
	{
		int FeedIndex = FeedPath[Index];
		if (FeedIndex >= 0 && FeedIndex < CurrentFeed->NewsFeeds.size())
		{
			CurrentFeed = &CurrentFeed->NewsFeeds[FeedIndex];
		}
	}

	return CurrentFeed;
}

CNewsFeed* CApp::GetParentFeed(const std::vector<int>& FeedPath)
{
	if (FeedPath.empty())
	{
		std::cerr << "Getting parent feed of root feed !! ERROR" << std::endl;
		return NULL;
	}
	return GetFeed(FeedPath, FeedPath.size() - 1);
}

void CApp::ReplaceAll(std::string& String, const std::string& ToReplace, const std::string& ReplaceString)
{
	if (ToReplace.empty())
		return;

	size_t iF = std::string::npos;
	size_t iS = 0;
	while ((iF = String.find(ToReplace, iS)) != std::string::npos)
	{
		String.replace(iF, ToReplace.size(), ReplaceString);
		iS = iF + ReplaceString.size();
	}
}

void CApp::ConvertToValidFilename(std::string& FileName)
{
	ReplaceAll(FileName, "/", "");
	ReplaceAll(FileName, "\\", "");
	ReplaceAll(FileName, ":", "");
	ReplaceAll(FileName, "*", "");
	ReplaceAll(FileName, "?", "");
	ReplaceAll(FileName, "\"", "");
	ReplaceAll(FileName, "<", "");
	ReplaceAll(FileName, ">", "");
	ReplaceAll(FileName, "|", "");
	ReplaceAll(FileName, "=", "");

	if (FileName.size() > 150)
	{
		FileName = FileName.substr(std::max<size_t>(0, FileName.size() - 150));
	}
}

std::string CApp::GetHostName(const std::string& Url)
{
	size_t StartOffset = 0;
	const size_t ProtocolEndPos = Url.find("://");
	if (ProtocolEndPos != std::string::npos)
	{
		StartOffset = ProtocolEndPos + 3;
	}
	size_t FirstPos = Url.find("/", StartOffset);
	if (FirstPos != std::string::npos)
	{
		return Url.substr(StartOffset, FirstPos - StartOffset);
	}
	return Url;
}

void CApp::OnTouchDown(const SVector2i& Coord)
{
	WidgetPushDown = Viewport.GetWidgetUnder(Coord);
	WidgetPushDownTime = std::time(NULL);

	SetFocusOn(WidgetPushDown);
	if (WidgetPushDown)
	{
		WidgetPushDown->SetIsPushed(true);
		Draw(false);
		Viewport.PartialUpdateForWidget(WidgetPushDown);
	}
}

void CApp::OnTouchLong(const SVector2i& Coord)
{
	CUiWidget* WidgetTouchLong = Viewport.GetWidgetUnder(Coord);
	if (WidgetPushDown && WidgetTouchLong == WidgetPushDown)
	{
		std::vector<SUiContextMenuOption> ContextMenuOptions = WidgetPushDown->GetContextMenuOptions();
		if (ContextMenuOptions.size() > 0)
		{
			CUiContextMenu* ContextMenu = new CUiContextMenu();
			ContextMenu->ContextMenuOptions = ContextMenuOptions;
			ContextMenu->bDirty = true;
			ContextMenu->Padding.TopLeft = Coord;
			Viewport.AddOverlayWidget(ContextMenu);
			SetFocusOn(ContextMenu);
			Draw(true);
			WidgetPushDown->SetIsPushed(false);
		}
		else
		{
			WidgetPushDown->OnClick();
		}
	}
	WidgetPushDown = NULL;
}

void CApp::OnTouchUp(const SVector2i& Coord)
{
	CUiWidget* WidgetTouchUp = Viewport.GetWidgetUnder(Coord);

	if (WidgetPushDown)
	{
		WidgetPushDown->SetIsPushed(false);

		if (WidgetTouchUp == WidgetPushDown)
		{
			WidgetPushDown->OnClick();
		}
	}

	WidgetPushDown = NULL;
}

void CApp::SetFocusOn(CUiWidget* InFocusedWidget)
{
	auto OldWidgetFocusPath = WidgetFocusPath;
	WidgetFocusPath = {};
	if (InFocusedWidget)
	{
		WidgetFocusPath = InFocusedWidget->GetWidgetPath();
	}

	{
		bool bSamePath = true;
		for (int Index = 0; ; ++Index)
		{
			TSharedPtr<CUiWidget> NewWidget = Index < WidgetFocusPath.size() ? WidgetFocusPath[Index] : nullptr;
			TSharedPtr<CUiWidget> OldWidget = Index < OldWidgetFocusPath.size() ? OldWidgetFocusPath[Index] : nullptr;
			if (!NewWidget && OldWidget)
			{
				OldWidget->OnLostFocusPath();
			}
			else if (NewWidget && !OldWidget)
			{
				NewWidget->OnGainFocusPath();
			}
			else if (NewWidget && OldWidget)
			{
				if (!bSamePath || NewWidget != OldWidget)
				{
					//std::cout << "NewWidget " << NewWidget.Get() << " OldWidget " << OldWidget.Get() << std::endl;
					OldWidget->OnLostFocusPath();
					NewWidget->OnGainFocusPath();
					if (bSamePath)
					{
						for (int Index2 = Index - 1; Index2 >= 0; --Index2)
						{
							TSharedPtr<CUiWidget> ParentWidget = OldWidgetFocusPath[Index2];
							ParentWidget->OnFocusPathChangedBelow();
						}
					}
					bSamePath = false;
				}
			}
			else if (!NewWidget && !OldWidget)
			{
				break;
			}

			//if (NewWidget)
			//{
			//	std::cout << *NewWidget << " / ";
			//}
		}
		//std::cout << std::endl;
	}
}

void CApp::NextPage()
{
	CUiWidget* Widget = MainPage.GetMainElement();
	if (Widget && Widget->As<CUiVerticalBox>() && Widget->As<CUiVerticalBox>()->CanNextPage())
	{
		Widget->As<CUiVerticalBox>()->NextPage();
		if (!History.empty())
		{
			History.top().PageIndex = Widget->As<CUiVerticalBox>()->GetPageIndex();
		}
		Draw();
		return;
	}
	else if (!History.empty())
	{
		SHistoryItem Item = History.top();
		if (Item.bIsLastEntriesDisplay)
		{

		}
		else
		{
			CNewsFeed* Feed = GetFeed(Item.FeedPath);
			if (Item.EntryIndex >= 0 && Item.EntryIndex + 1 < Feed->Entries.size())
			{
				History.pop();
				OpenEntry(Item.FeedPath, Item.EntryIndex + 1, 0);
			}
		}
	}
}

void CApp::PreviousPage()
{
	CUiWidget* Widget = MainPage.GetMainElement();
	if (Widget && Widget->As<CUiVerticalBox>() && Widget->As<CUiVerticalBox>()->CanPreviousPage())
	{
		Widget->As<CUiVerticalBox>()->PreviousPage();
		if (!History.empty())
		{
			History.top().PageIndex = Widget->As<CUiVerticalBox>()->GetPageIndex();
		}
		Draw();
		return;
	}
	else if (!History.empty())
	{
		SHistoryItem Item = History.top();
		if (Item.bIsLastEntriesDisplay)
		{

		}
		else
		{
			if (Item.EntryIndex > 0)
			{
				History.pop();
				OpenEntry(Item.FeedPath, Item.EntryIndex - 1, 0);
			}
		}
	}
}
