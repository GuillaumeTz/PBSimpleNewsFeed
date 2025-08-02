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

#include "CUiNewsFeedButton.h"
#include "CApp.h"

#include <sstream>

void CUiNewsFeedButton::SetFeed(const std::vector<int>& FeedPath)
{
	CurrentPath = FeedPath;
	Refresh();
}

void CUiNewsFeedButton::MarkAsRead()
{
	CApp* App = CApp::Get();
	CNewsFeed* NewsFeed = App->GetFeed(CurrentPath);
	NewsFeed->MarkAsRead();
	Refresh();
	App->RedrawWidget(this);
}

void CUiNewsFeedButton::Delete()
{
	CApp* App = CApp::Get();
	CNewsFeed* NewsFeed = App->GetFeed(CurrentPath);
	NewsFeed->MarkAsDeleted();
	CurrentPath.clear();
	TSharedPtr<CUiNewsFeedButton> ThisShared = this;
	if (Parent)
	{
		CUiCompositeWidget* CompositeWidget = dynamic_cast<CUiCompositeWidget*>(Parent);
		CompositeWidget->RemoveChild(this);
	}
	App->Draw();
}

void CUiNewsFeedButton::Refresh()
{
	CApp* App = CApp::Get();
	CNewsFeed* NewsFeed = App->GetFeed(CurrentPath);
	if (!NewsFeed)
		return;

	CUiText* UiText = CUiTextAllocator::New();
	UiText->Font = (NewsFeed && NewsFeed->GetNbUnRead() > 0) ? App->AppSettings.FeedUnReadFont : App->AppSettings.FeedReadFont;

	std::stringstream stream;
	if (NewsFeed)
	{
		if (NewsFeed->GetNbNew() > 0)
		{
			stream << "* ";
		}

		if (NewsFeed->bIsFolder)
		{
			stream << "/";
		}

		stream << NewsFeed->Title << " (" << NewsFeed->GetNbUnRead() << ")";
	}

	UiText->Text = stream.str();
	Child = UiText;
}

std::vector<SUiContextMenuOption> CUiNewsFeedButton::GetContextMenuOptions()
{
	std::vector<SUiContextMenuOption> Options;

	{
		SUiContextMenuOption MarkAsReadOption;
		MarkAsReadOption.Text = "Mark as read";
		MarkAsReadOption.OnPushFunction = std::tr1::bind(&CUiNewsFeedButton::MarkAsRead, this);
		Options.push_back(MarkAsReadOption);
	}

	{
		SUiContextMenuOption DeleteOption;
		DeleteOption.Text = "Delete";
		DeleteOption.OnPushFunction = std::tr1::bind(&CUiNewsFeedButton::Delete, this);
		Options.push_back(DeleteOption);
	}

	return Options;
}

void CUiNewsFeedButton::DeleteItself()
{
	CUiNewsFeedButtonAllocator::Allocator.Delete(this);
}
