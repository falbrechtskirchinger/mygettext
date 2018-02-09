//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include "mygettextDefines.h" // IWYU pragma: keep
#include "LocaleInfo.h"
#include <boost/config.hpp>
#include <cstdlib>
#include <cstring>
#if defined(BOOST_WINDOWS) || defined(__CYGWIN__)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#define BOOST_LOCALE_USE_WIN32_API
#endif
namespace conv { namespace impl {
    std::string normalize_encoding(char const* ccharset)
    {
        std::string charset;
        charset.reserve(std::strlen(ccharset));
        while(*ccharset != 0)
        {
            char c = *ccharset++;
            if('0' <= c && c <= '9')
                charset += c;
            else if('a' <= c && c <= 'z')
                charset += c;
            else if('A' <= c && c <= 'Z')
                charset += char(c - 'A' + 'a');
        }
        return charset;
    }
}} // namespace conv::impl

std::string get_system_locale(bool use_utf8)
{
    char const* lang = 0;
    if(!lang || !*lang)
        lang = getenv("LC_CTYPE");
    if(!lang || !*lang)
        lang = getenv("LC_ALL");
    if(!lang || !*lang)
        lang = getenv("LANG");
#ifndef BOOST_LOCALE_USE_WIN32_API
    (void)use_utf8; // not relevant for non-windows
    if(!lang || !*lang)
        lang = "C";
    return lang;
#else
    if(lang && *lang)
    {
        return lang;
    }
    char buf[10];
    if(GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, buf, sizeof(buf)) == 0)
        return "C";
    std::string lc_name = buf;
    if(GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, buf, sizeof(buf)) != 0)
    {
        lc_name += "_";
        lc_name += buf;
    }
    if(!use_utf8)
    {
        if(GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, buf, sizeof(buf)) != 0)
        {
            if(atoi(buf) == 0)
                lc_name += ".UTF-8";
            else
            {
                lc_name += ".windows-";
                lc_name += buf;
            }
        } else
        {
            lc_name += "UTF-8";
        }
    } else
    {
        lc_name += ".UTF-8";
    }
    return lc_name;

#endif
}

LocaleInfo::LocaleInfo() : name("C"), language("C"), encoding("us-ascii"), utf8(false) {}

void LocaleInfo::parse(std::string locale_name)
{
    name = locale_name;
    language = "C";
    country.clear();
    variant.clear();
    encoding = "us-ascii";
    utf8 = false;
    if(locale_name.empty())
        locale_name = get_system_locale(true);
    // Default to British English
    if(locale_name == "C")
        locale_name = "en_GB";
    parse_from_lang(locale_name);
}

void LocaleInfo::parse_from_lang(std::string const& locale_name)
{
    size_t end = locale_name.find_first_of("-_@.");
    std::string tmp = locale_name.substr(0, end);
    if(tmp.empty())
        return;
    for(unsigned i = 0; i < tmp.size(); i++)
    {
        if('A' <= tmp[i] && tmp[i] <= 'Z')
            tmp[i] = tmp[i] - 'A' + 'a';
        else if(tmp[i] < 'a' && 'z' < tmp[i])
            return;
    }
    language = tmp;
    if(end >= locale_name.size())
        return;

    if(locale_name[end] == '-' || locale_name[end] == '_')
    {
        parse_from_country(locale_name.substr(end + 1));
    } else if(locale_name[end] == '.')
    {
        parse_from_encoding(locale_name.substr(end + 1));
    } else if(locale_name[end] == '@')
    {
        parse_from_variant(locale_name.substr(end + 1));
    }
}

void LocaleInfo::parse_from_country(std::string const& locale_name)
{
    size_t end = locale_name.find_first_of("@.");
    std::string tmp = locale_name.substr(0, end);
    if(tmp.empty())
        return;
    for(unsigned i = 0; i < tmp.size(); i++)
    {
        if('a' <= tmp[i] && tmp[i] <= 'a')
            tmp[i] = tmp[i] - 'a' + 'A';
        else if(tmp[i] < 'A' && 'Z' < tmp[i])
            return;
    }

    country = tmp;

    if(end >= locale_name.size())
        return;
    else if(locale_name[end] == '.')
    {
        parse_from_encoding(locale_name.substr(end + 1));
    } else if(locale_name[end] == '@')
    {
        parse_from_variant(locale_name.substr(end + 1));
    }
}

void LocaleInfo::parse_from_encoding(std::string const& locale_name)
{
    size_t end = locale_name.find_first_of("@");
    std::string tmp = locale_name.substr(0, end);
    if(tmp.empty())
        return;
    for(unsigned i = 0; i < tmp.size(); i++)
    {
        if('A' <= tmp[i] && tmp[i] <= 'Z')
            tmp[i] = tmp[i] - 'A' + 'a';
    }
    encoding = tmp;

    utf8 = conv::impl::normalize_encoding(encoding.c_str()) == "utf8";

    if(end >= locale_name.size())
        return;

    if(locale_name[end] == '@')
    {
        parse_from_variant(locale_name.substr(end + 1));
    }
}

void LocaleInfo::parse_from_variant(std::string const& locale_name)
{
    variant = locale_name;
    for(unsigned i = 0; i < variant.size(); i++)
    {
        if('A' <= variant[i] && variant[i] <= 'Z')
            variant[i] = variant[i] - 'A' + 'a';
    }
}