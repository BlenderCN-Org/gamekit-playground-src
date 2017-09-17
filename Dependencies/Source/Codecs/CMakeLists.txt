include_directories(include Vorbis)

set(OGG
	include/ogg/ogg.h
	include/ogg/os_types.h
	Ogg/bitwise.c
	Ogg/framing.c
)

set(VORBIS
	include/vorbis/codec.h
	include/vorbis/vorbisenc.h
	include/vorbis/vorbisfile.h
	Vorbis/backends.h
	Vorbis/bitrate.h
	Vorbis/codebook.h
	Vorbis/codec_internal.h
	Vorbis/envelope.h
	Vorbis/highlevel.h
	Vorbis/lookup.h
	Vorbis/lookup_data.h
	Vorbis/lpc.h
	Vorbis/lsp.h
	Vorbis/masking.h
	Vorbis/mdct.h
	Vorbis/misc.h
	Vorbis/os.h
	Vorbis/psy.h
	Vorbis/registry.h
	Vorbis/scales.h
	Vorbis/smallft.h
	Vorbis/window.h

	Vorbis/analysis.c
	#Vorbis/barkmel.c
	Vorbis/bitrate.c
	Vorbis/block.c
	Vorbis/codebook.c
	Vorbis/envelope.c
	Vorbis/floor0.c
	Vorbis/floor1.c
	Vorbis/info.c
	Vorbis/lookup.c
	Vorbis/lpc.c
	Vorbis/lsp.c
	Vorbis/mapping0.c
	Vorbis/mdct.c
	Vorbis/psy.c
	Vorbis/registry.c
	Vorbis/res0.c
	Vorbis/sharedbook.c
	Vorbis/smallft.c
	Vorbis/synthesis.c
	Vorbis/vorbisenc.c
	Vorbis/vorbisfile.c
	Vorbis/window.c
)

#there were some x86_64 files that might need to be included on x86_64! Look makefiles! Not sure
#anymore :D

set (THEORA
	include/theora/codec.h
	include/theora/config.h
	include/theora/theora.h
	include/theora/theoradec.h
	include/theora/theoraenc.h
	
	Theora/analyze.c 
	Theora/apiwrapper.c 
	Theora/bitpack.c 
	Theora/decapiwrapper.c 
	Theora/decinfo.c 
	Theora/decode.c 
	Theora/dequant.c 
	Theora/encapiwrapper.c 
	Theora/encfrag.c 
	Theora/encinfo.c 
	Theora/encode.c 
	Theora/enquant.c 
	Theora/fdct.c 
	Theora/fragment.c 
	Theora/huffdec.c 
	Theora/huffenc.c 
	Theora/idct.c 
	Theora/info.c 
	Theora/internal.c 
	Theora/mathops.c 
	Theora/mcenc.c 
	Theora/x86/mmxencfrag.c 
	Theora/x86/mmxfdct.c 
	Theora/x86/mmxfrag.c 
	Theora/x86/mmxidct.c 
	Theora/x86/mmxstate.c 
	Theora/quant.c 
	Theora/rate.c 
	Theora/state.c 
	Theora/tokenize.c 
	Theora/x86_vc/x86enc.c 
	Theora/x86_vc/x86state.c )


if (OGREKIT_USE_THEORA)
	add_library(${OGREKIT_OGGVORBIS_TARGET} ${OGG} ${VORBIS} ${THEORA})
else()
	add_library(${OGREKIT_OGGVORBIS_TARGET} ${OGG} ${VORBIS} )
endif()

config_target_if_iphone_build(${OGREKIT_OGGVORBIS_TARGET} TRUE)
