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
	OriginalAppSettings = App->AppSettings;

	InternalRefresh();
}

void CUiSettingsPage::InternalRefresh()
{
	CApp* App = CApp::Get();

	ClearChildren();

	CUiVerticalBox* VerticalBox = new CUiVerticalBox();
	VerticalBox->bSupportMultiplePages = true;
	AddChild(VerticalBox);
	VerticalBox->SetPadding(20, 20, 20, 20);

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

	{ // Reset
		CUiButton* Button = new CUiButton();
		Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::ResetConfig, this);
		Button->PivotPointRatio.X = 0.5f;
		VerticalBox->AddChild(Button);

		CUiText* Text = CUiTextAllocator::New();
		Text->Text = "Reset config";
		Button->Child = Text;
		Button->SetPadding(20, 10, 20, 10);
	}

	// Choose Opml file
	{
		CUiText* Text = CUiTextAllocator::New();
		Text->Text = "OPML file location : ";
		Text->SetBottomPadding(10);
		VerticalBox->AddChild(Text);

		{
			CUiText* OpmlFilePathText = CUiTextAllocator::New();
			OpmlFilePathText->Text = App->AppSettings.PathToOPML;
			OpmlFilePathText->SetEditable(true);
			OpmlFilePathText->Type = UiTextFlags::Int;
			OpmlFilePathText->OnEditedFunction = std::tr1::bind(&CUiSettingsPage::OnOpmlFilePathChanged, this, std::tr1::placeholders::_1);
			OpmlFilePathText->SetBottomPadding(25);
			VerticalBox->AddChild(OpmlFilePathText);
		}
	}

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
			MaxNumberText->Text = CUtils::ToString(App->AppSettings.MaxEntryToKeepByFeed);
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
				MaxNumberText->Text = CUtils::ToString(App->AppSettings.ResolutionWidth);
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
				MaxNumberText->Text = CUtils::ToString(App->AppSettings.ResolutionHeight);
				MaxNumberText->SetEditable(true);
				MaxNumberText->Type = UiTextFlags::Int;
				MaxNumberText->OnEditedFunction = std::tr1::bind(&CUiSettingsPage::OnResolutionHeightChanged, this, std::tr1::placeholders::_1);
				MaxNumberText->PivotPointRatio.X = 1.f;
				HorizontalBox->AddChild(MaxNumberText);
			}
		}
	}

	// Change scale
	{
		{
			CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
			HorizontalBox->bFillWidth = true;
			HorizontalBox->SetBottomPadding(25);
			VerticalBox->AddChild(HorizontalBox);

			CUiText* Text = CUiTextAllocator::New();
			Text->Text = "Scale : ";
			HorizontalBox->AddChild(Text);

			{
				CUiText* MaxNumberText = CUiTextAllocator::New();
				MaxNumberText->Text = CUtils::ToString(App->AppSettings.GetScale());
				MaxNumberText->SetEditable(true);
				MaxNumberText->Type = UiTextFlags::Float;
				MaxNumberText->OnEditedFunction = std::tr1::bind(&CUiSettingsPage::OnScaleChanged, this, std::tr1::placeholders::_1);
				MaxNumberText->PivotPointRatio.X = 1.f;
				HorizontalBox->AddChild(MaxNumberText);
			}
		}
	}

	{ // Synchronize at start
		CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
		HorizontalBox->bFillWidth = true;
		HorizontalBox->SetBottomPadding(25);
		VerticalBox->AddChild(HorizontalBox);

		{
			CUiText* Text = CUiTextAllocator::New();
			Text->Text = "Synchronize at start : ";
			HorizontalBox->AddChild(Text);
		}

		CUiButton* Button = new CUiButton();
		Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::OnToggleSynchronizeAtStart, this);
		Button->PivotPointRatio.X = 1.0f;
		HorizontalBox->AddChild(Button);

		CUiText* Text = CUiTextAllocator::New();
		Text->Text = App->AppSettings.bSynchronizeAtStart ? "yes" : "no";
		Button->Child = Text;
		Button->SetPadding(20, 10, 20, 10);
	}

	{ // Show / hide navigation footer
		CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
		HorizontalBox->bFillWidth = true;
		HorizontalBox->SetBottomPadding(25);
		VerticalBox->AddChild(HorizontalBox);

		{
			CUiText* Text = CUiTextAllocator::New();
			Text->Text = "Hide footer navigation : ";
			HorizontalBox->AddChild(Text);
		}

		CUiButton* Button = new CUiButton();
		Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::OnToggleNavigationFooter, this);
		Button->PivotPointRatio.X = 1.0f;
		HorizontalBox->AddChild(Button);

		CUiText* Text = CUiTextAllocator::New();
		Text->Text = App->AppSettings.bHideFooterNavigation ? "yes" : "no";
		Button->Child = Text;
		Button->SetPadding(20, 10, 20, 10);
	}

	{ // Clear cache
		CUiButton* Button = new CUiButton();
		Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::ClearCache, this);
		Button->PivotPointRatio.X = 0.5f;
		VerticalBox->AddChild(Button);

		CUiText* Text = CUiTextAllocator::New();
		Text->Text = "CLEAR CACHE";
		Button->Child = Text;
		Button->SetPadding(20, 10, 20, 10);
	}

	// Cancel Save
	{
		CUiLine* Line = CUiLineAllocator::New();
		Line->bVertical = false;
		Line->SetPadding(0, 10, 0, 10);
		VerticalBox->AddChild(Line);

		CUiHorizontalBox* CancelSaveHorizontalBox = new CUiHorizontalBox();
		CancelSaveHorizontalBox->bFillWidth = true;
		VerticalBox->AddChild(CancelSaveHorizontalBox);

		{ // Cancel
			CUiButton* Button = new CUiButton();
			Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::Cancel, this);
			Button->PivotPointRatio.X = 0.5f;
			CancelSaveHorizontalBox->AddChild(Button);

			CUiText* OpmlFilePathText = CUiTextAllocator::New();
			OpmlFilePathText->Text = "CANCEL";
			Button->Child = OpmlFilePathText;
			Button->SetPadding(30, 30, 30, 30);
		}

		{ // Save
			CUiButton* Button = new CUiButton();
			Button->OnPushFunction = std::tr1::bind(&CUiSettingsPage::Save, this);
			Button->PivotPointRatio.X = 0.5f;
			CancelSaveHorizontalBox->AddChild(Button);

			CUiText* OpmlFilePathText = CUiTextAllocator::New();
			OpmlFilePathText->Text = "SAVE";
			Button->Child = OpmlFilePathText;
			Button->SetPadding(30, 30, 30, 30);
		}
	}
}
void CUiSettingsPage::OnMaxEntryToKeepChanged(CUiText* Text)
{
	CApp* App = CApp::Get();

	App->AppSettings.MaxEntryToKeepByFeed = atoi(Text->Text.c_str());
	App->AppSettings.MaxEntryToKeepByFeed = App->AppSettings.MaxEntryToKeepByFeed < 1 ? 1 : App->AppSettings.MaxEntryToKeepByFeed;
	Text->Text = CUtils::ToString(App->AppSettings.MaxEntryToKeepByFeed);
	App->Draw();
}

void CUiSettingsPage::OnResolutionWidthChanged(CUiText* Text)
{
	CApp* App = CApp::Get();
	App->AppSettings.ResolutionWidth = std::max(480, atoi(Text->Text.c_str()));
	App->AppSettings.ResolutionWidth = std::min(App->AppSettings.ResolutionWidth, ScreenWidth());
	Text->Text = CUtils::ToString(App->AppSettings.ResolutionWidth);
	App->Draw();
}

void CUiSettingsPage::OnResolutionHeightChanged(CUiText* Text)
{
	CApp* App = CApp::Get();
	App->AppSettings.ResolutionHeight = std::max(640, atoi(Text->Text.c_str()));
	App->AppSettings.ResolutionHeight = std::min(App->AppSettings.ResolutionHeight, ScreenHeight());
	Text->Text = CUtils::ToString(App->AppSettings.ResolutionHeight);
	App->Draw();
}

void CUiSettingsPage::OnOpmlFilePathChanged(CUiText* Text)
{
	CApp* App = CApp::Get();
	App->AppSettings.PathToOPML = Text->Text;
	App->Draw();
}

void CUiSettingsPage::OnScaleChanged(CUiText* Text)
{
	CApp* App = CApp::Get();
	App->AppSettings.SetScale(atof(Text->Text.c_str()));
	App->Draw();
}

void CUiSettingsPage::OnToggleNavigationFooter()
{
	CApp* App = CApp::Get();
	App->AppSettings.bHideFooterNavigation = !App->AppSettings.bHideFooterNavigation;
	InternalRefresh();
	App->Draw();
}

void CUiSettingsPage::OnToggleSynchronizeAtStart()
{
	CApp* App = CApp::Get();
	App->AppSettings.bSynchronizeAtStart = !App->AppSettings.bSynchronizeAtStart;
	InternalRefresh();
	App->Draw();
}

void CUiSettingsPage::Save()
{
	CApp* App = CApp::Get();
	App->AppSettings.SaveConfig();
	App->OpenMainPage();
}

void CUiSettingsPage::Cancel()
{
	CApp* App = CApp::Get();
	App->AppSettings = OriginalAppSettings;
	App->OpenMainPage();
}

void CUiSettingsPage::ClearCache()
{
	CApp* App = CApp::Get();
	ShowHourglass();
	App->ClearCache();
	HideHourglass();
	App->Draw();
}

void CUiSettingsPage::ResetConfig()
{
	CApp* App = CApp::Get();
	App->AppSettings.Reset();
	InternalRefresh();
	App->Draw();
}