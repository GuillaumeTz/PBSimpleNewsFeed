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

#include "CUiFeedPage.h"
#include "CApp.h"

#include "ui/CUiVerticalBox.h"
#include "ui/CUiButton.h"
#include "ui/CUiText.h"
#include "ui/CUiSpacer.h"
#include "CUiReaderModeBrowser.h"

CUiEntryPage::CUiEntryPage() : CUiVerticalBox()
{
	CurrentEntryIndex = -1;
}

void CUiEntryPage::SetEntry(const std::vector<int>& FeedPath, int EntryIndex)
{
	CApp* App = CApp::Get();

	CurrentFeedPath = FeedPath;
	CurrentEntryIndex = EntryIndex;

	CNewsFeed* CurrentFeed = App->GetFeed(CurrentFeedPath);
	if (CurrentEntryIndex < 0 || CurrentEntryIndex >= CurrentFeed->Entries.size())
	{
		std::cerr << "Error trying to open wrong entry index " << CurrentEntryIndex << std::endl;
		return;
	}

	ClearChildren();

	CNewsEntry& NewsEntry(CurrentFeed->Entries[EntryIndex]);
	NewsEntry.bHasRead = true;
	NewsEntry.bIsNew = false;

	CDownload ReaderModeDownload = CApp::GetReaderModeDownloadFor(NewsEntry.Link, CurrentFeed);
	bool bReaderModeFileDLed = CApp::IsFileValid(ReaderModeDownload.GetFilePath().c_str());
	bool bIsReddit = NewsEntry.Link.find("www.reddit.com") != std::string::npos;

	{
		CUiHorizontalBox* TitleHorizontalBox = new CUiHorizontalBox();
		TitleHorizontalBox->Padding.BottomRight.Y = 8;
		AddChild(TitleHorizontalBox);

		CUiText* UiText = CUiTextAllocator::New();
		UiText->Font = App->AppSettings.EntryTitleFontBold;
		UiText->Text = NewsEntry.Title;

		CUiButton* ButtonOpenExternalLink = new CUiButton();
		ButtonOpenExternalLink->Child = UiText;
		ButtonOpenExternalLink->OnPushFunction = std::tr1::bind(&CUiEntryPage::OnOpenExternalLink, this);
		TitleHorizontalBox->AddChild(ButtonOpenExternalLink);

		{
			PageNumText = CUiTextAllocator::New();
			PageNumText->Font = App->AppSettings.EntryTextFont;
			PageNumText->Padding.TopLeft.X = 50;
			TitleHorizontalBox->AddChild(*PageNumText);
		}

		{
			CUiText* IndexEntryInAllText = CUiTextAllocator::New();
			IndexEntryInAllText->Padding.TopLeft.X = 10;
			IndexEntryInAllText->Font = App->AppSettings.EntryTextFont;
			IndexEntryInAllText->Text = "(" + CUtils::ToString(EntryIndex + 1) + "/" + CUtils::ToString(CurrentFeed->Entries.size()) + ")";
			TitleHorizontalBox->AddChild(IndexEntryInAllText);
		}
	}

	{
		CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
		HorizontalBox->bFill = true;
		HorizontalBox->Padding.BottomRight.Y = 25;
		
		{
			CUiText* UiText = CUiTextAllocator::New();
			UiText->Font = App->AppSettings.EntryTextFont;
			UiText->Text = NewsEntry.Time;
			HorizontalBox->AddChild(UiText);
		}

		if (!bIsReddit)
		{
			CUiText* UiText = CUiTextAllocator::New();
			UiText->Font =  bReaderModeFileDLed ? App->AppSettings.EntryTextFont : App->AppSettings.EntryTextFontBold;
			UiText->Text = "Open ReaderMode";

			CUiButton* ButtonOpenExternalLink = new CUiButton();
			ButtonOpenExternalLink->Child = UiText;
			ButtonOpenExternalLink->OnPushFunction = std::tr1::bind(&CUiEntryPage::OnOpenInReaderMode, this);
			ButtonOpenExternalLink->PivotPointRatio.X = 1.f;
			HorizontalBox->AddChild(ButtonOpenExternalLink);
		}
		else
		{
			CUiText* UiText = CUiTextAllocator::New();
			UiText->Font =  bReaderModeFileDLed ? App->AppSettings.EntryTextFont : App->AppSettings.EntryTextFontBold;
			UiText->Text = "Refresh";

			CUiButton* RefreshButton = new CUiButton();
			RefreshButton->Child = UiText;
			RefreshButton->OnPushFunction = std::tr1::bind(&CUiEntryPage::RefreshReaderMode, this);
			RefreshButton->PivotPointRatio.X = 1.f;
			HorizontalBox->AddChild(RefreshButton);
		}

		AddChild(HorizontalBox);
	}

	if (bIsReddit)
	{
		if (bReaderModeFileDLed)
		{
			//if this is a link to an external link then display it
			if (!NewsEntry.ExternalLink.empty())
			{
				CDownload ExternalLinkDownload = CApp::GetReaderModeDownloadFor(NewsEntry.ExternalLink, CurrentFeed);
				if (CApp::IsFileValid(ExternalLinkDownload.GetFilePath().c_str()))
				{
					CUiReaderModeBrowser* ReaderModeBrowser = new CUiReaderModeBrowser();
					ReaderModeBrowser->MaxSize = SVector2i();
					ReaderModeBrowser->ShowUrl(NewsEntry.ExternalLink, 0, false, CurrentFeed);
					AddChild(ReaderModeBrowser);
				}
			}

			CNewsFeed CommentFeed;
			CommentFeed.LoadDocument(ReaderModeDownload.GetFilePath());
			for (int Index = 0; Index < CommentFeed.Entries.size(); ++Index)
			{
				const CNewsEntry& CommentEntry(CommentFeed.Entries[Index]);

				if (Index > 0)
				{
					CUiText* UiText = CUiTextAllocator::New();
					UiText->Font = App->AppSettings.EntryTextFontBold;
					std::string UserName = CommentEntry.Title;
					CApp::ReplaceAll(UserName, NewsEntry.Title, "");
					CApp::ReplaceAll(UserName, " on ", "");
					CApp::ReplaceAll(UserName, "/u/", "");
					UiText->Text = UserName + "    (" + CommentEntry.Time + ")";
					AddChild(UiText);
				}

				CUiText* UiHtml = CUiTextAllocator::New();
				UiHtml->Font = App->AppSettings.EntryTextFont;
				UiHtml->Padding.TopLeft.X = 10;
				UiHtml->Padding.BottomRight.X = 10;
				UiHtml->Padding.BottomRight.Y = 15;
				UiHtml->SetHtml(CommentEntry.Text);
				AddChild(UiHtml);
			}
		}
	}
	else
	{
		CUiText* UiHtml = CUiTextAllocator::New();
		UiHtml->Font = App->AppSettings.EntryTextFont;
		UiHtml->Padding.TopLeft.X = 10;
		UiHtml->Padding.BottomRight.X = 10;
		UiHtml->Padding.BottomRight.Y = 15;
		UiHtml->SetHtml(NewsEntry.Text);
		AddChild(UiHtml);

		if (bReaderModeFileDLed)
		{
			CUiReaderModeBrowser* ReaderModeBrowser = new CUiReaderModeBrowser();
			ReaderModeBrowser->MaxSize = SVector2i();
			ReaderModeBrowser->ShowUrl(NewsEntry.Link, 0, false, CurrentFeed);
			AddChild(ReaderModeBrowser);
		}
	}

	PageNumText->Text = CUtils::ToString(GetPageIndex() + 1) + "/" + CUtils::ToString(GetMaxPageIndex());
}

void CUiEntryPage::CalcDesiredSize(SVector2i AllowedSize)
{
	CUiVerticalBox::CalcDesiredSize(AllowedSize);

	CApp* App = CApp::Get();
	PageNumText->Text = CUtils::ToString(GetPageIndex() + 1) + "/" + CUtils::ToString(GetMaxPageIndex());
}

void CUiEntryPage::OnOpenExternalLink()
{
	CApp* App = CApp::Get();
	App->OpenExternalLink(CurrentFeedPath, CurrentEntryIndex);
}

void CUiEntryPage::OnOpenInReaderMode()
{
	CApp* App = CApp::Get();
	App->OpenReaderMode(CurrentFeedPath, CurrentEntryIndex);
}

void CUiEntryPage::RefreshReaderMode()
{
	CApp* App = CApp::Get();

	CNewsFeed* CurrentFeed = App->GetFeed(CurrentFeedPath);
	CNewsEntry& NewsEntry(CurrentFeed->Entries[CurrentEntryIndex]);

	CDownloadGroup Group;
	Group.Downloads.push_back(App->GetReaderModeDownloadFor(NewsEntry.Link, CurrentFeed));
	if (!NewsEntry.ExternalLink.empty())
	{
		Group.Downloads.push_back(App->GetReaderModeDownloadFor(NewsEntry.ExternalLink, CurrentFeed));
		std::cerr << "Add external link to dl " << NewsEntry.ExternalLink << std::endl;
	}
	Group.OnGroupFinished = std::tr1::bind(&CUiEntryPage::RefreshEnded, this);
	CDownloadManager::Get()->AddDownloadGroup(Group, true);
}

void CUiEntryPage::RefreshEnded()
{
	SetEntry(CurrentFeedPath, CurrentEntryIndex);
	CApp* App = CApp::Get();
	App->Draw();
}
