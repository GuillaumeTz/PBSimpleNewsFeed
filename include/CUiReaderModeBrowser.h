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

#ifndef CUIREADERMODEBROWSER_H
#define CUIREADERMODEBROWSER_H

#include "ui/CUiVerticalBox.h"

#include "CDownload.h"
#include "CNewsFeed.h"
#include "tinyxml2.h"

#include <set>
#include <stack>

struct SReaderModeBrowserHistoryItem
{
	std::string Url;
	int PageIndex;

	SReaderModeBrowserHistoryItem(const std::string& InUrl, int InPageIndex) : Url(InUrl), PageIndex(InPageIndex) { }
};

class CUiReaderModeBrowser : public CUiVerticalBox
{
public:
	std::string Title;
	bool bIsDownloading;

public:
	CUiReaderModeBrowser();
	void ShowUrl(const std::string& InUrl, int InPageIndex, bool bForceDownload = false, CNewsFeed* Feed = nullptr);

	void Refresh();
	void Back();

	void OnDownloadFinished(bool bIsFromDownload);

	virtual void Draw(SUiDrawVisitor& DrawVisitor) override;
	virtual bool CanNextPage() const override { return SubVerticalBox->CanNextPage(); }
	virtual void NextPage() override { return SubVerticalBox->NextPage(); }
	virtual bool CanPreviousPage() const override { return SubVerticalBox->CanPreviousPage(); }
	virtual void PreviousPage() override { return SubVerticalBox->PreviousPage(); }
	virtual int GetPageIndex() const override { return SubVerticalBox->GetPageIndex(); }
	virtual void SetPageIndex(int InIndexPage) override { SubVerticalBox->SetPageIndex(InIndexPage); }

private:
	TSharedPtr<CUiVerticalBox> SubVerticalBox;
	std::stack<SReaderModeBrowserHistoryItem> HistoryUrls;
	std::string Url;
	std::string LocalFilePath;
	CNewsFeed* Feed;
	TSharedPtr<class CUiButton> RefreshButton;
	TSharedPtr<class CUiButton> BackButton;
};

#endif
