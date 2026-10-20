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

#pragma once

#include <QString>

/**
 * Obfuscated local password storage, used to remember the passwords of authlib-injector
 * accounts so that they can be recovered automatically once their tokens expire (Yggdrasil
 * has no refresh tokens, so a full re-authentication with the password is the only way to
 * recover).
 *
 * Passwords are encrypted with a key embedded in the binary and stored in passwords.json
 * next to accounts.json. This is obfuscation, NOT real protection: anyone with access to
 * both the stored file and any copy of the launcher binary can recover the passwords. It
 * is a deliberate trade-off for a dependency-free store that works on every platform.
 */
class PasswordStore {
   public:
    /// Stores (or overwrites) the password saved under the given key. Silently ignores errors.
    static void store(const QString& key, const QString& password);

    /// Returns the password saved under the given key, or an empty string if there is none.
    static QString load(const QString& key);

    /// Removes the password saved under the given key. Silently ignores errors.
    static void remove(const QString& key);
};
