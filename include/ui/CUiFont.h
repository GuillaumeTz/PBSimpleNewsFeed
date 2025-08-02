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

#ifndef CUIFONT_H
#define CUIFONT_H

#include <string>
#include <map>

#include "inkview.h"

class CUiFont
{
public:
	ifont* Font;

	CUiFont() : Font(NULL) { }
	CUiFont(const CUiFont& rhs);
	void operator=(CUiFont rhs);
	CUiFont(const std::string& InFontName, int InSize, bool bInAntiAliased = true);
	~CUiFont();

	friend void swap(CUiFont& first, CUiFont& second)
	{
		std::swap(first.Font, second.Font);
	}

	bool IsValid() const { return Font != NULL; }

private:
	static std::map<ifont*, int> FontRefCountMap;
	
};

#endif
