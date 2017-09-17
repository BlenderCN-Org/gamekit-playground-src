/*
 * gkDownloadManager.h
 *
 *  Created on: Mar 31, 2013
 *      Author: ttrocha
 */

#ifndef GKDOWNLOADMANAGER_H_
#define GKDOWNLOADMANAGER_H_

#include <curl/curl.h>

#include "utSingleton.h"
#include "gkResourceManager.h"
#include "gkString.h"
#include "gkResource.h"
#include "utStreams.h"
#include "gkCommon.h"
#include "gkResourceName.h"
#include "../Thread/gkActiveObject.h"

#define max_multihandles 10

struct WriteCallbackData
{
	utStream* stream;
	gkString url;
	int handleNr;
};

class gkHttpSessionCallback
{
public:
	enum DoneType {
		success, fail
	};

	gkHttpSessionCallback(){};
	virtual ~gkHttpSessionCallback(){};

	virtual void onDone(DoneType type,utStream* stream,const gkString& url) = 0;

};

class gkHttpSession : public gkResource
{
public:
	gkHttpSession(gkResourceManager* creator, const gkResourceName& name, const gkResourceHandle& handle, bool blocking=true,bool sslCheckPeerCA=false,bool sslCheckHostName=false);
	~gkHttpSession();


	bool isMultiSessionRunning() { return m_handlesInUse; }
	void performMultisession();
	void pollMultisession();
	void setMultisessionCallback(gkHttpSessionCallback* callback);
	void close();

	static void downloadToFilesystem(const gkString& url, const gkString& to, const gkString& postData="", bool overwrite=true, bool sslCheckPeerCA=false,bool sslCheckHostName=false);
	static gkString downloadToString(bool blocking,const gkString& url, const gkString& postData="",bool sslCheckPeerCA=false,bool sslCheckHostName=false);
	static void nbSetCallback(gkHttpSessionCallback* cb);
	bool usingSSLPeerCACheck() { return m_SSLCheckPeerCA;}
	bool usingSSLPeerHostnameCheck() { return m_SSLCheckPeerHostName;}

private:
	// if there comes a stream back you have to take care of the memory
	utStream* getURL(const gkString& url, const gkString& postData="", utStream* stream=0);
	CURL* nextFreeSlot();

	typedef utHashTable<utPointerHashKey,WriteCallbackData*> HandleMapping;

	HandleMapping m_handleMapping;

	int m_still_running;
	int m_handlesInUse;
	CURL* curl_handles[max_multihandles];
	CURLM* multi_handle;
	gkHttpSessionCallback* m_multiCallback;

	bool m_SSLCheckPeerCA;
	bool m_SSLCheckPeerHostName;
	bool m_blockingSession;
};

class gkNetClientManager : public utSingleton<gkNetClientManager>, public gkResourceManager{
public:
	gkNetClientManager();
	virtual ~gkNetClientManager();

	gkHttpSession* createHttpSession(const gkResourceName& name, bool blocking=true, bool sslCheckPeerCA=false,bool sslCheckHostName=false);

	gkActiveObject& getNetworkHelper() {
		return m_helperThread;
	}

	UT_DECLARE_SINGLETON(gkNetClientManager);

private:
	gkResource* createImpl(const gkResourceName& name, const gkResourceHandle& handle);

	gkResource* create(const gkResourceName& name) {
		gkResource* result = getByName(name);
		if (result)
			return result;

		return gkResourceManager::create(name);
	}

	enum CreationType {
		CREATION_HTTPSESSION
	};

	CreationType creationType;
	bool creationOption_sslCheckPeerCA;
	bool creationOption_sslCheckPeerHostname	;
	bool creationOption_blocking;
	gkActiveObject m_helperThread;

};

class gkNetworkUtils {
public:
	static bool isNetworkAvailable(const gkString& url="http://www.google.com",int timeout=5);
};

#endif /* GKDOWNLOADMANAGER_H_ */
