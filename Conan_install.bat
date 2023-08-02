conan install . ^
	-of _Build/ZTCpp-x64/ ^
	--profile=default ^
	-s build_type=Debug ^
	-s compiler.cppstd=17 ^
	--build=missing ^
    --build=outdated ^
    --build=cascade
	
conan install . ^
	-of _Build/ZTCpp-x64/ ^
	--profile=default ^
	-s build_type=Release ^
	-s compiler.cppstd=17 ^
	--build=missing ^
    --build=outdated ^
    --build=cascade
