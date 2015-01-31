//
//  MyPlexPinInfo.cpp
//  Plex
//
//  Created by Tobias Hieta <tobias@plexapp.com> on 2013-04-12.
//  Copyright 2013 Plex Inc. All rights reserved.
//

#include "MyPlexPinInfo.h"
#include "XBMCTinyXML.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "utils/log.h"

bool CMyPlexPinInfo::SetFromXmlElement(TiXmlElement *root)
{
  if (root->ValueStr() != "pin")
  {
    CLog::Log(LOGERROR, "CMyPlexPinInfo::SetFromXmlElement return didn't contain a <pin> element.");
    return false;
  }

  for (TiXmlElement *elem = root->FirstChildElement(); elem; elem = elem->NextSiblingElement())
  {
    if (elem->GetText() == NULL)
      continue;

    if (elem->ValueStr() == "client-identifier")
      clientid = elem->GetText();
    else if (elem->ValueStr() == "auth_token")
      authToken = elem->GetText();
    else if (elem->ValueStr() == "code")
      code = elem->GetText();
    else if (elem->ValueStr() == "expires-at")
    {
      std::string date = elem->GetText();
      boost::algorithm::replace_first(date, "T", " ");
      boost::algorithm::replace_first(date, "Z", "");

      expiresAt.SetFromDBDateTime(date);
      if (!expiresAt.IsValid())
      {
        CLog::Log(LOGERROR, "CMyPlexPinInfo::SetFromXmlElement failed to parse datetime '%s'", date.c_str());
      }
    }
    else if (elem->ValueStr() == "id")
    {
      try
      {
        id = boost::lexical_cast<int>(elem->GetText());
      }
      catch(boost::bad_lexical_cast)
      {
        CLog::Log(LOGERROR, "CMyPlexPinInfo::SetFromXmlElement failed to parse code element %s", elem->GetText());
        id = -1;
      }
    }
  }
  return true;
}
