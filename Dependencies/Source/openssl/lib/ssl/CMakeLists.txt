project (openssl)

add_definitions(-DOPENSSL_THREADS -D_REENTRANT -DDSO_DLFCN -DHAVE_DLFCN_H -DL_ENDIAN)
add_definitions(-DOPENSSL_NO_CAMELLIA -DOPENSSL_NO_CAPIENG -DOPENSSL_NO_CAST -DOPENSSL_NO_CMS -DOPENSSL_NO_GMP -DOPENSSL_NO_IDEA -DOPENSSL_NO_JPAKE -DOPENSSL_NO_MD2 -DOPENSSL_NO_MDC2 -DOPENSSL_NO_RC5 -DOPENSSL_NO_SHA0 -DOPENSSL_NO_RFC3779 -DOPENSSL_NO_SEED -DOPENSSL_NO_STORE -DOPENSSL_NO_WHIRLPOOL )
add_definitions(-DOPENSSL_NO_HW -DOPENSSL_NO_ENGINE -DZLIB )


set (local_c_includes 
    ${CMAKE_CURRENT_SOURCE_DIR}/.. 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include 
    ${CMAKE_CURRENT_SOURCE_DIR}/../crypto
)

set (local_src_files
    s2_meth.c 
    s2_srvr.c 
    s2_clnt.c 
    s2_lib.c 
    s2_enc.c 
    s2_pkt.c 
    s3_meth.c 
    s3_srvr.c 
    s3_clnt.c 
    s3_lib.c 
    s3_enc.c 
    s3_pkt.c 
    s3_both.c 
    s23_meth.c 
    s23_srvr.c 
    s23_clnt.c 
    s23_lib.c 
    s23_pkt.c 
    t1_meth.c 
    t1_srvr.c 
    t1_clnt.c 
    t1_lib.c 
    t1_enc.c 
    t1_reneg.c 
    ssl_lib.c 
    ssl_err2.c 
    ssl_cert.c 
    ssl_sess.c 
    ssl_ciph.c 
    ssl_stat.c 
    ssl_rsa.c 
    ssl_asn1.c 
    ssl_txt.c 
    ssl_algs.c 
    bio_ssl.c 
    ssl_err.c 
    kssl.c
)

include_directories( ${local_c_includes} )

#add_library(crypto)
add_library(ssl STATIC ${local_src_files})
