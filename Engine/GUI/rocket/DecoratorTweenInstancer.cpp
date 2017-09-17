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

#include "DecoratorTweenInstancer.h"
#include "DecoratorTween.h"
#include <Rocket/Core/String.h>
#include <Rocket/Core/PropertyDictionary.h>

/*
 *


 	transition:
 	 	LINEAR,
        SINE,
        QUINT,
        QUART,
        QUAD,
        EXPO,
        ELASTIC,
        CUBIC,
        CIRC,
        BOUNCE,
        BACK

	equation:
        EASE_IN,
        EASE_OUT,
        EASE_IN_OUT

	time: time for transition
	wait: delay
	value: the value
    repeat: times repeated
	repeat-reverse: with reverse-transition?
	repeat-infinite: repeate until style get deactivated
	on-start : event that is called when it gets activated
	on-end : event that is called when it finished

 */

DecoratorTweenInstancer::DecoratorTweenInstancer()
{
	RegisterProperty("transition","linear")
			.AddParser("keyword","linear,sine,quint,quart,quad,expo,elastic,cubic,circ,bounce,back").AddParser("string");
	RegisterProperty("equation","ease_out")
			.AddParser("keyword","ease_in,ease_out,ease_in_out");
	RegisterProperty("time","1.0").AddParser("number");
	RegisterProperty("wait","0.0").AddParser("number");
	RegisterProperty("prop","left").AddParser("string");
	RegisterProperty("value","0.0").AddParser("color").AddParser("number").AddParser("string");
	RegisterProperty("repeat","0").AddParser("number");
	RegisterProperty("repeat-reverse","false").AddParser("keyword","false,true");
	RegisterProperty("repeat-infinite","false").AddParser("keyword","false,true");
	RegisterProperty("time","1.0").AddParser("number");
	RegisterProperty("event-callback","").AddParser("string");
	RegisterProperty("event-inline","").AddParser("string");



	RegisterShorthand("tween-s","tween-prop,tween-value,tween-time,tween-transition,tween-equation");

}

DecoratorTweenInstancer::~DecoratorTweenInstancer()
{
}

// Instances a decorator given the property tag and attributes from the RCSS file.
Decorator* DecoratorTweenInstancer::InstanceDecorator(const Rocket::Core::String& name, const PropertyDictionary& properties)
{
	int transition = properties.GetProperty("transition")->Get<int>();
	int equation = properties.GetProperty("equation")->Get<int>();
	float time = properties.GetProperty("time")->Get<float>();
	float wait = properties.GetProperty("wait")->Get<float>();
	Rocket::Core::String propertyName(properties.GetProperty("prop")->Get<Rocket::Core::String>());
	Rocket::Core::String eventFunction(properties.GetProperty("event-callback")->Get<Rocket::Core::String>());
	Rocket::Core::String eventInline(properties.GetProperty("event-inline")->Get<Rocket::Core::String>());
	// the value that should be reached at after time-secs
	const Property* dstProp = properties.GetProperty("value");


	float repeat = properties.GetProperty("repeat")->Get<float>();
	bool repeatWithReverse = properties.GetProperty("repeat-reverse")->Get<int>();
	bool repeatInfinite = properties.GetProperty("repeat-infinite")->Get<int>();

	gkString unit;
	switch (dstProp->unit){
		case Property::PX: unit = " px";
							break;
		case Property::EM: unit = " em";
							break;
		case Property::PERCENT: unit = " %";
							break;
	}



	DecoratorTween* decoTween =  new DecoratorTween();
	if (decoTween->initalize(transition,equation,time,wait,propertyName,dstProp,unit,repeat,repeatWithReverse,repeatInfinite,eventFunction,eventInline))
		return decoTween;

	decoTween->RemoveReference();
	ReleaseDecorator(decoTween);
	return NULL;
}

// Releases the given decorator.
void DecoratorTweenInstancer::ReleaseDecorator(Decorator* decorator)
{
	delete decorator;
}

// Releases the instancer.
void DecoratorTweenInstancer::Release()
{
	delete this;
}


