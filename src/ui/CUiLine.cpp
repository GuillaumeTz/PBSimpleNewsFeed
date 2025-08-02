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

#include "ui/CUiLine.h"

#include "inkview.h"

CUiLine::CUiLine() : CUiWidget()
{
	bVertical = false;
}

void CUiLine::Draw(SUiDrawVisitor& DrawVisitor)
{
	if (bVertical)
	{
		DrawLine(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y + DrawVisitor.ParentSize.Y, 0);
	}
	else
	{
		DrawLine(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, DrawVisitor.AtLocation.X + DrawVisitor.ParentSize.X, DrawVisitor.AtLocation.Y, 0);
	}
}

void CUiLine::DeleteItself()
{
	CUiLineAllocator::Allocator.Delete(this);
}
