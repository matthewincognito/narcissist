find_package(CryptoPP REQUIRED)
find_package(secp256k1 REQUIRED)

add_library(narcissist
	base58.c
	bech32.c
	bitcoin.cpp
	narcissist.cpp
	wif.cpp)

target_link_libraries(narcissist
	PRIVATE
	${CRYPTOPP_LIBRARIES}
	${SECP256K1_LIBRARIES})
target_include_directories(narcissist
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE
	${CRYPTOPP_INCLUDE_DIR}
	${SECP256K1_INCLUDE_DIR})
