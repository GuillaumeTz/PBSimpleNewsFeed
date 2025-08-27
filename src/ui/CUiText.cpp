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

#include "ui/CUiText.h"

#include "CHtmlParsing.h"
#include "HtmlEntities.h"
#include "CApp.h"

#include "utf8.h"

CUiText::CUiText() : CUiWidget()
{
#ifndef IVSAPP
	MaxWidth = 1100;
#else 
	MaxWidth = 550;
#endif
	Type = UiTextFlags::None;
	bIsEditable = false;

	DebugName = "UiText";
}

void CUiText::SetHtml(const std::string& InHtml)
{
	Text.clear();
	std::string LocalText;

	//std::cerr << "SetBodyHtml : " << InHtml << std::endl;
	for (int Index = 0; Index < InHtml.size(); ++Index)
	{
		char Character = InHtml[Index];
		if (Character == '<')
		{
			if (!LocalText.empty())
			{
				std::string DecodedValue;
				DecodedValue.resize(LocalText.size());
				decode_html_entities_utf8(&DecodedValue[0], &LocalText[0]);

				Text += DecodedValue;
				LocalText.clear();
			}

			EHtmlTag::Type HtmlTagType = CHtmlParsing::GetTagType(InHtml, Index);
			switch (HtmlTagType)
			{
				case EHtmlTag::br:
				case EHtmlTag::div:
				case EHtmlTag::span:
				case EHtmlTag::p:
				case EHtmlTag::h1:
				case EHtmlTag::h2:
				case EHtmlTag::h3:
				case EHtmlTag::h4:
				case EHtmlTag::h5:
				{
					if (!Text.empty())
					{
						Text += "\n";
					}
					CHtmlParsing::SkipTo(InHtml, Index, '>'); 
					--Index;
					break;
				}
				case EHtmlTag::a:
				case EHtmlTag::Unknown:
				case EHtmlTag::img:
				{
					CHtmlParsing::SkipTo(InHtml, Index, '>'); 
					--Index;
					break;
				}
				default:
				{
					break;
				}
			}
		}
		else if (Character == '>')
		{

		}
		else
		{
			LocalText += Character;
		}
	}

	if (!LocalText.empty())
	{
		std::string DecodedValue;
		DecodedValue.resize(LocalText.size());
		decode_html_entities_utf8(&DecodedValue[0], &LocalText[0]);

		Text += DecodedValue;
		LocalText.clear();
	}
}

void CUiText::SetEditable(bool bInEditable)
{
	bIsEditable = bInEditable;
}

void CUiText::CalcDesiredSize(SVector2i AllowedSize)
{
	CUiWidget::CalcDesiredSize(AllowedSize);

	MaxWidth = AllowedSize.X;
	CUiFont* UsedFont = Font.IsValid() ? &Font : &CApp::Get()->AppSettings.DefaultFont;
	SetFont(UsedFont->Font, BLACK);

	CuttingTextIndexes.clear();

	DesiredSize.Y += UsedFont->Font->size;

	int CurrentSize = 0;
	try
	{
		int LastCutIndex = 0;
		utf8::iterator<std::string::iterator> StartIt(Text.begin(), Text.begin(), Text.end());
		utf8::iterator<std::string::iterator> EndIt(Text.end(), Text.begin(), Text.end());
		utf8::iterator<std::string::iterator> LastCutIt(StartIt);
		for (utf8::iterator<std::string::iterator> CharIt(StartIt); CharIt != EndIt; ++CharIt)
		{
			uint32_t CharacterUTF32 = *CharIt;

			if (CharacterUTF32 > 255)
			{
				continue;
			}

			int Index = CharIt.base() - StartIt.base();
			char Character = CharacterUTF32;

			CurrentSize += CharWidth(Character);

			if (Character == ' ' || Character == '-' || Character == ',' || Character == '.')
			{
				LastCutIndex = Index;
				LastCutIt = CharIt;
			}

			if (Character == '\n')
			{
				DesiredSize.X = std::max(CurrentSize, DesiredSize.X);
				DesiredSize.Y += UsedFont->Font->size;

				LastCutIndex = Index;
				LastCutIt = CharIt;
				CuttingTextIndexes.push_back(Index);
				CurrentSize = 0;
			}

			if (CurrentSize >= MaxWidth)
			{
				DesiredSize.X = std::max(MaxWidth, DesiredSize.X);
				DesiredSize.Y += UsedFont->Font->size;

				if (LastCutIndex > 0)
				{
					if (!CuttingTextIndexes.empty() && CuttingTextIndexes.back() == LastCutIndex)
					{
						CuttingTextIndexes.push_back(Index);
						LastCutIndex = Index;
						LastCutIt = CharIt;
					}
					else
					{
						CuttingTextIndexes.push_back(LastCutIndex);
						CharIt = LastCutIt;
						Index = LastCutIndex;
					}
				}
				else
				{
					CuttingTextIndexes.push_back(Index);
				}
				CurrentSize = 0;
			}
		}
	}
	catch (...)
	{

	}

	DesiredSize.X = std::max(CurrentSize, DesiredSize.X);
}

void CUiText::Draw(SUiDrawVisitor& DrawVisitor)
{
	if (/*bNeedRedraw && */Visibility >= EUiWidgetVisibility::VisibleNotInteractable)
	{
		if (!Text.empty())
		{
			CUiFont* UsedFont = Font.IsValid() ? &Font : &CApp::Get()->AppSettings.DefaultFont;
			SetFont(UsedFont->Font, BLACK);

			const int FontSize = UsedFont->Font->size;

			if (CuttingTextIndexes.empty())
			{
				if (FontSize / 2 >= DrawVisitor.StartHeight && FontSize / 2 < DrawVisitor.MaxAllowedHeight)
				{
					DrawString(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, Text.c_str());
					DrawVisitor.MarkHasDrawn(this, SRect(DrawVisitor.AtLocation, DrawVisitor.AtLocation + DesiredSize));
				}
			}
			else
			{
				int StartY = DrawVisitor.AtLocation.Y;
				int CurrentHeight = 0;
				int LastCutIndex = -1;
				for (int Index = 0; Index < CuttingTextIndexes.size(); ++Index)
				{
					int CutIndex = CuttingTextIndexes[Index];
					std::string StringPart = Text.substr(LastCutIndex + 1, CutIndex - (LastCutIndex+1));
					if (CurrentHeight + FontSize / 2 >= DrawVisitor.StartHeight && CurrentHeight + FontSize / 2 < DrawVisitor.MaxAllowedHeight)
					{
						DrawString(DrawVisitor.AtLocation.X, StartY + CurrentHeight, StringPart.c_str());
						DrawVisitor.MarkHasDrawn(this, SRect(SVector2i(DrawVisitor.AtLocation.X, StartY + CurrentHeight), SVector2i(DrawVisitor.AtLocation.X, StartY + CurrentHeight) + SVector2i(DesiredSize.X, FontSize)));
					}
					CurrentHeight += FontSize;
					LastCutIndex = CutIndex;
				}
				std::string StringPart = Text.substr(LastCutIndex + 1, Text.size() - (LastCutIndex+1));
				if (CurrentHeight + FontSize / 2 >= DrawVisitor.StartHeight && CurrentHeight + FontSize / 2 < DrawVisitor.MaxAllowedHeight)
				{
					DrawString(DrawVisitor.AtLocation.X, StartY + CurrentHeight, StringPart.c_str());
					DrawVisitor.MarkHasDrawn(this, SRect(SVector2i(DrawVisitor.AtLocation.X, StartY + CurrentHeight), SVector2i(DrawVisitor.AtLocation.X, StartY + CurrentHeight) + SVector2i(DesiredSize.X, FontSize)));
				}
				CurrentHeight += FontSize;
			}
		}
		/*bNeedRedraw = false;*/

		if (bIsEditable)
		{
			DrawVisitor.SetInteractable(this, SRect(DrawVisitor.AtLocation, DrawVisitor.AtLocation + DesiredSize));
		}

		if (bIsPushed)
		{
			DrawRect(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, DesiredSize.X, DesiredSize.Y, 128);
		}
	}
	
	if (Visibility >= EUiWidgetVisibility::Hidden)
	{
		DrawVisitor.SetVisible(this, SRect(DrawVisitor.AtLocation, DrawVisitor.AtLocation + DesiredSize));
	}
}

void CUiText::DeleteItself()
{
	CUiTextAllocator::Allocator.Delete(this);
}

static CUiText* TextForKeyboard = NULL;
static void OnTextEdited(char* Text)
{
	if (!TextForKeyboard)
		return;

	std::cerr << "OnTextEdited " << Text << std::endl;
	CUiText* TextForKeyboardLocal = TextForKeyboard;
	TextForKeyboard = NULL;

	CloseKeyboard();

	TextForKeyboardLocal->OnEdited(Text);
}

void CUiText::OnClick()
{
	char* Buffer = new char[2048];
	memset(Buffer, 0, sizeof(char) * 2048);
	memcpy(Buffer, Text.c_str(), Text.size());
	TextForKeyboard = this;
	OpenKeyboard("Modify value", Buffer, 2047, 0, &OnTextEdited);
}

void CUiText::OnEdited(char* InText)
{
	if (!InText)
		return;

	//transform to int
	std::string NewText = InText;

	//try to validate input
	bool bIsValid = true;
	for (int Index = 0; Index < NewText.size(); ++Index)
	{
		if (Type == UiTextFlags::Int)
		{
			if (NewText[Index] < '0' || NewText[Index] > '9')
			{
				bIsValid = false;
				break;
			}
		}
		else if (Type == UiTextFlags::Float)
		{
			if ((NewText[Index] != '.') && (NewText[Index] < '0' || NewText[Index] > '9'))
			{
				bIsValid = false;
				break;
			}
		}
	}

	if (!bIsValid)
		return;

	Text = NewText;
	if (OnEditedFunction)
	{
		OnEditedFunction(this);
	}
	CApp::Get()->RedrawWidget(this);
	delete[] InText;
}
