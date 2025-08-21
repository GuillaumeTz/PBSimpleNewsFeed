#include "CUiSettingsPage.h"
#include "CApp.h"

#include "ui/CUiVerticalBox.h"
#include "ui/CUiHorizontalBox.h"
#include "ui/CUiButton.h"
#include "ui/CUiText.h"
#include "ui/CUiSpacer.h"
#include "ui/CUiLine.h"

#include <iostream>

CUiSettingsPage::CUiSettingsPage() : CUiHorizontalBox()
{
	bFillWidth = true;
	bFillHeight = true;
}

void CUiSettingsPage::Refresh()
{
	CApp* App = CApp::Get();
	LocalAppSettings = App->AppSettings;

	ClearChildren();

	CUiVerticalBox* VerticalBox = new CUiVerticalBox();
	VerticalBox->bSupportMultiplePages = true;
	AddChild(VerticalBox);
	VerticalBox->SetPadding(20, 0, 20, 0);

	{
		CUiText* Text = CUiTextAllocator::New();
		Text->Text = "SETTINGS";
		Text->Font = App->AppSettings.MenuButtonFont;
		Text->SetBottomPadding(10);
		VerticalBox->AddChild(Text);

		CUiLine* Line = CUiLineAllocator::New();
		Line->bVertical = false;
		Line->SetPadding(0, 10, 0, 10);
		VerticalBox->AddChild(Line);
	}

	// Choose Opml file
	{
		CUiText* Text = CUiTextAllocator::New();
		Text->Text = "OPML file location : ";
		Text->SetBottomPadding(10);
		VerticalBox->AddChild(Text);

		{
			CUiText* OpmlFilePathText = CUiTextAllocator::New();
			OpmlFilePathText->Text = LocalAppSettings.PathToOPML;
			OpmlFilePathText->SetEditable(true);
			OpmlFilePathText->Type = UiTextFlags::Int;
			OpmlFilePathText->OnEditedFunction = std::tr1::bind(&CUiSettingsPage::OnOpmlFilePathChanged, this, std::tr1::placeholders::_1);
			OpmlFilePathText->SetBottomPadding(25);
			VerticalBox->AddChild(OpmlFilePathText);
		}
	}

	// Choose font size

	// Choose max number of last articles to download
	{
		CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
		HorizontalBox->bFillWidth = true;
		HorizontalBox->SetBottomPadding(25);
		VerticalBox->AddChild(HorizontalBox);

		CUiText* Text = CUiTextAllocator::New();
		Text->Text = "Max number of entries to keep by feed : ";
		HorizontalBox->AddChild(Text);

		{
			CUiText* MaxNumberText = CUiTextAllocator::New();
			MaxNumberText->Text = CUtils::ToString(LocalAppSettings.MaxEntryToKeepByFeed);
			MaxNumberText->SetEditable(true);
			MaxNumberText->Type = UiTextFlags::Int;
			MaxNumberText->OnEditedFunction = std::tr1::bind(&CUiSettingsPage::OnMaxEntryToKeepChanged, this, std::tr1::placeholders::_1);
			MaxNumberText->PivotPointRatio.X = 1.f;
			HorizontalBox->AddChild(MaxNumberText);
		}
	}

	// Change resolution
	{
		{
			CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
			HorizontalBox->bFillWidth = true;
			HorizontalBox->SetBottomPadding(25);
			VerticalBox->AddChild(HorizontalBox);

			CUiText* Text = CUiTextAllocator::New();
			Text->Text = "Screen Width : ";
			HorizontalBox->AddChild(Text);

			{
				CUiText* MaxNumberText = CUiTextAllocator::New();
				MaxNumberText->Text = CUtils::ToString(LocalAppSettings.ResolutionWidth);
				MaxNumberText->SetEditable(true);
				MaxNumberText->Type = UiTextFlags::Int;
				MaxNumberText->OnEditedFunction = std::tr1::bind(&CUiSettingsPage::OnResolutionWidthChanged, this, std::tr1::placeholders::_1);
				MaxNumberText->PivotPointRatio.X = 1.f;
				HorizontalBox->AddChild(MaxNumberText);
			}
		}

		{
			CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
			HorizontalBox->bFillWidth = true;
			HorizontalBox->SetBottomPadding(25);
			VerticalBox->AddChild(HorizontalBox);

			CUiText* Text = CUiTextAllocator::New();
			Text->Text = "Screen Height : ";
			HorizontalBox->AddChild(Text);

			{
				CUiText* MaxNumberText = CUiTextAllocator::New();
				MaxNumberText->Text = CUtils::ToString(LocalAppSettings.ResolutionHeight);
				MaxNumberText->SetEditable(true);
				MaxNumberText->Type = UiTextFlags::Int;
				MaxNumberText->OnEditedFunction = std::tr1::bind(&CUiSettingsPage::OnResolutionHeightChanged, this, std::tr1::placeholders::_1);
				MaxNumberText->PivotPointRatio.X = 1.f;
				HorizontalBox->AddChild(MaxNumberText);
			}
		}
	}

	{ // Clear cache
		CUiButton* Button = new CUiButton();
		Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::ClearCache, this);
		Button->PivotPointRatio.X = 1.f;
		VerticalBox->AddChild(Button);

		CUiText* Text = CUiTextAllocator::New();
		Text->Text = "CLEAR CACHE";
		Button->Child = Text;
		Button->SetBottomPadding(40);
	}

	// Cancel Save
	{
		CUiHorizontalBox* CancelSaveHorizontalBox = new CUiHorizontalBox();
		CancelSaveHorizontalBox->bFillWidth = true;
		CancelSaveHorizontalBox->PivotPointRatio.X = 1.f;
		VerticalBox->AddChild(CancelSaveHorizontalBox);

		{ // Cancel
			CUiButton* Button = new CUiButton();
			Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::Cancel, this);
			CancelSaveHorizontalBox->AddChild(Button);

			CUiText* OpmlFilePathText = CUiTextAllocator::New();
			OpmlFilePathText->Text = "CANCEL";
			Button->Child = OpmlFilePathText;
			Button->SetPadding(10, 0, 10, 0);
		}

		{ // Save
			CUiButton* Button = new CUiButton();
			Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::Save, this);
			CancelSaveHorizontalBox->AddChild(Button);

			CUiText* OpmlFilePathText = CUiTextAllocator::New();
			OpmlFilePathText->Text = "SAVE";
			Button->Child = OpmlFilePathText;
			Button->SetPadding(10, 0, 10, 0);
		}
	}
}

static CUiSettingsPage* SettingsPage = NULL;
static void OnSelectOpmlFileComplete(char* Filepath)
{
	CUiSettingsPage* SettingsPageLocal = SettingsPage;
	SettingsPage = NULL;

	SettingsPageLocal->OnSelectedOpmlFile(Filepath);
}

void CUiSettingsPage::SelectOpmlFile()
{
	SettingsPage = this;
	char* Buffer = new char[4096];
	memset(Buffer, 0, sizeof(char) * 4096);
	memcpy(Buffer, LocalAppSettings.PathToOPML.c_str(), LocalAppSettings.PathToOPML.size());
	OpenDirectorySelector("Select OMPL File", Buffer, 4095, &OnSelectOpmlFileComplete);
}

void CUiSettingsPage::OnSelectedOpmlFile(char* SelectedFile)
{
	std::cout << "Selected opml file complete : " << SelectedFile << std::endl;
}

void CUiSettingsPage::OnMaxEntryToKeepChanged(CUiText* Text)
{
	LocalAppSettings.MaxEntryToKeepByFeed = atoi(Text->Text.c_str());
	LocalAppSettings.MaxEntryToKeepByFeed = LocalAppSettings.MaxEntryToKeepByFeed < 1 ? 1 : LocalAppSettings.MaxEntryToKeepByFeed;
	Text->Text = CUtils::ToString(LocalAppSettings.MaxEntryToKeepByFeed);
}

void CUiSettingsPage::OnResolutionWidthChanged(CUiText* Text)
{
	LocalAppSettings.ResolutionWidth = std::max(480, atoi(Text->Text.c_str()));
	LocalAppSettings.ResolutionWidth = std::min(LocalAppSettings.ResolutionWidth, ScreenWidth());
	Text->Text = CUtils::ToString(LocalAppSettings.ResolutionWidth);
}

void CUiSettingsPage::OnResolutionHeightChanged(CUiText* Text)
{
	LocalAppSettings.ResolutionHeight = std::max(640, atoi(Text->Text.c_str()));
	LocalAppSettings.ResolutionHeight = std::min(LocalAppSettings.ResolutionHeight, ScreenHeight());
	Text->Text = CUtils::ToString(LocalAppSettings.ResolutionHeight);
}

void CUiSettingsPage::OnOpmlFilePathChanged(CUiText* Text)
{
	LocalAppSettings.PathToOPML = Text->Text;
}

void CUiSettingsPage::Save()
{
	CApp* App = CApp::Get();
	App->AppSettings = LocalAppSettings;
	App->AppSettings.SaveConfig();
	App->OpenMainPage();
}

void CUiSettingsPage::Cancel()
{
	CApp* App = CApp::Get();
	App->OpenMainPage();
}

void CUiSettingsPage::ClearCache()
{
	CApp* App = CApp::Get();
	App->ClearCache();
}
