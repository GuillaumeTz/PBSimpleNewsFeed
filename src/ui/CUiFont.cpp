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

#include "ui/CUiFont.h"

std::map<ifont*, int> CUiFont::FontRefCountMap;

CUiFont::CUiFont(const std::string& InFontName, int InSize, bool bInAntiAliased)
{
	Font = OpenFont(InFontName.c_str(), InSize, bInAntiAliased ? 1 : 0);
	if (FontRefCountMap.find(Font) == FontRefCountMap.end())
	{
		FontRefCountMap[Font] = 1;
	}
	else
	{
		FontRefCountMap[Font] += 1;
	}
}

CUiFont::CUiFont(const CUiFont& rhs)
{
	Font = rhs.Font;
	if (Font)
	{
		FontRefCountMap[Font] += 1;
	}
}

void CUiFont::operator=(CUiFont rhs)
{
	swap(*this, rhs);
}

CUiFont::~CUiFont()
{
	if (Font)
	{
		FontRefCountMap[Font] -= 1;
		if (FontRefCountMap[Font] == 0)
		{
			CloseFont(Font);
		}
		Font = NULL;
	}
}
