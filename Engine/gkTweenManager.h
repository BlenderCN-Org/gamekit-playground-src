/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2012 Thomas Trocha

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#ifndef _gkTweenManagerObject_h_
#define _gkTweenManagerObject_h_

#include "gkSerialize.h"
#include "CppTweener.h"
#include "../Dependencies/Source/GameKit/Utils/utSingleton.h"

class gkTweenManager : public tween::TweenerListener, public utSingleton<gkTweenManager>
{

public:
	typedef tween::TweenCallback gkTweenCallback;

	gkTweenManager();
	virtual ~gkTweenManager();

	const tween::Tweener& getTweener() { return m_tweener;}

	void update(gkScalar dt);

	void addTween(tween::TweenerParam& param, tween::TweenerListener* listener=0);

	UT_DECLARE_SINGLETON(gkTweenManager)

private:
	tween::Tweener m_tweener;

	void onStart(tween::TweenerParam& param);
	void onStep(tween::TweenerParam& param);
	void onComplete(tween::TweenerParam& param);


};

#endif//_gkTweenManagerObject_h_
