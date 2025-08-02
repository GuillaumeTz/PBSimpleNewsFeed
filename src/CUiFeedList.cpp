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

#include "CUiFeedList.h"
#include "CUiFeedPage.h"
#include "CApp.h"

#include "CUiNewsFeedButton.h"

#include "ui/CUiVerticalBox.h"
#include "ui/CUiHorizontalBox.h"
#include "ui/CUiText.h"
#include "ui/CUiSpacer.h"

#include <iostream>
#include <sstream>
#include <set>

struct SFeedInfo
{
	std::vector<int> FeedPath;
	std::time_t Time;
	int Index;

	bool operator<(const SFeedInfo& rhs) const
	{
		return Time < rhs.Time || (Time == rhs.Time && Index < rhs.Index);
	}
};

void CUiFeedList::Refresh(const std::vector<int>& InPath, int IndexPage)
{
	CApp* App = CApp::Get();

	CurrentPath = InPath;
	CurrentPageIndex = IndexPage;
	
	ClearChildren();

	CNewsFeed* CurrentFeed = App->GetFeed(CurrentPath);

	std::set<SFeedInfo> SortedFeeds;
	if (CurrentFeed->bDisplayLastEntryFirst)
	{
		for (int Index = 0; Index < CurrentFeed->NewsFeeds.size(); ++Index)
		{
			if (CurrentFeed->NewsFeeds[Index].bDeleted)
				continue;

			SFeedInfo FeedInfo;
			FeedInfo.FeedPath = InPath;
			FeedInfo.FeedPath.push_back(Index);
			FeedInfo.Index = Index;
			FeedInfo.Time = CurrentFeed->NewsFeeds[Index].LastEntryTime;
			SortedFeeds.insert(FeedInfo);
		}

		for (std::set<SFeedInfo>::const_reverse_iterator It = SortedFeeds.rbegin(); It != SortedFeeds.rend(); ++It)
		{
			CUiNewsFeedButton* NewsFeedButton = CUiNewsFeedButtonAllocator::New();
			NewsFeedButton->SetFeed(It->FeedPath);
			NewsFeedButton->OnPushFunction = std::tr1::bind(&CUiFeedList::OnOpenFeed, this, std::tr1::placeholders::_1, It->Index);
			NewsFeedButton->Padding.BottomRight.Y = 15;
			AddChild(NewsFeedButton);
		}
	}
	else
	{
		for (int Index = 0; Index < CurrentFeed->NewsFeeds.size(); ++Index)
		{
			if (CurrentFeed->NewsFeeds[Index].bDeleted)
				continue;

			CUiNewsFeedButton* NewsFeedButton = CUiNewsFeedButtonAllocator::New();
			std::vector<int> Path = InPath;
			Path.push_back(Index);
			NewsFeedButton->SetFeed(Path);
			NewsFeedButton->OnPushFunction = std::tr1::bind(&CUiFeedList::OnOpenFeed, this, std::tr1::placeholders::_1, Index);
			NewsFeedButton->Padding.BottomRight.Y = 15;
			AddChild(NewsFeedButton);
		}
	}
}

void CUiFeedList::OnOpenFeed(CUiButton* Button, int NewsFeedIndex)
{
	CApp* App = CApp::Get();

	std::vector<int> FeedPath = CurrentPath;
	FeedPath.push_back(NewsFeedIndex);
	App->OpenFeed(FeedPath, 0);
}
