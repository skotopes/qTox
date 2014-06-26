/*
    Copyright (C) 2013 by Maxim Biro <nurupo.contributions@gmail.com>

    This file is part of Tox Qt GUI.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the COPYING file for more details.
*/

#ifndef CORE_HPP
#define CORE_HPP

#include "status.h"

#include <tox/tox.h>
#include <tox/toxav.h>

#include <cstdint>
#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QFuture>

struct DhtServer
{
    QString name;
    QString userId;
    QString address;
    int port;
};

struct ToxFile
{
    enum FileStatus
    {
        STOPPED,
        PAUSED,
        TRANSMITTING
    };

    enum FileDirection : bool
    {
        SENDING,
        RECEIVING
    };

    ToxFile()=default;
    ToxFile(int FileNum, int FriendId, QByteArray FileData, long long Filesize, QByteArray FileName, FileDirection Direction)
        : fileNum(FileNum), friendId(FriendId), fileData{FileData}, fileName{FileName},
          bytesSent{0}, filesize(Filesize), status{STOPPED}, direction{Direction} {}

    int fileNum;
    int friendId;
    QByteArray fileData;
    QByteArray fileName;
    long long bytesSent;
    long long filesize;
    FileStatus status;
    FileDirection direction;
    QFuture<void> sendFuture;
};

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    ~Core();

    int getGroupNumberPeers(int groupId) const;
    QString getGroupPeerName(int groupId, int peerId) const;
    QList<QString> getGroupPeerNames(int groupId) const;
    int joinGroupchat(int32_t friendnumber, uint8_t* friend_group_public_key) const;
    void quitGroupChat(int groupId) const;

public slots:
    void start();

    void acceptFriendRequest(const QString& userId);
    void requestFriendship(const QString& friendAddress, const QString& message);

    void removeFriend(int friendId);
    void removeGroup(int groupId);

    void sendMessage(int friendId, const QString& message);
    void sendGroupMessage(int groupId, const QString& message);
    void sendAction(int friendId, const QString& action);
    void sendTyping(int friendId, bool typing);

    void sendFile(int32_t friendId, QString Filename, QByteArray data);
    void cancelFileSend(int friendId, int fileNum);
    void cancelFileRecv(int friendId, int fileNum);
    void rejectFileRecvRequest(int friendId, int fileNum);
    void acceptFileRecvRequest(int friendId, int fileNum);
    void pauseResumeFileSend(int friendId, int fileNum);
    void pauseResumeFileRecv(int friendId, int fileNum);

    void setUsername(const QString& username);
    void setStatusMessage(const QString& message);
    void setStatus(Status status);

    void process();

    void bootstrapDht();

signals:
    void connected();
    void disconnected();

    void friendRequestReceived(const QString& userId, const QString& message);
    void friendMessageReceived(int friendId, const QString& message);

    void friendAdded(int friendId, const QString& userId);

    void friendStatusChanged(int friendId, Status status);
    void friendStatusMessageChanged(int friendId, const QString& message);
    void friendUsernameChanged(int friendId, const QString& username);
    void friendTypingChanged(int friendId, bool isTyping);

    void friendStatusMessageLoaded(int friendId, const QString& message);
    void friendUsernameLoaded(int friendId, const QString& username);

    void friendAddressGenerated(const QString& friendAddress);

    void friendRemoved(int friendId);

    void friendLastSeenChanged(int friendId, const QDateTime& dateTime);

    void groupInviteReceived(int friendnumber, uint8_t *group_public_key);
    void groupMessageReceived(int groupnumber, int friendgroupnumber, const QString& message);
    void groupNamelistChanged(int groupnumber, int peernumber, uint8_t change);

    void usernameSet(const QString& username);
    void statusMessageSet(const QString& message);
    void statusSet(Status status);

    void messageSentResult(int friendId, const QString& message, int messageId);
    void actionSentResult(int friendId, const QString& action, int success);

    void failedToAddFriend(const QString& userId);
    void failedToRemoveFriend(int friendId);
    void failedToSetUsername(const QString& username);
    void failedToSetStatusMessage(const QString& message);
    void failedToSetStatus(Status status);
    void failedToSetTyping(bool typing);

    void actionReceived(int friendId, const QString& acionMessage);

    void failedToStart();

    void fileSendStarted(ToxFile file);
    void fileReceiveRequested(ToxFile file);
    void fileTransferAccepted(ToxFile file);
    void fileTransferCancelled(int FriendId, int FileNum, ToxFile::FileDirection direction);
    void fileTransferFinished(ToxFile file);
    void fileTransferPaused(int FriendId, int FileNum, ToxFile::FileDirection direction);
    void fileTransferInfo(int FriendId, int FileNum, int Filesize, int BytesSent, ToxFile::FileDirection direction);

private:
    static void onFriendRequest(Tox* tox, const uint8_t* cUserId, const uint8_t* cMessage, uint16_t cMessageSize, void* core);
    static void onFriendMessage(Tox* tox, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core);
    static void onFriendNameChange(Tox* tox, int friendId, uint8_t* cName, uint16_t cNameSize, void* core);
    static void onFriendTypingChange(Tox* tox, int friendId, uint8_t isTyping, void* core);
    static void onStatusMessageChanged(Tox* tox, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core);
    static void onUserStatusChanged(Tox* tox, int friendId, uint8_t userstatus, void* core);
    static void onConnectionStatusChanged(Tox* tox, int friendId, uint8_t status, void* core);
    static void onAction(Tox* tox, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core);
    static void onGroupInvite(Tox *tox, int friendnumber, uint8_t *group_public_key, void *userdata);
    static void onGroupMessage(Tox *tox, int groupnumber, int friendgroupnumber, uint8_t * message, uint16_t length, void *userdata);
    static void onGroupNamelistChange(Tox *tox, int groupnumber, int peernumber, uint8_t change, void *userdata);
    static void onFileSendRequestCallback(Tox *tox, int32_t friendnumber, uint8_t filenumber, uint64_t filesize,
                                          uint8_t *filename, uint16_t filename_length, void *userdata);
    static void onFileControlCallback(Tox *tox, int32_t friendnumber, uint8_t receive_send, uint8_t filenumber,
                                      uint8_t control_type, uint8_t *data, uint16_t length, void *core);
    static void onFileDataCallback(Tox *tox, int32_t friendnumber, uint8_t filenumber, uint8_t *data, uint16_t length, void *userdata);

    static void onAvInvite(int32_t call_index, void* toxav);
    static void onAvStart(int32_t call_index, void* toxav);
    static void onAvCancel(int32_t call_index, void* toxav);
    static void onAvReject(int32_t call_index, void* toxav);
    static void onAvEnd(int32_t call_index, void* toxav);
    static void onAvRinging(int32_t call_index, void* toxav);
    static void onAvStarting(int32_t call_index, void* toxav);
    static void onAvEnding(int32_t call_index, void* toxav);
    static void onAvError(int32_t call_index, void* toxav);
    static void onAvRequestTimeout(int32_t call_index, void* toxav);
    static void onAvPeerTimeout(int32_t call_index, void* toxav);

    void checkConnection();
    void onBootstrapTimer();

    void loadConfiguration();
    void saveConfiguration();
    void loadFriends();
    static void sendAllFileData(Core* core, ToxFile* file);

    static void removeFileFromQueue(bool sendQueue, int friendId, int fileId);

    void checkLastOnline(int friendId);

private:
    Tox* tox;
    ToxAv* toxav;
    QTimer *toxTimer, *saveTimer, *fileTimer, *bootstrapTimer;
    QList<DhtServer> dhtServerList;
    int dhtServerId;
    static QList<ToxFile> fileSendQueue, fileRecvQueue;

    static const QString CONFIG_FILE_NAME;
};

#endif // CORE_HPP
