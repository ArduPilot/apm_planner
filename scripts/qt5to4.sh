#! /bin/sh

rm /usr/bin/qmake
echo "Creating a new link\n"
if  ln -s /usr/bin/qmake-4.8 /usr/bin/qmake
then
        echo "Switched to Version"
	qmake -v
else
        echo "failed to create link"
fi
