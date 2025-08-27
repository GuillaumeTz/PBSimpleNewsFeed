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

#include "ui/CUiVerticalBox.h"
#include "ui/CUiText.h"
#include "CApp.h"

#include "inkview.h"

#include <climits>

CUiVerticalBox::CUiVerticalBox() : CUiCompositeWidget()
{
	CurrentPage = 0;
	MaxPage = 1;
	bSupportMultiplePages = false;

	DebugName = "VerticalBox";
}

CUiVerticalBox::~CUiVerticalBox()
{

}

void CUiVerticalBox::CalcDesiredSize(SVector2i AllowedSize)
{
	CUiCompositeWidget::CalcDesiredSize(AllowedSize);

	bool bFirstPass = true;
	bool bDirty = true;
	while (bDirty)
	{
		bDirty = false;

		MaxPage = 1;
		DesiredSize = SVector2i(0);

		int CurrentHeight = 0;
		bool bHasDrawnFirstInPage = false;

		int LocalIndexPage = 0;
		for (int Index = 0; Index < (*GetChildren()).size(); ++Index)
		{
			const int LocalIndexPageMinHeight = LocalIndexPage * AllowedSize.Y;
			const int LocalIndexPageMaxHeight = (LocalIndexPage + 1) * AllowedSize.Y;

			CUiWidget* Widget = (*GetChildren())[Index].Get();

			if (CurrentHeight >= LocalIndexPageMaxHeight)
			{
				if (!bSupportMultiplePages)
					break;

				LocalIndexPage += 1;
				--Index;
				continue;
			}

			if (CurrentHeight + Widget->DesiredSize.Y >= LocalIndexPageMinHeight && (!bHasDrawnFirstInPage || CurrentHeight < LocalIndexPageMaxHeight))
			{
				SVector2i ChildAllowedSize = AllowedSize;
				ChildAllowedSize.X = AllowedSize.X - Widget->GetPaddingAlongX();
				ChildAllowedSize.Y = AllowedSize.Y - (CurrentHeight - LocalIndexPageMinHeight) - Widget->GetPaddingAlongY();
				Widget->CalcDesiredSize(ChildAllowedSize);
				// std::cout << DebugName << " CalcDesiredSize for " << Widget->DebugName << " " << ChildAllowedSize.X << " " << ChildAllowedSize.Y << " DesiredSize " << Widget->DesiredSize.X << " " << Widget->DesiredSize.Y << " at Heigt " << CurrentHeight << std::endl;
			}

			if (CurrentHeight + Widget->DesiredSize.Y >= LocalIndexPageMinHeight && (!bHasDrawnFirstInPage || CurrentHeight < LocalIndexPageMaxHeight))
			{
				bHasDrawnFirstInPage = true;
				DesiredSize.X = std::max(DesiredSize.X, Widget->DesiredSize.X + Widget->GetPaddingAlongX());
				DesiredSize.Y += Widget->DesiredSize.Y + Widget->GetPaddingAlongY();
			}

			CurrentHeight += Widget->DesiredSize.Y + Widget->GetPaddingAlongY();
		}

		DesiredSize.X = bFillWidth ? AllowedSize.X : std::min(AllowedSize.X, DesiredSize.X);
		DesiredSize.Y = bFillHeight ? AllowedSize.Y : std::min(AllowedSize.Y, DesiredSize.Y);
		MaxPage = LocalIndexPage + 1;

		if (bFirstPass && MaxPage > 1)
		{
			AllowedSize.Y -= 25;
			bDirty = true;
		}
		bFirstPass = false;
	}
	// std::cout << " MaxPage " << MaxPage << " " << this << std::endl;
}

void CUiVerticalBox::NextPage()
{
	CurrentPage = std::min(MaxPage - 1, CurrentPage + 1);
}

void CUiVerticalBox::PreviousPage()
{
	CurrentPage = std::max(0, CurrentPage - 1);
}

bool CUiVerticalBox::CanNextPage() const
{
	return CurrentPage + 1 < MaxPage;
}

bool CUiVerticalBox::CanPreviousPage() const
{
	return CurrentPage > 0;
}

void CUiVerticalBox::Draw(SUiDrawVisitor& DrawVisitor)
{
	const SVector2i OriginalAllowedSize = DrawVisitor.AllowedSize;
	const SVector2i OriginalLocation = DrawVisitor.AtLocation;
	const int OldStartHeight = DrawVisitor.StartHeight;
	const int OldMaxAllowedHeight = DrawVisitor.MaxAllowedHeight;

	SVector2i PageSize = OriginalAllowedSize;
	if (MaxPage > 1)
	{
		PageSize.Y -= 25;
	}

	//if (MaxPage > 1)
	//{
	//	DrawRect(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, OriginalAllowdSize.X, OriginalAllowdSize.Y, 128);
	//}

	int CurrentHeight = 0;
	int LocalIndexPage = 0;
	CUiWidget* LastWidgetThatPossiblyNeedToBeDrawn = nullptr;
	bool bJustDrawnLastWidget = false;
	for (int Index = 0; Index < (*GetChildren()).size(); ++Index)
	{
		CUiWidget* Widget = (*GetChildren())[Index].Get();

		const int LocalIndexPageMinHeight = LocalIndexPage * PageSize.Y;
		const int LocalIndexPageMaxHeight = (LocalIndexPage + 1) * PageSize.Y;

		if (CurrentHeight >= LocalIndexPageMaxHeight)
		{
			if (!bSupportMultiplePages)
				break;

			LocalIndexPage += 1;
			if (LocalIndexPage > CurrentPage)
				break;

			--Index;
			continue;
		}

		bool bNeedToDraw = LocalIndexPage == CurrentPage;
		if (LastWidgetThatPossiblyNeedToBeDrawn && CurrentHeight - (LastWidgetThatPossiblyNeedToBeDrawn->DesiredSize.Y + LastWidgetThatPossiblyNeedToBeDrawn->GetPaddingAlongY()) < LocalIndexPageMinHeight && CurrentHeight >= LocalIndexPageMinHeight)
		{
			bNeedToDraw = true;
			//DrawLine(OriginalLocation.X, OriginalLocation.Y + CurrentHeight - LocalIndexPageMinHeight + Widget->Padding.TopLeft.Y, OriginalLocation.X + PageSize.X, OriginalLocation.Y + CurrentHeight - LocalIndexPageMinHeight, 0);
			CurrentHeight = CurrentHeight - LastWidgetThatPossiblyNeedToBeDrawn->DesiredSize.Y - LastWidgetThatPossiblyNeedToBeDrawn->GetPadding().BottomRight.Y - LastWidgetThatPossiblyNeedToBeDrawn->GetPadding().TopLeft.Y;
			//std::cout << DebugName << " Need to draw last widget ! " << LastWidgetThatPossiblyNeedToBeDrawn->DebugName << " at " << CurrentHeight << std::endl;
			//DrawLine(OriginalLocation.X, OriginalLocation.Y + CurrentHeight - LocalIndexPageMinHeight, OriginalLocation.X + PageSize.X, OriginalLocation.Y + CurrentHeight - LocalIndexPageMinHeight, 0);
			Widget = LastWidgetThatPossiblyNeedToBeDrawn;
			--Index;
			bJustDrawnLastWidget = true;
		}

		LastWidgetThatPossiblyNeedToBeDrawn = nullptr;
		if (bNeedToDraw)
		{
			DrawVisitor.AllowedSize = PageSize;

			DrawVisitor.AtLocation.X = OriginalLocation.X + Widget->GetPadding().TopLeft.X;
			DrawVisitor.AllowedSize.X -= Widget->GetPaddingAlongX();
			DrawVisitor.AtLocation.X += Widget->PivotPointRatio.X * float(DrawVisitor.AllowedSize.X - Widget->DesiredSize.X);

			DrawVisitor.AtLocation.Y = OriginalLocation.Y + CurrentHeight - LocalIndexPageMinHeight + Widget->GetPadding().TopLeft.Y;
			DrawVisitor.AllowedSize.Y = PageSize.Y - (CurrentHeight - LocalIndexPageMinHeight) - Widget->GetPaddingAlongY();

			DrawVisitor.MaxAllowedHeight = LocalIndexPageMaxHeight - CurrentHeight;
			DrawVisitor.StartHeight = std::max(0, (LocalIndexPageMinHeight - CurrentHeight));
			//std::cout << DebugName << " Draw for " << Widget->DebugName << " " << CurrentHeight << " -> " << CurrentHeight + Widget->DesiredSize.Y << " (MawAllowedHeight " << DrawVisitor.MaxAllowedHeight << ") at " << DrawVisitor.AtLocation.Y << std::endl;
			Widget->Draw(DrawVisitor);
			DrawVisitor.MaxAllowedHeight = OldMaxAllowedHeight;
			DrawVisitor.StartHeight = OldStartHeight;
		}

		LastWidgetThatPossiblyNeedToBeDrawn = bJustDrawnLastWidget || LocalIndexPage != CurrentPage - 1 ? nullptr : Widget;
		bJustDrawnLastWidget = false;
		//else
		//{
		//	std::cout << DebugName << "No draw => " << CurrentHeight << " " << Widget->DesiredSize.Y << " " << MinHeight << " " << MaxHeight << std::endl;
		//}

		//std::cout << DebugName << " CurrentHeight " << CurrentHeight << " -> " << CurrentHeight + std::min(DrawVisitor.MaxAllowedHeight, Widget->DesiredSize.Y) << " at " << DrawVisitor.AtLocation.Y 
		//	<< " LocalPage " << LocalIndexPage << " " << LocalIndexPageMinHeight << " => " << LocalIndexPageMaxHeight << std::endl;
		CurrentHeight += Widget->DesiredSize.Y + Widget->GetPaddingAlongY();
	}

	// Page marker
	if (MaxPage > 1)
	{
		CUiText PageMarkerText;
		char Buffer[] = "123456789\0";
		sprintf(Buffer, "%d/%d", CurrentPage + 1, MaxPage);

		PageMarkerText.Text = Buffer;
		PageMarkerText.Font = CApp::Get()->AppSettings.DefaultFont;
		PageMarkerText.CalcDesiredSize(DesiredSize);

		DrawVisitor.AtLocation.X = OriginalLocation.X + PageSize.X - PageMarkerText.DesiredSize.X;
		DrawVisitor.AtLocation.Y = OriginalLocation.Y + PageSize.Y;
		DrawVisitor.StartHeight = 0;
		DrawVisitor.MaxAllowedHeight = ScreenHeight();
		PageMarkerText.Draw(DrawVisitor);
	}

	DrawVisitor.AtLocation = OriginalLocation;
	DrawVisitor.AllowedSize = OriginalAllowedSize;
	DrawVisitor.StartHeight = OldStartHeight;
	DrawVisitor.MaxAllowedHeight = OldMaxAllowedHeight;
}