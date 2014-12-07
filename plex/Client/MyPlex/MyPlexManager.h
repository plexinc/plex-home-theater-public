#pragma once

#include "GlobalsHandling.h"
#include "threads/Thread.h"
#include "Client/PlexServer.h"
#include "XBMCTinyXML.h"
#include "XBDateTime.h"

#include "MyPlexPinInfo.h"
#include "MyPlexUserInfo.h"

#include "FileItem.h"

class CMyPlexManager : public CThread
{
public:
  enum EMyPlexState
  {
    STATE_REFRESH,
    STATE_NOT_LOGGEDIN, // -> can progress to try_login or fetch_pin
    STATE_TRY_LOGIN, // -> can progress to loggedin or not_loggedin
    STATE_FETCH_PIN, // -> can progress to wait_pin or not_loggedin
    STATE_WAIT_PIN, // -> can progress to loggedin or not loggedin (after timeout)
    STATE_LOGGEDIN,
    STATE_EXIT
  };
  
  enum EMyPlexError
  {
    ERROR_NOERROR,
    ERROR_NETWORK,
    ERROR_WRONG_CREDS,
    ERROR_PARSE,
    ERROR_TMEOUT
  };
  
  CMyPlexManager();
  
  bool IsSignedIn() const { return m_state == STATE_LOGGEDIN; }
  bool IsPinProtected() const { return m_currentUserInfo.pin.empty() == false; }
  bool VerifyPin(const std::string& pin, int userId = -1);
  
  void StartPinLogin();
  void StopPinLogin();
  void Login(const CStdString& username, const CStdString& password);
  void SwitchHomeUser(int id, const std::string &pin = "");
  void Logout();
  void Refresh() { m_state = STATE_REFRESH; Poke(); }
  void Rescan() { m_state = STATE_LOGGEDIN; Poke(); }
  
  const CMyPlexUserInfo& GetCurrentUserInfo() const { return m_currentUserInfo; }
  const CMyPlexPinInfo& GetCurrentPinInfo() const { return m_currentPinInfo; }
  
  CStdString GetAuthToken() const;
  
  void Poke() { m_wakeEvent.Set(); }
  void Stop();
  
protected:
  virtual void Process();
  
private:
  std::string HashPin(const std::string& pin);
  int DoLogin();
  int DoFetchPin();
  int DoFetchWaitPin();
  int DoScanMyPlex();
  int DoRefreshUserInfo();
  int DoRemoveAllServers();
  void CacheUserInfo(TiXmlElement* userXml);
  
  void BroadcastState();
  TiXmlElement* GetXml(const CURL &url, bool POST=false);
  
  /* XML Parsing */
  bool ParseLogin(TiXmlElement* root);
  bool ParsePin(TiXmlElement *root);
  
  CCriticalSection m_stateLock;
  
  int m_secToSleep;
  CEvent m_wakeEvent;
  
  EMyPlexState m_state;
  EMyPlexError m_lastError;
  
  CStdString m_username;
  CStdString m_password;
  
  int m_homeId;
  std::string m_homePin;
  
  CPlexServerPtr m_myplex;
  
  CMyPlexUserInfo m_currentUserInfo;
  CMyPlexPinInfo m_currentPinInfo;
  
  CXBMCTinyXML m_doc;
  bool m_havePlexServers;
};
