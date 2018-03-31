find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
find_package(CryptoPP REQUIRED)
find_package(secp256k1 REQUIRED)

add_executable(narcissist-cli
	main.cpp)
target_link_libraries(narcissist-cli
	PRIVATE
	narcissist
	${Boost_LIBRARIES}
	${CRYPTOPP_LIBRARIES}
	${SECP256K1_LIBRARIES}
	gmp)
target_include_directories(narcissist-cli
	PRIVATE
	${Boost_INCLUDE_DIR}
	${CRYPTOPP_INCLUDE_DIR}
	${SECP256K1_INCLUDE_DIR})
