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

#include "CUiMainPage.h"
#include "CUiFeedPage.h"
#include "CUiFeedList.h"
#include "CApp.h"

#include "ui/CUiVerticalBox.h"
#include "ui/CUiHorizontalBox.h"
#include "ui/CUiButton.h"
#include "ui/CUiText.h"
#include "ui/CUiSpacer.h"
#include "ui/CUiLine.h"

#include <iostream>
#include <sstream>

CUiMainPage::CUiMainPage() : CUiOverlay()
{
	BreadcrumbHBox = NULL;
	DownloadCounterText = NULL;
	bFill = true;
}

void CUiMainPage::Refresh()
{
	std::cerr << "Main Page Refresh" << std::endl;
	CApp* App = CApp::Get();

	ClearChildren();
	AddChild(new CUiVerticalBox());
	CUiVerticalBox* VerticalBox = dynamic_cast<CUiVerticalBox*>((*GetChildren())[0].Get());
	VerticalBox->Padding.TopLeft.X = 20;
	VerticalBox->Padding.BottomRight.X = 20;

	// add header bar
	{
		CUiHorizontalBox* HeaderBarHBox = new CUiHorizontalBox();
		HeaderBarHBox->bFill = true;

		{
			CUiButton* HomeButton = new CUiButton();
			CUiText* Text = CUiTextAllocator::New();
			Text->Font = App->AppSettings.MenuButtonFont;
			Text->Text = "X";
			HomeButton->Child = Text;
			HomeButton->OnPushFunction = std::tr1::bind(&CApp::QuitApplication, App);
			HomeButton->Padding.BottomRight.X = 20;
			HeaderBarHBox->AddChild(HomeButton);
		}

		{
			CUiButton* SyncButton = new CUiButton();
			CUiText* Text = CUiTextAllocator::New();
			Text->Font = App->AppSettings.MenuButtonFont;
			Text->Text = "Sync";
			SyncButton->Child = Text;
			SyncButton->OnPushFunction = std::tr1::bind(&CApp::SyncAll, App);
			SyncButton->Padding.BottomRight.X = 20;
			HeaderBarHBox->AddChild(SyncButton);
		}

		{
			CUiButton* SyncButton = new CUiButton();
			CUiText* Text = CUiTextAllocator::New();
			Text->Font = App->AppSettings.MenuButtonFont;
			Text->Text = "Download";
			SyncButton->Child = Text;
			SyncButton->OnPushFunction = std::tr1::bind(&CApp::DLReaderMode, App);
			SyncButton->Padding.BottomRight.X = 20;
			HeaderBarHBox->AddChild(SyncButton);
		}

		{
			CUiButton* Button = new CUiButton();
			CUiText* Text = CUiTextAllocator::New();
			Text->Font = App->AppSettings.MenuButtonFont;
			Text->Text = "Settings";
			Button->Child = Text;
			Button->OnPushFunction = std::tr1::bind(&CApp::OpenSettingsPage, App);
			Button->Padding.BottomRight.X = 20;
			HeaderBarHBox->AddChild(Button);
		}

		{
			CUiButton* BackButton = new CUiButton();
			CUiText* Text = CUiTextAllocator::New();
			Text->Font = App->AppSettings.MenuButtonFont;
			Text->Text = "<=";
			BackButton->Child = Text;
			BackButton->OnPushFunction = std::tr1::bind(&CApp::GoBack, App);
			HeaderBarHBox->AddChild(BackButton);
		}

		HeaderBarHBox->Padding.BottomRight.Y = 20;
		VerticalBox->AddChild(HeaderBarHBox);
	}

	{
		CUiVerticalBox* VertBox = new CUiVerticalBox();
		VerticalBox->AddChild(VertBox);

		{
			CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
			HorizontalBox->bFill = true;
			HorizontalBox->Padding.BottomRight.Y = 5;
			VertBox->AddChild(HorizontalBox);

			BreadcrumbHBox = new CUiHorizontalBox();
			HorizontalBox->AddChild(BreadcrumbHBox);

			{
				DownloadCounterText = CUiTextAllocator::New();
				DownloadCounterText->Font = App->AppSettings.FeedPathFont;
				DownloadCounterText->PivotPointRatio.X = 1.f;
				HorizontalBox->AddChild(DownloadCounterText);
				RefreshDownloadCounter();
			}
		}

		
		{
			CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
			HorizontalBox->Padding.BottomRight.Y = 10;
			VertBox->AddChild(HorizontalBox);

			{
				CUiButton* FoldersButton = new CUiButton();
				CUiText* Text = CUiTextAllocator::New();
				Text->Font = App->AppSettings.SwitchViewButtonsFont;
				Text->Text = "Folders";
				FoldersButton->Child = Text;
				FoldersButton->OnPushFunction = std::tr1::bind(&CApp::ShowFolders, App);
				FoldersButton->Padding.BottomRight.X = 25;
				HorizontalBox->AddChild(FoldersButton);
			}

			{
				CUiButton* LastEntriesButton = new CUiButton();
				CUiText* Text = CUiTextAllocator::New();
				Text->Font = App->AppSettings.SwitchViewButtonsFont;
				Text->Text = "Last Entries";
				LastEntriesButton->Child = Text;
				LastEntriesButton->OnPushFunction = std::tr1::bind(&CApp::ShowLastEntries, App);
				HorizontalBox->AddChild(LastEntriesButton);
			}

			{
				CUiButton* SyncButton = new CUiButton();
				CUiText* Text = CUiTextAllocator::New();
				Text->Font = App->AppSettings.SwitchViewButtonsFont;
				Text->Text = "Sync";
				SyncButton->Child = Text;
				SyncButton->OnPushFunction = std::tr1::bind(&CApp::SyncCurrent, App);
				SyncButton->Padding.TopLeft.X = 40;
				HorizontalBox->AddChild(SyncButton);
			}
		}

		CUiLine* Line = CUiLineAllocator::New();
		Line->Padding.BottomRight.Y = 15;
		VertBox->AddChild(Line);
	}

	CUiFeedList* FeedList = new CUiFeedList();
	std::vector<int> Path; 
	FeedList->Refresh(Path);
	SetMainElement(FeedList);
	SetPath({});

	std::cerr << "End Main Page Refresh" << std::endl;
}

void CUiMainPage::RefreshDownloadCounter()
{
	CApp* App = CApp::Get();

	if (App->NbTotalDownload > 0)
	{
		std::stringstream stream;
		stream << "Downloading " << App->NbDownloadFinished << "/" << App->NbTotalDownload << std::endl;
		DownloadCounterText->Text = stream.str();
	}
	else
	{
		DownloadCounterText->Text = "              ";
	}
}

void CUiMainPage::SetMainElement(CUiWidget* Widget)
{
	if (GetChildren()->empty())
	{
		AddChild(new CUiVerticalBox());
	}

	CUiVerticalBox* VerticalBox = dynamic_cast<CUiVerticalBox*>((*GetChildren())[0].Get());

	if (VerticalBox->GetChildren()->size() > 2)
	{
		VerticalBox->RemoveChildAt(2);
	}

	VerticalBox->AddChild(Widget);
}

CUiWidget* CUiMainPage::GetMainElement()
{
	if (GetChildren()->empty())
	{
		return NULL;
	}

	CUiVerticalBox* VerticalBox = dynamic_cast<CUiVerticalBox*>((*GetChildren())[0].Get());
	if ((*VerticalBox->GetChildren()).size() > 2)
	{
		return (*VerticalBox->GetChildren())[2].Get();
	}
	return NULL;
}

void CUiMainPage::SetPath(const std::vector<int>& InFeedPath)
{
	if (!BreadcrumbHBox)
		return;

	BreadcrumbHBox->ClearChildren();

	CApp* App = CApp::Get();

	CUiButton* Button = new CUiButton();
	CUiText* Text = CUiTextAllocator::New();
	Text->Font = App->AppSettings.FeedPathFont;
	Text->Text = "Root";
	Button->Child = Text;
	std::vector<int> FeedPath;
	Button->OnPushFunction = std::tr1::bind(&CApp::OpenFeed, App, FeedPath, 0, false);
	BreadcrumbHBox->AddChild(Button);
	for (int Index = 0; Index < InFeedPath.size(); ++Index)
	{
		CUiButton* Button = new CUiButton();
		CUiText* Text = CUiTextAllocator::New();
		Text->Font = App->AppSettings.FeedPathFont;
		Text->Text = " / ";
		Text->Text += App->GetFeed(InFeedPath, Index + 1)->Title;
		Button->Child = Text;
		std::vector<int> FeedPath = InFeedPath;
		FeedPath.resize(Index + 1);
		Button->OnPushFunction = std::tr1::bind(&CApp::OpenFeed, App, FeedPath, 0, false);
		BreadcrumbHBox->AddChild(Button);
	}
}
