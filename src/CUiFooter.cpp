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

#include "CUiFooter.h"
#include "CApp.h"

#include "ui/CUiButton.h"
#include "ui/CUiText.h"
#include "ui/CUiLine.h"
#include "ui/CUiVerticalBox.h"

CUiFooter::CUiFooter() : CUiHorizontalBox()
{
	bFillWidth = true;

	DebugName = "UiFooter";

	CApp* App = CApp::Get();

	CUiVerticalBox* VertBox = new CUiVerticalBox();
	VertBox->bFillWidth = true;
	AddChild(VertBox);

	{
		CUiLine* Line = CUiLineAllocator::New();
		Line->bVertical = false;
		VertBox->AddChild(Line);
	}

	{
		CUiLine* Line = CUiLineAllocator::New();
		Line->bVertical = false;
		Line->SetPadding(0, 2, 0, 0);
		VertBox->AddChild(Line);
	}

	CUiHorizontalBox* ButtonHBar = new CUiHorizontalBox();
	ButtonHBar->bFillWidth = true;
	VertBox->AddChild(ButtonHBar);
	
	{
		CUiButton* Button = new CUiButton();
		CUiText* Text = CUiTextAllocator::New();
		Text->Font = App->AppSettings.MenuButtonFont;
		Text->Text = "Home";
		Button->Child = Text;
		Button->OnPushFunction = std::tr1::bind(&CApp::OpenMainPage, App);
		Button->SetPadding(20, 10, 20, 20);
		Button->PivotPointRatio.X = 0.5f;
		ButtonHBar->AddChild(Button);
	}

	{
		CUiButton* Button = new CUiButton();
		CUiText* Text = CUiTextAllocator::New();
		Text->Font = App->AppSettings.MenuButtonFont;
		Text->Text = "<=";
		Button->Child = Text;
		Button->OnPushFunction = std::tr1::bind(&CApp::PreviousPage, App);
		Button->SetPadding(30, 10, 30, 20);
		Button->PivotPointRatio.X = 0.5f;
		ButtonHBar->AddChild(Button);
	}

	{
		CUiButton* Button = new CUiButton();
		CUiText* Text = CUiTextAllocator::New();
		Text->Font = App->AppSettings.MenuButtonFont;
		Text->Text = "=>";
		Button->Child = Text;
		Button->OnPushFunction = std::tr1::bind(&CApp::NextPage, App);
		Button->SetPadding(30, 10, 30, 20);
		Button->PivotPointRatio.X = 0.5f;
		ButtonHBar->AddChild(Button);
	}

	{
		CUiButton* Button = new CUiButton();
		CUiText* Text = CUiTextAllocator::New();
		Text->Font = App->AppSettings.MenuButtonFont;
		Text->Text = "Back";
		Button->Child = Text;
		Button->OnPushFunction = std::tr1::bind(&CApp::GoBack, App);
		Button->SetPadding(20, 10, 20, 20);
		Button->PivotPointRatio.X = 0.5f;
		ButtonHBar->AddChild(Button);
	}
}
