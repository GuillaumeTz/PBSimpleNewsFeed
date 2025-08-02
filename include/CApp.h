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

#ifndef CAPP_H
#define CAPP_H

#include <stack>
#include <ctime>
#include <fstream>

#include "CUtils.h"
#include "CAppSettings.h"
#include "CNewsFeed.h"
#include "CFeedList.h"
#include "CDownload.h"

#include "ui/CUiViewport.h"
#include "CUiMainPage.h"
#include "CUiFeedPage.h"
#include "CUiEntryPage.h"
#include "CUiSettingsPage.h"

struct SHistoryItem
{
public:
	std::vector<int> FeedPath;
	int EntryIndex;
	int PageIndex;
	int LastEntriesPageIndex;

	bool bIsLastEntriesDisplay;

	SHistoryItem() : EntryIndex(-1), PageIndex(0), LastEntriesPageIndex(0), bIsLastEntriesDisplay(false) { }
	SHistoryItem(const std::vector<int>& InFeedPath) : FeedPath(InFeedPath), EntryIndex(-1), PageIndex(0), LastEntriesPageIndex(0), bIsLastEntriesDisplay(false) { }
	SHistoryItem(const std::vector<int>& InFeedPath, int InEntryIndex) : FeedPath(InFeedPath), EntryIndex(InEntryIndex), PageIndex(0), LastEntriesPageIndex(0), bIsLastEntriesDisplay(false) { }
};

class CApp
{
public:
	static int App_Handler(int type, int par1, int par2);
	static void Main();
	static CApp* Get();

public:
	CAppSettings AppSettings;
	CUiViewport Viewport;

	CFeedList FeedList;

	CUiMainPage MainPage;
	CUiSettingsPage SettingsPage;

	SHistoryItem LastEntryRead;
	std::stack<SHistoryItem> History;
	int NbDownloadFinished;
	int NbTotalDownload;

	std::ofstream ErrorFile;

public:
	void Init();
	void SyncAll();
	void SyncCurrent();
	void Sync(std::vector<int> FeedPath);
	void DLReaderMode();

	void OnDLReaderModeFinished();

	void OnDownloadStarted();
	void OnDownloadFinished();
	void OnSyncFinished(std::vector<int> FeedPath);
	void OnAdditionalSyncFinished(std::vector<int> FeedPath);
	void Draw(bool bUpdate = true);
	void RedrawWidget(CUiWidget* Widget);

	void OpenMainPage();
	void OpenFeed(std::vector<int> FeedPath, int PageIndex, bool bShowLastEntries = false);
	void OpenEntry(std::vector<int> FeedPath, int EntryIndex, int PageIndex);
	void OpenExternalLink(std::vector<int> FeedPath, int EntryIndex);

	void OpenReaderMode(std::vector<int> FeedPath, int EntryIndex);
	void OpenSettingsPage();

	void ShowFolders();
	void ShowLastEntries();

	void GoBack();
	void QuitApplication();
	void Save();
	void ClearCache();

	CNewsFeed* GetFeed(const std::vector<int>& FeedPath);
	CNewsFeed* GetFeed(const std::vector<int>& FeedPath, int MaxSize);
	CNewsFeed* GetParentFeed(const std::vector<int>& FeedPath);

	static void ReplaceAll(std::string& String, const std::string& ToReplace, const std::string& ReplaceString);
	static void ConvertToValidFilename(std::string& FileName);

	static std::string GetHostName(const std::string& Url);
	static CDownload GetReaderModeDownloadFor(std::string InUrl, CNewsFeed* OwnerFeed);
	static bool IsFileValid(const std::string& FilePath);

public:
	void OnTouchDown(const SVector2i& Coord);
	void OnTouchLong(const SVector2i& Coord);
	void OnTouchUp(const SVector2i& Coord);
	void SetFocusOn(CUiWidget* InFocusedWidget);

	void NextPage();
	void PreviousPage();

private:
	CApp();

	static CApp* Singleton;
	void msg(const char *s);

	class CUiWidget* WidgetPushDown;
	std::vector<TSharedPtr<CUiWidget>> WidgetFocusPath;
	std::time_t WidgetPushDownTime;

	bool bSyncAllInProgress;
};


#endif
