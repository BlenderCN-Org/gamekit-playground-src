#include <Core/DecoratorTiled.h>
#include <Core/DecoratorTiledBox.h>
#include <Core/DecoratorTiledHorizontal.h>
#include <Core/DecoratorTiledImage.h>
#include <Core/DecoratorTiledVertical.h>
#include <Rocket/Core/Property.h>
#include <Rocket/Core/String.h>
#include "DecoratorAtlasImage.h"

#include "Gorilla.h"
using Rocket::Core::DecoratorTiled;
using Rocket::Core::DecoratorTiledHorizontal;

DecorationAtlasImageInstancer::DecorationAtlasImageInstancer() :
		DecoratorInstancer() {
	RegisterProperty("atlas", "").AddParser("string");
	RegisterProperty("image", "").AddParser("string");
	RegisterProperty("type", "image").AddParser("keyword","image,box,horizontal,vertical");
	RegisterProperty("u-x","0").AddParser("number");
	RegisterProperty("u-y","0").AddParser("number");
	RegisterProperty("l-x","1").AddParser("number");
	RegisterProperty("l-y","1").AddParser("number");
	RegisterProperty("color","rgb(255,255,255)").AddParser("color");
	RegisterProperty("repeat", "stretch")
				.AddParser("keyword", "stretch, clamp-stretch, clamp-truncate, repeat-stretch, repeat-truncate");

	RegisterShorthand("region","u-x, u-y, l-x, l-y");
}

DecorationAtlasImageInstancer::~DecorationAtlasImageInstancer() {
}

Decorator* DecorationAtlasImageInstancer::InstanceDecorator(
		const Rocket::Core::String& name,
		const PropertyDictionary& properties) {

	int type = properties.GetProperty("type")->Get<int>();

	const Rocket::Core::Property* atlasProperty = properties.GetProperty(
			"atlas");
	Rocket::Core::String atlasName(atlasProperty->Get<Rocket::Core::String>());
	Rocket::Core::String atlasImage(
			properties.GetProperty("image")->Get<Rocket::Core::String>());

	// get region-data
	int ux = properties.GetProperty("u-x")->Get<int>();
	int uy = properties.GetProperty("u-y")->Get<int>();
	int lx = properties.GetProperty("l-x")->Get<int>();
	int ly = properties.GetProperty("l-y")->Get<int>();

	const Rocket::Core::Property* repeat_property = properties.GetProperty("repeat");
	DecoratorTiled::TileRepeatMode repeatMode = (DecoratorTiled::TileRepeatMode) repeat_property->value.Get< int >();

	const Rocket::Core::Property* dstProp = properties.GetProperty("color");
	Rocket::Core::Colourb color = dstProp?dstProp->Get<Rocket::Core::Colourb>():Rocket::Core::Colourb(255,255,255);

	Gorilla::TextureAtlas* atlas = DecorationAtlasImageInstancer::getAtlas(
			atlasName);

	Rocket::Core::String texName(
			atlas->getTexture()->getName().c_str());

	if (atlas) {
		Gorilla::Sprite* sp = atlas->getSprite(atlasImage.CString());

		if (sp) {

			if (type == IMAGE)
			{
				DecoratorTiled::Tile tile;
				_createTile(tile,sp->uvLeft,sp->uvTop,sp->uvRight,sp->uvBottom,repeatMode);
				Rocket::Core::DecoratorTiledImage* decorator =
						new Rocket::Core::DecoratorTiledImage();
				if (decorator->Initialise(tile, texName, atlasProperty->source, color))
					return decorator;

				decorator->RemoveReference();
				ReleaseDecorator(decorator);
			} else {
				// values to translate from px-space to uv-space inside the atlas
				float dX = (sp->uvRight - sp->uvLeft) / sp->spriteWidth;
				float dY = (sp->uvBottom - sp->uvTop) / sp->spriteHeight;

				float left = sp->uvLeft + ux * dX;
				float right = sp->uvLeft + lx * dX;
				float top = sp->uvTop + uy * dY;
				float bottom = sp->uvTop + ly * dY;

				if (type == BOX)
				{
					DecoratorTiled::Tile tiles[9];
					Rocket::Core::String texture_names[9]={texName,texName,texName,texName,texName,texName,texName,texName, texName};
					Rocket::Core::String rcss_paths[9]={atlasProperty->source,atlasProperty->source,atlasProperty->source,atlasProperty->source,atlasProperty->source,atlasProperty->source,atlasProperty->source,atlasProperty->source,atlasProperty->source};

	 				_createTile(tiles[0],sp->uvLeft,sp->uvTop,left,top,repeatMode); 			/*top-left*/
					_createTile(tiles[1],right,sp->uvTop,sp->uvRight,top,repeatMode);			/*top-right*/
					_createTile(tiles[2],sp->uvLeft,bottom,left,sp->uvBottom,repeatMode);		/*bottom-left*/
					_createTile(tiles[3],right,bottom,sp->uvRight,sp->uvBottom,repeatMode);	/*bottom-right*/
					_createTile(tiles[4],sp->uvLeft,top,left,bottom,repeatMode);				/*left*/
					_createTile(tiles[5],right,top,sp->uvRight,bottom,repeatMode);				/*right*/
					_createTile(tiles[6],left,sp->uvTop,right,top,repeatMode);					/*top*/
					_createTile(tiles[7],left,bottom,right,sp->uvBottom,repeatMode);			/*bottom*/
					_createTile(tiles[8],left,top,right,bottom,repeatMode);					/*center*/

					Rocket::Core::DecoratorTiledBox* decorator = new Rocket::Core::DecoratorTiledBox();
					if (decorator->Initialise(tiles, texture_names, rcss_paths,color))
						return decorator;

					decorator->RemoveReference();
					ReleaseDecorator(decorator);
				}
				else if (type == HORIZONTAL)
				{
					DecoratorTiled::Tile tiles[3];
					Rocket::Core::String texture_names[3]={texName,texName,texName};
					Rocket::Core::String rcss_paths[3]={atlasProperty->source,atlasProperty->source,atlasProperty->source};

					_createTile(tiles[0],sp->uvLeft,sp->uvTop,left,sp->uvBottom,repeatMode); 			/*left*/
					_createTile(tiles[1],right,sp->uvTop,sp->uvRight,sp->uvBottom,repeatMode); 		/*right*/
					_createTile(tiles[2],left,sp->uvTop,right,sp->uvBottom,repeatMode); 				/*center*/

					DecoratorTiledHorizontal* decorator = new DecoratorTiledHorizontal();
					if (decorator->Initialise(tiles, texture_names, rcss_paths))
						return decorator;

					decorator->RemoveReference();
					ReleaseDecorator(decorator);
				}
				else if (type == VERTICAL)
				{
					DecoratorTiled::Tile tiles[3];
					Rocket::Core::String texture_names[3]={texName,texName,texName};
					Rocket::Core::String rcss_paths[3]={atlasProperty->source,atlasProperty->source,atlasProperty->source};

					_createTile(tiles[0],sp->uvLeft,sp->uvTop,sp->uvRight,top,repeatMode); 			/*top*/
					_createTile(tiles[1],sp->uvLeft,bottom,sp->uvRight,sp->uvBottom,repeatMode);    /*bottom*/				/*middle*/
					_createTile(tiles[2],sp->uvLeft,top,sp->uvRight,bottom,repeatMode); 		/*middle*/

					Rocket::Core::DecoratorTiledVertical* decorator = new Rocket::Core::DecoratorTiledVertical();
					if (decorator->Initialise(tiles, texture_names, rcss_paths))
						return decorator;

					decorator->RemoveReference();
					ReleaseDecorator(decorator);
				}

			}
		}
	}
	return 0;
}

void DecorationAtlasImageInstancer::_createTile(Rocket::Core::DecoratorTiled::Tile& tile
													,float ux,float uy, float lx,float ly
													,Rocket::Core::DecoratorTiled::TileRepeatMode repeateMode)
{
	tile.texcoords_absolute[0][0] = false;
	tile.texcoords_absolute[0][1] = false;
	tile.texcoords_absolute[1][0] = false;
	tile.texcoords_absolute[1][1] = false;

	tile.texcoords[0].x = ux;
	tile.texcoords[0].y = uy;
	tile.texcoords[1].x = lx;
	tile.texcoords[1].y = ly;

	tile.repeat_mode = repeateMode;
}

void DecorationAtlasImageInstancer::ReleaseDecorator(Decorator* decorator) {
	delete decorator;
}

void DecorationAtlasImageInstancer::Release() {
	AtlasMap::Iterator iter(atlasMap);
	while (iter.hasMoreElements()) {
		Gorilla::TextureAtlas* atlas = iter.peekNextValue();
		delete atlas;
		iter.next();
	}
	atlasMap.clear();
}

Gorilla::TextureAtlas* DecorationAtlasImageInstancer::getAtlas(
		const Rocket::Core::String& atlasName) {
	Gorilla::TextureAtlas** atlasPtr = atlasMap.get(
			utCharHashKey(atlasName.CString()));
	Gorilla::TextureAtlas* atlas = 0;
	if (!atlasPtr) {
		atlas = new Gorilla::TextureAtlas(atlasName.CString(), "default");
		atlasMap.insert(utCharHashKey(atlasName.CString()), atlas);
	} else {
		atlas = *atlasPtr;
	}
	return atlas;
}
