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

#include "ui/CUiViewport.h"
#include "ui/CUiOverlay.h"

#include "inkview.h"

#include <iostream>

CUiViewport::CUiViewport()
{
	RootWidget = new CUiOverlay();
}

void CUiViewport::AddOverlayWidget(CUiWidget* InWidgetOverlay)
{
	RootWidget->AddChild(InWidgetOverlay);
}

void CUiViewport::PopOverlayWidget(CUiWidget* InWidgetOverlay)
{
	if (!InWidgetOverlay)
	{
		RootWidget->RemoveChildAt(RootWidget->GetChildren()->size() - 1);
	}
	else
	{
		for (int Index = 0; Index < (*RootWidget->GetChildren()).size(); ++Index)
		{
			if ((*RootWidget->GetChildren())[Index] == InWidgetOverlay)
			{
				RootWidget->RemoveChildAt(Index);
				break;
			}
		}
	}
}

void CUiViewport::RemoveAllOverlayWidgets()
{
	RootWidget->ClearChildren();
}

void CUiViewport::Draw(bool bUpdate)
{
	if (!RootWidget)
		return;

	const std::map<CUiWidget*, SRect> LastVisibleZones = DrawVisitor.VisibleZones;

	ClearScreen();
	DrawVisitor = SUiDrawVisitor();
	DrawVisitor.ParentSize.X = ScreenWidth();
	DrawVisitor.ParentSize.Y = ScreenHeight();
	DrawVisitor.ParentSize -= RootWidget->GetPaddingSize();
	DrawVisitor.AtLocation = RootWidget->Padding.TopLeft;

	//DrawRect(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, DrawVisitor.ParentSize.X, DrawVisitor.ParentSize.Y, 128);

	RootWidget->CalcDesiredSize(DrawVisitor.ParentSize);
	RootWidget->Draw(DrawVisitor);

	if (!bUpdate)
		return;

	SRect BigZone;
	for (std::map<CUiWidget*, SRect>::iterator It = DrawVisitor.VisibleZones.begin(); It != DrawVisitor.VisibleZones.end(); ++It)
	{
		const SRect& DirtyRect(It->second);
		BigZone.ExtendTo(DirtyRect);
		//std::cerr << "Dirty " << DirtyRect.TopLeft.X << " " << DirtyRect.TopLeft.Y << " " << DirtyRect.BottomRight.X - DirtyRect.TopLeft.X << " " << DirtyRect.BottomRight.Y - DirtyRect.TopLeft.Y << std::endl;
		//PartialUpdate(DirtyRect.TopLeft.X, DirtyRect.TopLeft.Y, DirtyRect.BottomRight.X - DirtyRect.TopLeft.X, DirtyRect.BottomRight.Y - DirtyRect.TopLeft.Y);
	}

	//need to update last visible zones not in actual (visible zone removed)
	for (std::map<CUiWidget*, SRect>::const_iterator It = LastVisibleZones.begin(); It != LastVisibleZones.end(); ++It)
	{
		std::map<CUiWidget*, SRect>::iterator ItFound = DrawVisitor.VisibleZones.find(It->first);
		if (ItFound == DrawVisitor.VisibleZones.end() || ItFound->second != It->second)
		{
			const SRect& DirtyRect(It->second);
			BigZone.ExtendTo(DirtyRect);
			//std::cerr << "Remove " << DirtyRect.TopLeft.X << " " << DirtyRect.TopLeft.Y << " " << DirtyRect.BottomRight.X - DirtyRect.TopLeft.X << " " << DirtyRect.BottomRight.Y - DirtyRect.TopLeft.Y << std::endl;
			//PartialUpdate(DirtyRect.TopLeft.X, DirtyRect.TopLeft.Y, DirtyRect.BottomRight.X - DirtyRect.TopLeft.X, DirtyRect.BottomRight.Y - DirtyRect.TopLeft.Y);
		}
	}

	if (!BigZone.bIsValid)
		return;

	BigZone.Clamp(SVector2i(0), SVector2i(ScreenWidth(), ScreenHeight()));

	PartialUpdate(BigZone.TopLeft.X, BigZone.TopLeft.Y, BigZone.BottomRight.X - BigZone.TopLeft.X, BigZone.BottomRight.Y - BigZone.TopLeft.Y);
	std::cerr << "PartialUpdate " << BigZone.TopLeft.X << " " << BigZone.TopLeft.Y << " " << BigZone.BottomRight.X - BigZone.TopLeft.X << " " << BigZone.BottomRight.Y - BigZone.TopLeft.Y << std::endl;
}

CUiWidget* CUiViewport::GetWidgetUnder(const SVector2i Coord) const
{
	//std::cerr << "GetWidgetUnder " << Coord.X << " " << Coord.Y << std::endl;
	std::vector<CUiWidget*> Candidates;
	for (auto It = DrawVisitor.InteractableZones.rbegin(); It != DrawVisitor.InteractableZones.rend(); ++It)
	{
		const SRect& Rect(It->second);
		if (Rect.Contains(Coord))
		{
			Candidates.push_back(It->first);
		}
	}

	if (Candidates.size() > 0)
		return Candidates[0];

	return NULL;
}

void CUiViewport::PartialUpdateForWidget(CUiWidget* Widget)
{
	std::map<CUiWidget*, SRect>::iterator It = DrawVisitor.VisibleZones.find(Widget);
	if (It != DrawVisitor.VisibleZones.end())
	{
		SRect UpdateRect = It->second;
		PartialUpdate(UpdateRect.TopLeft.X, UpdateRect.TopLeft.Y, UpdateRect.BottomRight.X - UpdateRect.TopLeft.X, UpdateRect.BottomRight.Y - UpdateRect.TopLeft.Y);
	}
}
