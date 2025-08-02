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

#include "CUiNewsEntryButton.h"

#include "ui/CUiVerticalBox.h"
#include "ui/CUiText.h"
#include "ui/CUiSpacer.h"

CUiFeedPage::CUiFeedPage() : CUiVerticalBox()
{

}

void CUiFeedPage::SetFeed(const std::vector<int>& FeedPath)
{
	CApp* App = CApp::Get();

	CurrentFeedPath = FeedPath;
	ClearChildren();

	CNewsFeed* CurrentFeed = App->GetFeed(FeedPath);
	if (!CurrentFeed->IsLoaded())
	{
		CurrentFeed->LoadDocument();
	}
	
	for (int Index = 0; Index < CurrentFeed->Entries.size(); ++Index)
	{
		CUiNewsEntryButton* EntryButton = CUiNewsEntryButtonAllocator::New();
		EntryButton->SetEntry(FeedPath, Index);
		EntryButton->OnPushFunction = std::tr1::bind(&CUiFeedPage::OnOpenEntry, this, Index);
		EntryButton->Padding.BottomRight.Y = 15;
		AddChild(EntryButton);
	}
}

void CUiFeedPage::OnOpenEntry(int EntryIndex)
{
	CApp* App = CApp::Get();

	CNewsFeed* CurrentFeed = App->GetFeed(CurrentFeedPath);
	if (EntryIndex < 0 || EntryIndex >= CurrentFeed->Entries.size())
	{
		SetFeed(CurrentFeedPath);
		return;
	}

	App->OpenEntry(CurrentFeedPath, EntryIndex, 0);
}