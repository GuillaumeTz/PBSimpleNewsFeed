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

#ifndef CUIVIEWPORT_H
#define CUIVIEWPORT_H

#include "ui/CUiWidget.h"

class CUiWidget;

class CUiViewport
{
public:
	SVector2i ViewportSize;
	SUiDrawVisitor DrawVisitor;

public:
	CUiViewport();

	void AddOverlayWidget(CUiWidget* InWidgetOverlay);
	void PopOverlayWidget(CUiWidget* InWidgetOverlay = nullptr);
	void RemoveAllOverlayWidgets();
	void Draw(bool bUpdate = true);

	CUiWidget* GetWidgetUnder(const SVector2i Coord) const;
	void PartialUpdateForWidget(CUiWidget* Widget);

private:
	TSharedPtr<class CUiOverlay> RootWidget;
};

#endif
