#include "CUiSettingsPage.h"
#include "CApp.h"

#include "ui/CUiVerticalBox.h"
#include "ui/CUiHorizontalBox.h"
#include "ui/CUiButton.h"
#include "ui/CUiText.h"
#include "ui/CUiSpacer.h"

#include <iostream>

CUiSettingsPage::CUiSettingsPage() : CUiHorizontalBox()
{
	bFill = true;
}

void CUiSettingsPage::Refresh()
{
	CApp* App = CApp::Get();
	LocalAppSettings = App->AppSettings;

	ClearChildren();

	CUiVerticalBox* VerticalBox = new CUiVerticalBox();
	AddChild(VerticalBox);
	VerticalBox->Padding.TopLeft.X = 20;
	VerticalBox->Padding.BottomRight.X = 20;

	// Choose Opml file
	{
		CUiText* Text = CUiTextAllocator::New();
		Text->Text = "OPML file location : ";
		VerticalBox->AddChild(Text);

		{
			CUiButton* Button = new CUiButton();
			//TODO make a real file selector dialog Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::SelectOpmlFile, this);
			VerticalBox->AddChild(Button);

			CUiText* OpmlFilePathText = CUiTextAllocator::New();
			OpmlFilePathText->Text = LocalAppSettings.PathToOPML;
			Button->Child = OpmlFilePathText;
			Button->PivotPointRatio.X = 1.f;
			Button->Padding.BottomRight.Y = 20;
		}
	}

	// Choose font size

	// Choose max number of last articles to download
	{
		CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
		HorizontalBox->bFill = true;
		HorizontalBox->Padding.BottomRight.Y = 20;
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

	{ // Clear cache
		CUiButton* Button = new CUiButton();
		Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::ClearCache, this);
		Button->PivotPointRatio.X = 1.f;
		//VerticalBox->AddChild(Button);

		CUiText* Text = CUiTextAllocator::New();
		Text->Text = "CLEAR CACHE";
		Button->Child = Text;
		Button->Padding.BottomRight.X = 40;
	}

	// Cancel Save
	{
		CUiHorizontalBox* CancelSaveHorizontalBox = new CUiHorizontalBox();
		CancelSaveHorizontalBox->bFill = true;
		CancelSaveHorizontalBox->PivotPointRatio.X = 1.f;
		VerticalBox->AddChild(CancelSaveHorizontalBox);

		{ // Cancel
			CUiButton* Button = new CUiButton();
			Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::Cancel, this);
			CancelSaveHorizontalBox->AddChild(Button);

			CUiText* OpmlFilePathText = CUiTextAllocator::New();
			OpmlFilePathText->Text = "CANCEL";
			Button->Child = OpmlFilePathText;
			Button->Padding.BottomRight.X = 40;
		}

		{ // Save
			CUiButton* Button = new CUiButton();
			Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::Save, this);
			CancelSaveHorizontalBox->AddChild(Button);

			CUiText* OpmlFilePathText = CUiTextAllocator::New();
			OpmlFilePathText->Text = "SAVE";
			Button->Child = OpmlFilePathText;
		}
	}
}

static CUiSettingsPage* SettingsPage = NULL;
static void OnSelectOpmlFileComplete(char* Filepath)
{
	CUiSettingsPage* SettingsPageLocal = SettingsPage;
	SettingsPage = NULL;

	std::cout << "Selected opml file complete : " << Filepath << std::endl;
}

void CUiSettingsPage::SelectOpmlFile()
{
	SettingsPage = this;
	char* Buffer = new char[4096];
	memset(Buffer, 0, sizeof(char) * 4096);
	OpenDirectorySelector("Selet OMPL File", Buffer, 4095, &OnSelectOpmlFileComplete);
}

void CUiSettingsPage::OnSelectedOpmlFile(char* SelectedFile)
{
	
}

void CUiSettingsPage::OnMaxEntryToKeepChanged(CUiText* Text)
{
	LocalAppSettings.MaxEntryToKeepByFeed = atoi(Text->Text.c_str());
	LocalAppSettings.MaxEntryToKeepByFeed = LocalAppSettings.MaxEntryToKeepByFeed < 1 ? 1 : LocalAppSettings.MaxEntryToKeepByFeed;
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
