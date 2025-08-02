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

#ifndef CHTMLPARSING_H
#define CHTMLPARSING_H

#include <string>

namespace EHtmlTag
{
	enum Type
	{
		Unknown,
		a,
		br,
		em,
		img,

		p,
		span,
		div,

		h1,
		h2,
		h3,
		h4,
		h5
	};
}


class CHtmlParsing
{
public:

	static void SkipTo(const std::string& String, int& Index, char Character);
	static void SkipTo(const std::string& String, int& Index, const std::string& SkipToString);
	static bool AreNextCharactersEquals(const std::string& String, int Index, const std::string& EqualTo, int* OutIndex = NULL);
	static EHtmlTag::Type GetTagType(const std::string& String, int Index);

	static void FixInvalidUTF8(std::string& str);
	static void DecoteHtmlEntities(std::string& str);

	static void CleanXmlValue(std::string& Str);

};

#endif
