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
	MainVerticalBox = NULL;
	DownloadCounterText = NULL;
	bFillWidth = true;
	bFillHeight = true;

	DebugName = "UIMainPage";
}

void CUiMainPage::Refresh()
{
	std::cerr << "Main Page Refresh" << std::endl;
	CApp* App = CApp::Get();

	ClearChildren();
	MainVerticalBox = new CUiVerticalBox();
	MainVerticalBox->DebugName = "MainPageVBox";
	AddChild(MainVerticalBox);
	MainVerticalBox->SetPadding(20, 0, 20, 0);

	// add header bar
	{
		{
			CUiLine* Line = CUiLineAllocator::New();
			Line->bVertical = false;
			MainVerticalBox->AddChild(Line);
		}

		CUiHorizontalBox* HeaderBarHBox = new CUiHorizontalBox();
		HeaderBarHBox->DebugName = "HeaderBarHBox";
		HeaderBarHBox->bFillWidth = true;

		{
			CUiButton* HomeButton = new CUiButton();
			CUiText* Text = CUiTextAllocator::New();
			Text->Font = App->AppSettings.MenuButtonFont;
			Text->Text = "X";
			HomeButton->Child = Text;
			HomeButton->OnPushFunction = std::tr1::bind(&CApp::QuitApplication, App);
			HomeButton->SetPadding(10, 10, 10, 10);
			HeaderBarHBox->AddChild(HomeButton);
		}

		{
			CUiButton* SyncButton = new CUiButton();
			CUiText* Text = CUiTextAllocator::New();
			Text->Font = App->AppSettings.MenuButtonFont;
			Text->Text = "Sync";
			SyncButton->Child = Text;
			SyncButton->OnPushFunction = std::tr1::bind(&CApp::SyncAll, App);
			SyncButton->SetPadding(10, 10, 10, 10);
			HeaderBarHBox->AddChild(SyncButton);
		}

		{
			CUiButton* DownloadButton = new CUiButton();
			CUiText* Text = CUiTextAllocator::New();
			Text->Font = App->AppSettings.MenuButtonFont;
			Text->Text = "Download";
			DownloadButton->Child = Text;
			DownloadButton->OnPushFunction = std::tr1::bind(&CApp::DLReaderModeAll, App);
			DownloadButton->SetPadding(10, 10, 10, 10);
			HeaderBarHBox->AddChild(DownloadButton);
		}

		{
			CUiButton* Button = new CUiButton();
			CUiText* Text = CUiTextAllocator::New();
			Text->Font = App->AppSettings.MenuButtonFont;
			Text->Text = "Settings";
			Button->Child = Text;
			Button->OnPushFunction = std::tr1::bind(&CApp::OpenSettingsPage, App);
			Button->SetPadding(10, 10, 10, 10);
			HeaderBarHBox->AddChild(Button);
		}

		{
			CUiButton* BackButton = new CUiButton();
			CUiText* Text = CUiTextAllocator::New();
			Text->Font = App->AppSettings.MenuButtonFont;
			Text->Text = "<=";
			BackButton->Child = Text;
			BackButton->OnPushFunction = std::tr1::bind(&CApp::GoBack, App);
			BackButton->SetPadding(10, 10, 10, 10);
			HeaderBarHBox->AddChild(BackButton);
		}

		MainVerticalBox->AddChild(HeaderBarHBox);

		{
			CUiLine* Line = CUiLineAllocator::New();
			Line->bVertical = false;
			MainVerticalBox->AddChild(Line);
		}
	}

	{
		CUiVerticalBox* VertBox = new CUiVerticalBox();
		VertBox->DebugName = "DetailsVertBox";
		MainVerticalBox->AddChild(VertBox);

		{
			CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
			HorizontalBox->DebugName = "BreadCrumbHBox";
			HorizontalBox->bFillWidth = true;
			VertBox->AddChild(HorizontalBox);

			BreadcrumbHBox = new CUiHorizontalBox();
			HorizontalBox->AddChild(BreadcrumbHBox);

			{
				DownloadCounterText = CUiTextAllocator::New();
				DownloadCounterText->Font = App->AppSettings.FeedPathFont;
				DownloadCounterText->PivotPointRatio.X = 1.f;
				DownloadCounterText->SetPadding(0, 5, 0, 5);
				HorizontalBox->AddChild(DownloadCounterText);
				RefreshDownloadCounter();
			}
		}

		
		{
			CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
			HorizontalBox->DebugName = "FoldersHBox";
			VertBox->AddChild(HorizontalBox);

			{
				CUiButton* FoldersButton = new CUiButton();
				CUiText* Text = CUiTextAllocator::New();
				Text->Font = App->AppSettings.SwitchViewButtonsFont;
				Text->Text = "Folders";
				FoldersButton->Child = Text;
				FoldersButton->OnPushFunction = std::tr1::bind(&CApp::ShowFolders, App);
				FoldersButton->SetPadding(0, 0, 15, 5);
				HorizontalBox->AddChild(FoldersButton);
			}

			{
				CUiButton* LastEntriesButton = new CUiButton();
				CUiText* Text = CUiTextAllocator::New();
				Text->Font = App->AppSettings.SwitchViewButtonsFont;
				Text->Text = "Last Entries";
				LastEntriesButton->Child = Text;
				LastEntriesButton->OnPushFunction = std::tr1::bind(&CApp::ShowLastEntries, App);
				LastEntriesButton->SetPadding(15, 0, 15, 5);
				HorizontalBox->AddChild(LastEntriesButton);
			}

			{
				CUiButton* SyncButton = new CUiButton();
				CUiText* Text = CUiTextAllocator::New();
				Text->Font = App->AppSettings.SwitchViewButtonsFont;
				Text->Text = "Sync";
				SyncButton->Child = Text;
				SyncButton->OnPushFunction = std::tr1::bind(&CApp::SyncCurrent, App);
				SyncButton->SetPadding(15, 0, 15, 5);
				HorizontalBox->AddChild(SyncButton);
			}

			{
				CUiButton* SyncButton = new CUiButton();
				CUiText* Text = CUiTextAllocator::New();
				Text->Font = App->AppSettings.SwitchViewButtonsFont;
				Text->Text = "Download";
				SyncButton->Child = Text;
				SyncButton->OnPushFunction = std::tr1::bind(&CApp::DLReaderModeCurrent, App);
				SyncButton->SetPadding(15, 0, 15, 5);
				HorizontalBox->AddChild(SyncButton);
			}

			{
				CUiButton* SyncButton = new CUiButton();
				CUiText* Text = CUiTextAllocator::New();
				Text->Font = App->AppSettings.SwitchViewButtonsFont;
				Text->Text = "Mark as read";
				SyncButton->Child = Text;
				SyncButton->OnPushFunction = std::tr1::bind(&CApp::MarkAsReadCurrent, App);
				SyncButton->SetPadding(15, 0, 15, 5);
				HorizontalBox->AddChild(SyncButton);
			}
		}

		CUiLine* Line = CUiLineAllocator::New();
		Line->SetBottomPadding(10);
		VertBox->AddChild(Line);
	}

	IndexOfMainElement = MainVerticalBox->GetChildren()->size();

	SetPath({});

	std::cerr << "End Main Page Refresh" << std::endl;
}

void CUiMainPage::RefreshDownloadCounter()
{
	CDownloadManager* DownloadManager = CDownloadManager::Get();

	if (DownloadManager->NumDownloadRemaining > 0)
	{
		std::stringstream stream;
		if (bSyncing)
		{
			stream << "Syncing " << DownloadManager->NumDownloadRemaining << "..." << std::endl;
		}
		else
		{
			stream << "Downloading " << DownloadManager->NumDownloadRemaining << "..." << std::endl;
		}
		DownloadCounterText->Text = stream.str();
	}
	else
	{
		DownloadCounterText->Text = "                      ";
	}
}

void CUiMainPage::SetMainElement(CUiWidget* Widget)
{
	if (!MainVerticalBox)
		return;

	MainVerticalBox->GetChildren()->resize(IndexOfMainElement);
	MainVerticalBox->AddChild(Widget);
}

CUiWidget* CUiMainPage::GetMainElement()
{
	if (!MainVerticalBox)
		return NULL;

	if ((*MainVerticalBox->GetChildren()).size() > IndexOfMainElement)
	{
		return (*MainVerticalBox->GetChildren())[IndexOfMainElement].Get();
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
	Button->SetPadding(0, 5, 1, 5);
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
		Button->SetPadding(1, 5, 1, 5);
		std::vector<int> FeedPath = InFeedPath;
		FeedPath.resize(Index + 1);
		Button->OnPushFunction = std::tr1::bind(&CApp::OpenFeed, App, FeedPath, 0, false);
		BreadcrumbHBox->AddChild(Button);
	}
}
