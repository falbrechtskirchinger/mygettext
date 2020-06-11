// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#define MYGETTEXT_SOURCE

#include "mygettext/mygettext.h"
#include "mygettext/gettext.h"

namespace mygettext {
static GetText __gettext;

const char* setlocale(int /*category*/, const char* locale)
{
    return __gettext.setLocale(locale);
}

const char* gettext(const char* msgid)
{
    return __gettext.get(msgid);
}

const char* bindtextdomain(const char* domainname, const char* dirname)
{
    return __gettext.setCatalogDir(domainname, dirname);
}

const char* textdomain(const char* domainname)
{
    return __gettext.setCatalog(domainname);
}

const char* bind_textdomain_codeset(const char* /*domainname*/, const char* codeset)
{
    return __gettext.setCodepage(codeset);
}

} // namespace mygettext
