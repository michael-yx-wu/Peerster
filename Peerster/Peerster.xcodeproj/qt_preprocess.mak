#############################################################################
# Makefile for building: Peerster.app/Contents/MacOS/Peerster
# Generated by qmake (2.01a) (Qt 4.8.5) on: Tue Dec 17 17:16:04 2013
# Project:  peerster.pro
# Template: app
# Command: /usr/local/bin/qmake -spec /usr/local/Cellar/qt/4.8.5/mkspecs/macx-xcode -o Peerster.xcodeproj/project.pbxproj peerster.pro
#############################################################################

MOC       = /usr/local/Cellar/qt/4.8.5/bin/moc
UIC       = /usr/local/Cellar/qt/4.8.5/bin/uic
LEX       = flex
LEXFLAGS  = 
YACC      = yacc
YACCFLAGS = -d
DEFINES       = -DQT_NO_DEBUG -DQT_MULTIMEDIA_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
INCPATH       = -I/usr/local/Cellar/qt/4.8.5/mkspecs/macx-xcode -I. -I/usr/local/Cellar/qt/4.8.5/lib/QtCore.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtCore.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtNetwork.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtNetwork.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtGui.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtGui.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtMultimedia.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtMultimedia.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/include -I. -I/usr/local/Cellar/qt/4.8.5/include/QtCrypto -I. -I/usr/local/include -I/System/Library/Frameworks/CarbonCore.framework/Headers -F/usr/local/Cellar/qt/4.8.5/lib
DEL_FILE  = rm -f
MOVE      = mv -f

IMAGES = 
PARSERS =
preprocess: $(PARSERS) compilers
clean preprocess_clean: parser_clean compiler_clean

parser_clean:
check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compilers: ./moc_main.cpp ./moc_Socket.cpp ./moc_PeersterFile.cpp\
	 ./moc_Chatbox.cpp ./moc_FilePanel.cpp ./moc_PrivateChatDialog.cpp\
	 ./moc_PrivateMessagingPanel.cpp ./moc_VoipPanel.cpp
compiler_objective_c_make_all:
compiler_objective_c_clean:
compiler_moc_header_make_all: moc_main.cpp moc_Socket.cpp moc_PeersterFile.cpp moc_Chatbox.cpp moc_FilePanel.cpp moc_PrivateChatDialog.cpp moc_PrivateMessagingPanel.cpp moc_VoipPanel.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_main.cpp moc_Socket.cpp moc_PeersterFile.cpp moc_Chatbox.cpp moc_FilePanel.cpp moc_PrivateChatDialog.cpp moc_PrivateMessagingPanel.cpp moc_VoipPanel.cpp
moc_main.cpp: Constants.hh \
		GUI/Chatbox.hh \
		GUI/FilePanel.hh \
		GUI/PrivateMessagingPanel.hh \
		Messages/DHKeyMessage.hh \
		Messages/Message.hh \
		GUI/PrivateChatDialog.hh \
		GUI/VoipPanel.hh \
		Peer.hh \
		Messages/AudioMessage.hh \
		Messages/BlockReplyMessage.hh \
		Messages/BlockRequestMessage.hh \
		Messages/SearchReplyMessage.hh \
		Messages/SearchRequestMessage.hh \
		FileSharing/PeersterFile.hh \
		FileSharing/SearchResult.hh \
		Messages.hh \
		main.hh
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ main.hh -o moc_main.cpp

moc_Socket.cpp: Socket.hh
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ Socket.hh -o moc_Socket.cpp

moc_PeersterFile.cpp: Constants.hh \
		FileSharing/PeersterFile.hh
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ FileSharing/PeersterFile.hh -o moc_PeersterFile.cpp

moc_Chatbox.cpp: GUI/Chatbox.hh
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ GUI/Chatbox.hh -o moc_Chatbox.cpp

moc_FilePanel.cpp: GUI/PrivateMessagingPanel.hh \
		Messages/DHKeyMessage.hh \
		Messages/Message.hh \
		Constants.hh \
		GUI/PrivateChatDialog.hh \
		GUI/Chatbox.hh \
		GUI/VoipPanel.hh \
		Peer.hh \
		Messages/AudioMessage.hh \
		Messages/BlockReplyMessage.hh \
		Messages/BlockRequestMessage.hh \
		Messages/SearchReplyMessage.hh \
		Messages/SearchRequestMessage.hh \
		FileSharing/PeersterFile.hh \
		FileSharing/SearchResult.hh \
		GUI/FilePanel.hh
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ GUI/FilePanel.hh -o moc_FilePanel.cpp

moc_PrivateChatDialog.cpp: GUI/Chatbox.hh \
		Messages/Message.hh \
		Constants.hh \
		GUI/VoipPanel.hh \
		Peer.hh \
		Messages/AudioMessage.hh \
		GUI/PrivateChatDialog.hh
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ GUI/PrivateChatDialog.hh -o moc_PrivateChatDialog.cpp

moc_PrivateMessagingPanel.cpp: Messages/DHKeyMessage.hh \
		Messages/Message.hh \
		Constants.hh \
		GUI/PrivateChatDialog.hh \
		GUI/Chatbox.hh \
		GUI/VoipPanel.hh \
		Peer.hh \
		Messages/AudioMessage.hh \
		GUI/PrivateMessagingPanel.hh
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ GUI/PrivateMessagingPanel.hh -o moc_PrivateMessagingPanel.cpp

moc_VoipPanel.cpp: Peer.hh \
		Messages/AudioMessage.hh \
		Messages/Message.hh \
		Constants.hh \
		GUI/VoipPanel.hh
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ GUI/VoipPanel.hh -o moc_VoipPanel.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

