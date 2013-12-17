Peerster
========
Before attempting to compile, be sure that you have QT and QCA installed.

##### Mac OS X Compilation Instructions
If you would like to use Xcode to run the project, do the following:
1. cd $peerster/Peerster; qmake -spec macx-xcode
2. Open the Peerster.xcodeproj using Xcode. Peerster should index and build
automatically.

Xcode builds the source code into Peerster.app. If you would also like a normal
executable, do the following:
1. cd Peerster; qmake
2. make

##### Linux Compilation Instructions
1. cd $peerster/Peerster; qmake
2. make

