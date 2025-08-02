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

#include "CUiLastEntries.h"

#include "CApp.h"

#include "CUiNewsEntryButton.h"

#include "ui/CUiVerticalBox.h"
#include "ui/CUiHorizontalBox.h"
#include "ui/CUiText.h"
#include "ui/CUiSpacer.h"

#include <iostream>
#include <set>

struct SEntryInfo
{
	const CNewsEntry* NewsEntry;
	std::vector<int> FeedPath;
	std::string Tag;
	int EntryIndex;
	std::time_t Time;

	bool operator<(const SEntryInfo& rhs) const
	{
		return Time < rhs.Time || (Time == rhs.Time && NewsEntry < rhs.NewsEntry);
	}
};

void ParseNewsFeed(const CNewsFeed* InNewsFeed, std::vector<int>& InFeedPath, std::set<SEntryInfo>& Entries)
{
	for (int Index = 0; Index < InNewsFeed->NewsFeeds.size(); ++Index)
	{
		const CNewsFeed& NewsFeed(InNewsFeed->NewsFeeds[Index]);

		InFeedPath.push_back(Index);
		if (NewsFeed.bIsFolder)
		{
			ParseNewsFeed(&NewsFeed, InFeedPath, Entries);
		}
		else
		{
			std::string Tag = NewsFeed.Title.substr(0, 3);
			for (int EntryIndex = 0; EntryIndex < NewsFeed.Entries.size(); ++EntryIndex)
			{
				SEntryInfo EntryInfo;
				EntryInfo.FeedPath = InFeedPath;
				EntryInfo.EntryIndex = EntryIndex;
				EntryInfo.NewsEntry = &NewsFeed.Entries[EntryIndex];
				EntryInfo.Tag = Tag;
				std::tm TimeTM = EntryInfo.NewsEntry->GetTime();
				EntryInfo.Time = std::mktime(&TimeTM);
				Entries.insert(EntryInfo);
			}
		}
		InFeedPath.pop_back();
	}
}

void CUiLastEntries::Refresh(const std::vector<int>& InFeedPath)
{
	CApp* App = CApp::Get();
	
	ClearChildren();	

	CNewsFeed* RootFeed = App->GetFeed(InFeedPath);
	if (!RootFeed->IsLoaded())
	{
		RootFeed->LoadDocument();
	}
	std::vector<int> FeedPath = InFeedPath;
	std::set<SEntryInfo> Entries;
	ParseNewsFeed(RootFeed, FeedPath, Entries);
	
	int LastEntryTimeDay = 0;
	int RemainingEntryNumber = 500;
	for (std::set<SEntryInfo>::const_reverse_iterator It = Entries.rbegin(); It != Entries.rend(); ++It)
	{
		--RemainingEntryNumber;
		if (RemainingEntryNumber < 0)
			break;

		const SEntryInfo& EntryInfo(*It);

		//show last entry read as read font
		if (EntryInfo.NewsEntry->bHasRead && (EntryInfo.FeedPath != App->LastEntryRead.FeedPath || EntryInfo.EntryIndex != App->LastEntryRead.EntryIndex))
			continue;

		{
			std::tm* EntryLocalTime = std::localtime(&EntryInfo.Time);
			if (EntryLocalTime->tm_yday != LastEntryTimeDay)
			{
				//show new date
				CUiText* UiText = CUiTextAllocator::New();
				UiText->Font = App->AppSettings.FeedReadFont;
				UiText->Text = "-----  " + EntryInfo.NewsEntry->Time + "  -----";
				UiText->Padding.TopLeft.Y = 5;
				UiText->Padding.BottomRight.Y = 5;
				UiText->PivotPointRatio.X = 0.5f;
				AddChild(UiText);
			}
		}

		CUiNewsEntryButton* EntryButton = CUiNewsEntryButtonAllocator::New();
		EntryButton->SetEntry(EntryInfo.FeedPath, EntryInfo.EntryIndex, EntryInfo.Tag);
		EntryButton->OnPushFunction = std::tr1::bind(&CUiLastEntries::OnOpenEntry, this, std::tr1::placeholders::_1, EntryInfo.FeedPath, EntryInfo.EntryIndex);
		EntryButton->Padding.BottomRight.Y = 15;
		AddChild(EntryButton);

		LastEntryTimeDay = std::localtime(&EntryInfo.Time)->tm_yday;
	}

	//for (std::set<SEntryInfo>::const_reverse_iterator It = Entries.rbegin(); It != Entries.rend(); ++It)
	//{
	//	const SEntryInfo& EntryInfo(*It);

	//	if (!EntryInfo.NewsEntry.bHasRead)
	//		continue;

	//	CUiText* UiText = CUiTextAllocator::New();
	//	UiText->Font = App->AppSettings.FeedReadFont;
	//	UiText->Text = EntryInfo.NewsEntry.Title;

	//	CUiButton* Button = new CUiButton();
	//	Button->Child = UiText;
	//	Button->OnPushFunction = std::tr1::bind(&CUiLastEntries::OnOpenEntry, this, std::tr1::placeholders::_1, EntryInfo.FeedPath, EntryInfo.EntryIndex);
	//	AddChild(Button);

	//	CUiSpacer* Spacer = CUiSpacerAllocator::New();
	//	Spacer->DesiredSize.Y = 15;
	//	AddChild(Spacer);
	//}
}

void CUiLastEntries::OnOpenEntry(class CUiButton* Button, std::vector<int> FeedPath, int EntryIndex)
{
	CApp* App = CApp::Get();
	App->OpenEntry(FeedPath, EntryIndex, 0);
}
