#include "PlexAES.h"
#include "log.h"
#include "File.h"
#include "Base64.h"

#include <boost/foreach.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> CPlexAES::chunkData(const std::string& data)
{
  std::vector<std::string> chunks;

  for (int i = 0, len = data.length(); i < len; i += AES_BLOCK_SIZE)
  {
    std::string chunk = data.substr(i, AES_BLOCK_SIZE);
    chunks.push_back(chunk);
  }

  return chunks;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string CPlexAES::encrypt(const std::string &data)
{
  if (data.empty())
    return "";

  // aes_encrypt expects a full block size for input so ensure its available
  // by using a temporary buffed "block".
  unsigned char block[AES_BLOCK_SIZE], buffer[AES_BLOCK_SIZE];
  std::string outData;
  BOOST_FOREACH(const std::string& chunk, chunkData(data))
  {
    strncpy((char *)&block[0], chunk.c_str(), AES_BLOCK_SIZE);
    if (aes_encrypt(block, buffer, &m_encryptCtx) == EXIT_FAILURE)
    {
      CLog::Log(LOGWARNING, "CPlexAES::encrypt failed to encrypt data...");
      return "";
    }

    outData.append((const char*)buffer, AES_BLOCK_SIZE);
  }

  return outData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string CPlexAES::decrypt(const std::string &data)
{
  if (data.empty())
    return "";

  std::string outData;
  unsigned char buffer[AES_BLOCK_SIZE];
  BOOST_FOREACH(const std::string& chunk, chunkData(data))
  {
    if (aes_decrypt((const unsigned char*)chunk.c_str(), buffer, &m_decryptCtx) == EXIT_FAILURE)
    {
      CLog::Log(LOGWARNING, "CPlexAES::decrypt failed to decrypt data...");
      return "";
    }
    outData.append((const char*)buffer, strnlen((const char*)buffer, AES_BLOCK_SIZE));
  }

  return outData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string CPlexAES::decryptFile(const std::string &url)
{
  XFILE::CFile file;

  if (file.Open(url))
  {
    char buffer[4096];
    std::string outData;
    while (int r = file.Read(buffer, 4096))
    {
      outData.append(buffer, r);
      if (r < 4096)
        break;
    }
    file.Close();

    return decrypt(Base64::Decode(outData));
  }

  return "";
}
