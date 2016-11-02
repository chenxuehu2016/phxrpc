
set -e  # exit immediately on error
set -x  # display all commands

mkdir -p third_party

cd third_party;

if [ ! -f protobuf/bin/protoc ]; then
	if [ ! -f protobuf-cpp-3.0.0.tar.gz ]; then
		wget https://github.com/google/protobuf/releases/download/v3.0.0/protobuf-cpp-3.0.0.tar.gz
	fi	

	tar zxvf protobuf-cpp-3.0.0.tar.gz
	cd protobuf-3.0.0

	./configure --prefix=`pwd`/../protobuf
	make -j2
	make install

	cd ../

fi

if [ ! -f tinyxml2/libtinyxml2.a ]; then

	if [ ! -f tinyxml2.tar.gz ]; then
		wget https://github.com/leethomason/tinyxml2/archive/4.0.1.tar.gz -O ./tinyxml2-4.0.1.tar.gz
	fi
	tar zxvf tinyxml2-4.0.1.tar.gz
	cd tinyxml2-4.0.1
	make 
	cd ..
	ln -s tinyxml2-4.0.1 tinyxml2
fi


exit $?
