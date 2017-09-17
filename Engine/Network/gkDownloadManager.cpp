/*
 * gkDownloadManager.cpp
 *
 *  Created on: Mar 31, 2013
 *      Author: ttrocha
 */

#include "gkDownloadManager.h"

extern "C" {
	#include <curl/curl.h>
}

#include "gkValue.h"
#include "gkLogger.h"
#include "gkPath.h"

UT_IMPLEMENT_SINGLETON(gkNetClientManager)

gkNetClientManager::gkNetClientManager()
	: gkResourceManager("DownloadManager", "Download"),
	  creationType(CREATION_HTTPSESSION), creationOption_sslCheckPeerCA(false)
	, creationOption_sslCheckPeerHostname(false), m_helperThread("nethelper"), creationOption_blocking(false)
{
	 curl_global_init(CURL_GLOBAL_ALL);
}

gkNetClientManager::~gkNetClientManager() {
	 curl_global_cleanup();

	 destroyAll();
}



static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;

  WriteCallbackData* data = (WriteCallbackData*)userp;
  utStream* stream = data->stream;
  stream->write(contents,realsize);
  stream->flush();

  return realsize;
}

gkHttpSession* gkNetClientManager::createHttpSession(const gkResourceName& name, bool blocking,bool sslCheckPeerCA,bool sslCheckHostName) {
	creationType = CREATION_HTTPSESSION;
	creationOption_sslCheckPeerCA=sslCheckPeerCA;
	creationOption_sslCheckPeerHostname=sslCheckHostName;
	creationOption_blocking = blocking;
	return (gkHttpSession*)create(name);
}

gkResource* gkNetClientManager::createImpl(const gkResourceName& name, const gkResourceHandle& handle)
{
	if (creationType==CREATION_HTTPSESSION) {
		return new gkHttpSession(this,name,handle,creationOption_blocking,creationOption_sslCheckPeerCA,creationOption_sslCheckPeerHostname);
	}
	return 0;
}

void gkHttpSession::downloadToFilesystem(const gkString& url,  const gkString& toPath, const gkString& postData,bool overwrite, bool sslCheckPeerCA,bool sslCheckHostName)
{
	  gkNetClientManager* mgr = gkNetClientManager::getSingletonPtr();

	  if (!mgr){
		  mgr = new gkNetClientManager;
	  }

	  gkHttpSession* session = (gkHttpSession*)mgr->getByName("__gkdownloader__");
	  if (!session)
		  session = mgr->createHttpSession("__gkdownloader__",true,sslCheckPeerCA,sslCheckHostName);
	  else {
		  // check if the same ssl-stuff is set otherwise kill the session
		  // TODO: The session-key will get lost!? Something to do against?
		  if (session->usingSSLPeerCACheck()!=sslCheckPeerCA ||
			  session->usingSSLPeerCACheck()!=sslCheckHostName) {
			  mgr->destroy(session);
			  session = mgr->createHttpSession("__gkdownloader__",true,sslCheckPeerCA,sslCheckHostName);
		  }

	  }
#ifdef OGREKIT_BUILD_ANDROID
	  gkPath path(gkPath::getSDCardPath()+"/"+toPath);
	  if (!overwrite && path.exists())
	  {
		  gkLogger::write("path:"+path.getAbsPath()+" already exists! don't overwrite!",true);
		  return;
	  }
	  utFileStream* fileStream = new utFileStream();
	  fileStream->open((gkPath::getSDCardPath()+"/"+toPath).c_str(),utStream::SM_WRITE);
//	  fileStream->open(("/sdcard/gamekit/"+toPath).c_str(),utStream::SM_WRITE);
#elif defined OGREKIT_BUILD_BLACKBERRY
	  gkPath path("./data/"+toPath);
	  if (!overwrite && path.exists())
	  {
		  gkLogger::write("path:"+path.getAbsPath()+" already exists! don't overwrite!",true);
		  return;
	  }
	  utFileStream* fileStream = new utFileStream();
	  fileStream->open(("./data/"+toPath).c_str(),utStream::SM_WRITE);
#else
	  gkPath path(toPath);
	  if (!overwrite && path.exists())
	  {
		  gkLogger::write("path:"+path.getAbsPath()+" already exists! don't overwrite!",true);
		  return;
	  }
	  utFileStream* fileStream = new utFileStream();
	  fileStream->open(toPath.c_str(),utStream::SM_WRITE);
#endif

	  utStream* result = session->getURL(url,postData,fileStream);
	  if (!result)
	  {
		  gkLogger::write("proble writing from "+url+" to: "+toPath);
	  }
	  fileStream->close();
	  delete fileStream;

}


gkString gkHttpSession::downloadToString(bool blocking,const gkString& url, const gkString& postData, bool sslCheckPeerCA,bool sslCheckHostName)
{
	  gkString downloaderSessionName = blocking?"__gkdownloader__":"__nb_gkdownloader__";

	  gkNetClientManager* mgr = gkNetClientManager::getSingletonPtr();

	  if (!mgr){
		  mgr = new gkNetClientManager;
	  }

	  gkHttpSession* session = (gkHttpSession*)mgr->getByName(downloaderSessionName);
	  if (!session)
		  session = mgr->createHttpSession(downloaderSessionName,blocking,sslCheckPeerCA,sslCheckHostName);
	  else {
		  // check if the same ssl-stuff is set otherwise kill the session
		  // TODO: The session-key will get lost!? Something to do against?
		  if (session->usingSSLPeerCACheck()!=sslCheckPeerCA ||
			  session->usingSSLPeerCACheck()!=sslCheckHostName) {
			  mgr->destroy(session);
			  session = mgr->createHttpSession(downloaderSessionName,blocking,sslCheckPeerCA,sslCheckHostName);
		  }
	  }

	  if (blocking)
	  {
		  utMemoryStream result;
		  session->getURL(url,postData,(utStream*)&result);

		  // add string delimiter
		  char buf[1] = {0};
		  result.write(buf,1);
		  return gkString((char*)result.ptr());
	  }
	  else
	  {
		  session->getURL(url,postData);
		  return "nb_request";
	  }
}


gkHttpSession::gkHttpSession(gkResourceManager* creator, const gkResourceName& name, const gkResourceHandle& handle,bool blocking,bool sslCheckPeerCA,bool sslCheckHostName)
	:    gkResource(creator, name, handle)
		, multi_handle(0), m_still_running(0),m_handlesInUse(0), m_SSLCheckPeerCA(sslCheckPeerCA)
		, m_SSLCheckPeerHostName(sslCheckHostName),m_blockingSession(blocking),m_multiCallback(0)
{
		// clean the handle-slots
		for (int i=0;i<max_multihandles;i++)
		{
			curl_handles[i] = 0;
		}

		if (blocking)
		{
			m_handlesInUse = 1;
			curl_handles[0] = curl_easy_init();
		}
		else
		{
			multi_handle = curl_multi_init();
		}
}



gkHttpSession::~gkHttpSession() {
	close();
}

CURL*	gkHttpSession::nextFreeSlot()
{
	if (m_blockingSession)
	{
		return curl_handles[0];
	}
	else
	{
		if (m_handlesInUse<max_multihandles)
		{
			CURL* handle = curl_handles[m_handlesInUse++];
			if (!handle)
				handle = curl_easy_init();
			return handle;
		}
		else
		{
			return 0;
		}
	}
}

void gkHttpSession::performMultisession()
{
	if (m_blockingSession)
	{
		gkLogger::write("tried to perform multisession on a not multisession!",true);
		return;
	}

	if (m_still_running)
	{
		gkLogger::write("Can't perform multisession if session still in use!",true);
		return;
	}

	if (m_handlesInUse>0)
	{
		curl_multi_perform(multi_handle,&m_still_running);
	}
}

void gkHttpSession::setMultisessionCallback(gkHttpSessionCallback* callback)
{
	if (m_multiCallback)
	{
		delete m_multiCallback;
		m_multiCallback = 0;
	}
	m_multiCallback = callback;
}

void gkHttpSession::pollMultisession()
{
	if (m_blockingSession)
	{
			gkLogger::write("tried to perform multisession on a not multisession!",true);
			return;
	}

	if (!m_still_running)
	{
		gkLogger::write("tried to perform multisession, but there are not handles left!",true);
		return;
	}

	// took all of this from here: http://curl.haxx.se/libcurl/c/multi-app.html
	struct timeval timeout;
	int rc; /* select() return code */

	fd_set fdread;
	fd_set fdwrite;
	fd_set fdexcep;
	int maxfd = -1;

	long curl_timeo = -1;

	FD_ZERO(&fdread);
	FD_ZERO(&fdwrite);
	FD_ZERO(&fdexcep);

	/* set a suitable timeout to play around with */
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	curl_multi_timeout(multi_handle, &curl_timeo);
	if(curl_timeo >= 0) {
	  timeout.tv_sec = curl_timeo / 1000;
	  if(timeout.tv_sec > 1)
		timeout.tv_sec = 1;
	  else
		timeout.tv_usec = (curl_timeo % 1000) * 1000;
	}

	/* get file descriptors from the transfers */
	rc = curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

	/* In a real-world program you OF COURSE check the return code of the
	   function calls.  On success, the value of maxfd is guaranteed to be
	   greater or equal than -1.  We call select(maxfd + 1, ...), specially in
	   case of (maxfd == -1), we call select(0, ...), which is basically equal
	   to sleep. */

	rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);

	switch(rc) {
	case -1:
	  /* select error */
	  break;
	case 0: /* timeout */
	default: /* action */
	  curl_multi_perform(multi_handle, &m_still_running);
	  break;
	}


	  CURLMsg *msg; /* for picking up messages with the transfer status */
	  int msgs_left; /* how many messages are left */
	  while ((msg = curl_multi_info_read(multi_handle, &msgs_left)))
	  {
	     if (msg->msg == CURLMSG_DONE)
	     {
		       WriteCallbackData* dt = *m_handleMapping.get(utPointerHashKey(msg->easy_handle));

		       if (m_multiCallback)
		       {
		    	   if (msg->data.result == 0)
		    	   {
		    		   m_multiCallback->onDone(gkHttpSessionCallback::success,dt->stream,dt->url);
		    	   }
		       }

		       delete dt;
		       curl_easy_cleanup(msg->easy_handle);

		       m_handlesInUse--;

	       // TODO: call the callback here
//	       /* Find out which handle this message is about */
//	       for (idx=0; idx<HANDLECOUNT; idx++) {
//	         found = (msg->easy_handle == handles[idx]);
//	         if(found)
//	           break;
//	       }
//
//	       switch (idx) {
//	       case HTTP_HANDLE:
//	         printf("HTTP transfer completed with status %d\n", msg->data.result);
//	         break;
//	       case FTP_HANDLE:
//	         printf("FTP transfer completed with status %d\n", msg->data.result);
//	         break;
//	       }
	     }
	   }
}

utStream* gkHttpSession::getURL(const gkString& url, const gkString& postData, utStream* stream){
	  CURLcode res;

      bool streamCreatedByMethod(0);

      CURL* curl_handle = nextFreeSlot();

      curl_easy_reset(curl_handle);

      gkLogger::write("Retrieve URL:"+url,true);

	  if (stream)
		  stream->clear();
	  else {
		  streamCreatedByMethod = true;
		  stream = new utMemoryStream(utStream::SM_WRITE);
	  }

	  WriteCallbackData* callbackData = new WriteCallbackData;
	  callbackData->handleNr = m_handlesInUse-1;
	  callbackData->stream = stream;
	  callbackData->url = url;

	  m_handleMapping.insert(utPointerHashKey(curl_handle),callbackData);

	  /* specify URL to get */
	  curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());

	  /* send all data to this function  */
	  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	  /* we pass our 'chunk' struct to the callback function */
	  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, callbackData);

	  curl_easy_setopt(curl_handle,CURLOPT_COOKIEFILE, "cookie.txt");

	  if (!postData.empty()) {
		  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, postData.c_str());
	  }

// https
	  if (!m_SSLCheckPeerCA)
		  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	  if (!m_SSLCheckPeerHostName)
		  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
// ---
	  /* some servers don't like requests that are made without a user-agent
	     field, so we provide one */
	  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "tomaga-agent/1.0");


	  /* get it! */
	  if (m_blockingSession)
	  {
		  res = curl_easy_perform(curl_handle);

		  /* check for errors */
		  if(res != CURLE_OK) {
			  gkLogger::write("Problem with curl!"+gkString(curl_easy_strerror(res)),true);
		    fprintf(stderr, "curl_easy_perform() failed: %s\n",
		            curl_easy_strerror(res));
		    if (streamCreatedByMethod)
		    	delete stream;

		    return 0;
		  }
		  else {
			 delete callbackData;
			 return stream;
		  }
	  }
	  else
	  {
		  curl_multi_add_handle(multi_handle,curl_handle);
		  // no time to return streams now, since we
		  return 0;
	  }


}

void gkHttpSession::close() {
	for (int i=0;i<m_handlesInUse;i++)
	{
		if (curl_handles[i])
		{
			curl_easy_cleanup(curl_handles[i]);
			curl_handles[i] = 0;
		}
	}

	if (m_blockingSession)
	{
		curl_multi_cleanup(multi_handle);
	}
}

bool gkNetworkUtils::isNetworkAvailable(const gkString& url,int timeout)
{
	CURLcode res;

	CURL* curl;
	curl = curl_easy_init();

	if(curl) {
	    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
	  while ((res = curl_easy_perform(curl)) != CURLE_OK)
	  {
	    switch (res)
	    {
	        case CURLE_COULDNT_CONNECT:
	        case CURLE_COULDNT_RESOLVE_HOST:
	        case CURLE_COULDNT_RESOLVE_PROXY:
	        	curl_easy_cleanup(curl);
	        	return false;
	            break;
	        default:
	            gkLogger::write("isNetworkAvailable: Request failed",true);
	            curl_easy_cleanup(curl);
	            return false;
	    }
	  }

	    /* always cleanup */
    curl_easy_cleanup(curl);
    return true;
	} else {
		return false;
	}
}
