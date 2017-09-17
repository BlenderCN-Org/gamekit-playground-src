#ifndef __DECORATOR_ATLASIMAGE
#define __DECORATOR_ATLASIMAGE

#include <Core/DecoratorTiled.h>
#include <Gorilla.h>
#include <Rocket/Core/DecoratorInstancer.h>
#include <Rocket/Core/String.h>
#include <utTypes.h>

using Rocket::Core::Decorator;
using Rocket::Core::DecoratorDataHandle;
using Rocket::Core::Element;
using Rocket::Core::PropertyDictionary;

namespace Gorilla {
	class TextureAtlas;
}

class DecorationAtlasImageInstancer : public Rocket::Core::DecoratorInstancer
{
public:
	DecorationAtlasImageInstancer();
	virtual ~DecorationAtlasImageInstancer();

	enum Type {
		IMAGE = 0,BOX, HORIZONTAL, VERTICAL
	};

	/// Instances an image decorator.
	virtual Decorator* InstanceDecorator(const Rocket::Core::String& name, const PropertyDictionary& properties);
	/// Releases the given decorator.
	virtual void ReleaseDecorator(Decorator* decorator);

	/// Releases the instancer.
	virtual void Release();

private:
	Gorilla::TextureAtlas* getAtlas(const Rocket::Core::String& atlasName);
	void _createTile(Rocket::Core::DecoratorTiled::Tile& tile,float ux,float uy, float lx,float ly , Rocket::Core::DecoratorTiled::TileRepeatMode repeateMode = Rocket::Core::DecoratorTiled::CLAMP_STRETCH );
	typedef utHashTable<utCharHashKey,Gorilla::TextureAtlas*> AtlasMap;
	AtlasMap atlasMap;
};



#endif
