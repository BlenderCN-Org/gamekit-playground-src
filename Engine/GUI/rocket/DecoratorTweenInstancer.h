/*
-------------------------------------------------------------------------------
    This file is part of ogrekit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2013 Thomas Trocha (dertom)

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

#ifndef DecoratorTweenINSTANCER_H
#define DecoratorTweenINSTANCER_H

#include <Rocket/Core/DecoratorInstancer.h>

using namespace Rocket::Core;

/**
	The instancer for the none decorator.

	@author Peter Curry
 */

class DecoratorTweenInstancer : public DecoratorInstancer
{
public:
	DecoratorTweenInstancer();
	virtual ~DecoratorTweenInstancer();

	/// Instances a decorator given the property tag and attributes from the RCSS file.
	/// @param name The type of decorator desired. For example, "background-decorator: simple;" is declared as type "simple".
	/// @param properties All RCSS properties associated with the decorator.
	/// @return The decorator if it was instanced successful, NULL if an error occured.
	virtual Decorator* InstanceDecorator(const Rocket::Core::String& name, const PropertyDictionary& properties);
	/// Releases the given decorator.
	/// @param decorator Decorator to release. This is guaranteed to have been constructed by this instancer.
	virtual void ReleaseDecorator(Decorator* decorator);

	/// Releases the instancer.
	virtual void Release();
};



#endif
