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

#include "CUiNewsEntryButton.h"
#include "CApp.h"

#include <sstream>

CUiNewsEntryButton::CUiNewsEntryButton() : CUiButton()
{
	CurrentEntryIndex = -1;
	bHasBeenDrawn = false;
}

void CUiNewsEntryButton::SetEntry(const std::vector<int>& FeedPath, int EntryIndex, const std::string& Tag)
{
	bHasBeenDrawn = false;
	CurrentPath = FeedPath;
	CurrentEntryIndex = EntryIndex;
	CurrentTag = Tag;
	Refresh();
}

void CUiNewsEntryButton::MarkAsRead()
{
	CApp* App = CApp::Get();
	CNewsFeed* NewsFeed = App->GetFeed(CurrentPath);
	CNewsEntry& NewsEntry = NewsFeed->Entries[CurrentEntryIndex];
	NewsEntry.MarkAsRead();
	Refresh();
	App->RedrawWidget(this);
}

void CUiNewsEntryButton::Delete()
{
	CApp* App = CApp::Get();
	CNewsFeed* NewsFeed = App->GetFeed(CurrentPath);
	NewsFeed->Entries.erase(NewsFeed->Entries.begin() + CurrentEntryIndex);
	CurrentPath.clear();
	CurrentEntryIndex = -1;
	App->Draw();
}

void CUiNewsEntryButton::Refresh()
{
	CApp* App = CApp::Get();
	CNewsFeed* NewsFeed = App->GetFeed(CurrentPath);
	if (!NewsFeed || CurrentEntryIndex < 0 || CurrentEntryIndex >= NewsFeed->Entries.size())
		return;

	const CNewsEntry& NewsEntry = NewsFeed->Entries[CurrentEntryIndex];

	CUiText* UiText = CUiTextAllocator::New();
	UiText->Font = NewsEntry.bHasRead ? App->AppSettings.FeedReadFont : App->AppSettings.FeedUnReadFont;

	std::stringstream stream;
	if (NewsEntry.bIsNew)
	{
		stream << "* ";
	}
	if (!CurrentTag.empty())
	{
		stream << "[" << CurrentTag << "] ";
	}
	stream << NewsEntry.Title;
	UiText->Text = stream.str();
	Child = UiText;
}

void CUiNewsEntryButton::Draw(SUiDrawVisitor& DrawVisitor)
{
	CUiButton::Draw(DrawVisitor);

	if (!bHasBeenDrawn)
	{
		bHasBeenDrawn = true;

		CApp* App = CApp::Get();
		CNewsFeed* NewsFeed = App->GetFeed(CurrentPath);
		CNewsEntry& NewsEntry = NewsFeed->Entries[CurrentEntryIndex];
		NewsEntry.bIsNew = false;
	}
}

std::vector<SUiContextMenuOption> CUiNewsEntryButton::GetContextMenuOptions()
{
	std::vector<SUiContextMenuOption> Options;

	{
		SUiContextMenuOption MarkAsReadOption;
		MarkAsReadOption.Text = "Mark as read";
		MarkAsReadOption.OnPushFunction = std::tr1::bind(&CUiNewsEntryButton::MarkAsRead, this);
		Options.push_back(MarkAsReadOption);
	}

	//{
	//	SUiContextMenuOption DeleteOption;
	//	DeleteOption.Text = "Delete";
	//	DeleteOption.OnPushFunction = std::tr1::bind(&CUiNewsEntryButton::Delete, this);
	//	Options.push_back(DeleteOption);
	//}

	return Options;
}

void CUiNewsEntryButton::DeleteItself()
{
	CUiNewsEntryButtonAllocator::Allocator.Delete(this);
}
