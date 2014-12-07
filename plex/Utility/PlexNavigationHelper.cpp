#include "PlexNavigationHelper.h"
#include "FileItem.h"
#include "PlexUtils.h"
#include "Key.h"
#include "Variant.h"
#include "URL.h"
#include "GUI/GUIPlexMediaWindow.h"
#include "dialogs/GUIDialogBusy.h"
#include "DirectoryCache.h"
#include "threads/Event.h"
#include "JobManager.h"
#include "GUIKeyboardFactory.h"
#include "ApplicationMessenger.h"
#include "GUI/GUIDialogPlexPluginSettings.h"
#include "dialogs/GUIDialogOK.h"
#include "PlexBusyIndicator.h"
#include "PlexApplication.h"
#include "Application.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>


///////////////////////////////////////////////////////////////////////////////////////////////////
bool CPlexNavigationHelper::CacheUrl(const std::string& url, bool& cancel)
{
  cancel = g_plexApplication.busy.blockWaitingForJob(new CPlexDirectoryFetchJob(CURL(url)), this);
  return m_cacheSuccess;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CPlexNavigationHelper::navigateToNowPlaying()
{
  if (g_application.IsPlayingVideo())
    g_windowManager.ActivateWindow(WINDOW_FULLSCREEN_VIDEO);
  else if (g_application.IsPlayingAudio())
    g_windowManager.ActivateWindow(WINDOW_NOW_PLAYING);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CStdString CPlexNavigationHelper::navigateToItem(CFileItemPtr item, const CURL &parentUrl, int windowId, bool swap)
{
  CStdString empty;

  if (!item)
    return empty;

  if (!item->m_bIsFolder && (!PlexUtils::CurrentSkinHasPreplay() ||
      (item->GetPlexDirectoryType() == PLEX_DIR_TYPE_TRACK || item->GetPlexDirectoryType() == PLEX_DIR_TYPE_PHOTO)))
    return empty;

  CStdString originalUrl = item->GetPath();

  if (originalUrl.empty() && item->HasProperty("sectionPath"))
    originalUrl = item->GetProperty("sectionPath").asString();

  if (item->m_bIsFolder)
  {
    if (item->GetProperty("search").asBoolean())
    {
      originalUrl = ShowPluginSearch(item);
      return originalUrl;
    }
    else if (item->GetProperty("settings").asBoolean())
    {
      ShowPluginSettings(item);
      originalUrl = parentUrl.Get();
      return originalUrl;
    }
  }

  CStdString cacheUrl(originalUrl);

  if (item->m_bIsFolder && (windowId == WINDOW_SHARED_CONTENT || windowId == WINDOW_HOME))
  {
    CURL u = CGUIPlexMediaWindow::GetRealDirectoryUrl(originalUrl);
#ifdef USE_PAGING
    u.SetOption("X-Plex-Container-Start", "0");
    u.SetOption("X-Plex-Container-Size", boost::lexical_cast<std::string>(PLEX_DEFAULT_PAGE_SIZE));
#endif
    cacheUrl = u.Get();
  }

  bool didCancel;
  if (!CacheUrl(cacheUrl, didCancel))
  {
    if (!didCancel)
      CGUIDialogOK::ShowAndGetInput("Failed to load!", "The navigation item failed to load", "Check logs for more information.", "");
    return empty;
  }

  if (windowId == WINDOW_SHARED_CONTENT)
  {
    int window = WINDOW_VIDEO_NAV;
    if (item->GetPlexDirectoryType() == PLEX_DIR_TYPE_ARTIST)
      window = WINDOW_MUSIC_FILES;
    else if (item->GetPlexDirectoryType() == PLEX_DIR_TYPE_PHOTO ||
             item->GetPlexDirectoryType() == PLEX_DIR_TYPE_PHOTOALBUM)
      window = WINDOW_PICTURES;

    std::vector<CStdString> args;
    args.push_back(originalUrl);
    args.push_back("return");

    CApplicationMessenger::Get().ActivateWindow(window, args, swap);
    return empty;
  }

  if (!item->m_bIsFolder && PlexUtils::CurrentSkinHasPreplay() &&
      item->IsPlexMediaServer() &&
      (item->GetPlexDirectoryType() == PLEX_DIR_TYPE_MOVIE ||
       item->GetPlexDirectoryType() == PLEX_DIR_TYPE_EPISODE ||
       item->GetPlexDirectoryType() == PLEX_DIR_TYPE_CLIP ||
       item->GetPlexDirectoryType() == PLEX_DIR_TYPE_VIDEO))
  {
    std::vector<CStdString> args;
    args.push_back(originalUrl);
    args.push_back("return");
    args.push_back(parentUrl.Get());

    CApplicationMessenger::Get().ActivateWindow(WINDOW_PLEX_PREPLAY_VIDEO, args, swap);
    return empty;
  }

  int window = WINDOW_VIDEO_NAV;

  EPlexDirectoryType type = item->GetPlexDirectoryType();
  if (type == PLEX_DIR_TYPE_ALBUM || type == PLEX_DIR_TYPE_ARTIST)
    window = WINDOW_MUSIC_FILES;
  else if (type == PLEX_DIR_TYPE_PHOTOALBUM || type == PLEX_DIR_TYPE_PHOTO)
    window = WINDOW_PICTURES;
  else if (type == PLEX_DIR_TYPE_DIRECTORY)
    window = windowId;
  else if (type == PLEX_DIR_TYPE_CHANNEL)
  {
    CStdString typeStr = item->GetProperty("type").asString();
    if (typeStr == "channel")
    {
      CURL u(item->GetPath());
      if (boost::starts_with(u.GetFileName(), "music"))
        window = WINDOW_MUSIC_FILES;
      else if (boost::starts_with(u.GetFileName(), "video"))
        window = WINDOW_VIDEO_NAV;
      else if (boost::starts_with(u.GetFileName(), "photos"))
        window = WINDOW_PICTURES;
    }
    else if (typeStr == "music")
      window = WINDOW_MUSIC_FILES;
    else if (typeStr == "photos")
      window = WINDOW_PICTURES;
    else
      window = WINDOW_VIDEO_NAV;
  }
  else if (type == PLEX_DIR_TYPE_PLAYLIST)
    window = WINDOW_PLEX_PLAY_QUEUE;
  
  if (windowId != window)
  {
    CLog::Log(LOGDEBUG, "CPlexNavigationHelper::navigateToItem navigating to %s (%s)", originalUrl.c_str(), item->GetLabel().c_str());
    std::vector<CStdString> args;
    args.push_back(originalUrl);
    CApplicationMessenger::Get().ActivateWindow(window, args, swap);
  }
  else
    return originalUrl;

  return empty;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CPlexNavigationHelper::OnJobComplete(unsigned int jobID, bool success, CJob *job)
{
  CPlexDirectoryFetchJob *fjob = static_cast<CPlexDirectoryFetchJob*>(job);
  if (fjob && success)
    g_directoryCache.SetDirectory(fjob->m_url.Get(), fjob->m_items, XFILE::DIR_CACHE_ALWAYS);

  m_cacheSuccess = fjob ? success : false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CStdString CPlexNavigationHelper::ShowPluginSearch(CFileItemPtr item)
{
  CStdString strSearchTerm = "";
  if (CGUIKeyboardFactory::ShowAndGetInput(strSearchTerm, item->GetProperty("prompt").asString(), false))
  {
    // Find the ? if there is one.
    CURL u(item->GetPath());
    u.SetOption("query", strSearchTerm);
    return u.Get();
  }
  return CStdString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CPlexNavigationHelper::ShowPluginSettings(CFileItemPtr item)
{
  CFileItemList fileItems;
  std::vector<CStdString> items;
  XFILE::CPlexDirectory plexDir;

  plexDir.GetDirectory(item->GetPath(), fileItems);
  CGUIDialogPlexPluginSettings::ShowAndGetInput(item->GetPath(), plexDir.GetData());
}
