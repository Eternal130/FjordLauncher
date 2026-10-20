// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Fjord Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Fjord Launcher Contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "PasswordStore.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QSaveFile>

namespace {
// Random, build-time-constant key. NOTE: this is obfuscation, not security: the key ships
// inside the binary, so anyone with the binary and the stored file can recover a password.
// A per-build key would NOT work here: it would invalidate every stored password on update.
constexpr auto kKeyHex = "59420c7d02e1d237846c44056a2c1a2cee0056f3bd8d3347076e946161f414e5";
constexpr auto kStoreFileName = "passwords.json";
constexpr auto kNonceSize = 16;

QByteArray crypt(const QByteArray& data, const QByteArray& nonce)
{
    // SHA-256 in counter mode as the keystream (XOR stream cipher).
    constexpr qsizetype blockSize = 32;
    const QByteArray key = QByteArray::fromHex(kKeyHex);

    QByteArray out;
    out.resize(data.size());
    for (qsizetype block = 0; block * blockSize < data.size(); ++block) {
        const auto counter = quint32(block);
        const QByteArray seed = key + nonce + QByteArray(reinterpret_cast<const char*>(&counter), sizeof(counter));
        const QByteArray digest = QCryptographicHash::hash(seed, QCryptographicHash::Sha256);
        for (qsizetype i = 0; i < blockSize && block * blockSize + i < data.size(); ++i) {
            out[block * blockSize + i] = data[block * blockSize + i] ^ digest[i];
        }
    }
    return out;
}

QString encrypt(const QString& password)
{
    QByteArray nonce;
    QRandomGenerator* generator = QRandomGenerator::system();
    for (int i = 0; i < kNonceSize / int(sizeof(quint32)); ++i) {
        const auto value = generator->generate();
        nonce.append(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    const QByteArray encrypted = crypt(password.toUtf8(), nonce);
    return QByteArray(nonce + encrypted).toBase64();
}

QString decrypt(const QString& encoded)
{
    const QByteArray raw = QByteArray::fromBase64(encoded.toUtf8());
    if (raw.size() < kNonceSize) {
        return {};
    }
    return QString::fromUtf8(crypt(raw.sliced(kNonceSize), raw.first(kNonceSize)));
}

// NOTE: the store is only ever accessed from the GUI thread.
QJsonObject loadStore()
{
    QFile file(kStoreFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }
    return QJsonDocument::fromJson(file.readAll()).object().value("accounts").toObject();
}

void saveStore(const QJsonObject& accounts)
{
    QSaveFile file(kStoreFileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "PasswordStore: cannot open" << kStoreFileName << ":" << file.errorString();
        return;
    }

    QJsonObject root;
    root.insert("accounts", accounts);
    file.write(QJsonDocument(root).toJson(QJsonDocument::Compact));

    if (!file.commit()) {
        qWarning() << "PasswordStore: cannot write" << kStoreFileName << ":" << file.errorString();
    }
}
}  // namespace

void PasswordStore::store(const QString& key, const QString& password)
{
    QJsonObject accounts = loadStore();
    accounts.insert(key, encrypt(password));
    saveStore(accounts);
}

QString PasswordStore::load(const QString& key)
{
    const QString encoded = loadStore().value(key).toString();
    if (encoded.isEmpty()) {
        return {};
    }
    return decrypt(encoded);
}

void PasswordStore::remove(const QString& key)
{
    QJsonObject accounts = loadStore();
    if (accounts.remove(key) == 0) {
        return;
    }
    saveStore(accounts);
}
