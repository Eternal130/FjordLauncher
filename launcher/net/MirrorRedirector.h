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

#include <QUrl>

/**
 * Redirects well-known Mojang/Forge download hosts to the BMCLAPI mirror
 * (bmclapi2.bangbang93.com), in the same fashion as HMCL's automatic download
 * provider.
 *
 * The prefix table is field-proven and sourced from HMCL's BMCLAPIDownloadProvider
 * and PCL2. Extend it cautiously: entries are matched as plain string prefixes
 * against the full URL, and the first matching entry wins.
 */
class MirrorRedirector {
   public:
    /**
     * Rewrites the given URL to its BMCLAPI counterpart if it starts with one of
     * the known mirrorable prefixes.
     *
     * \param url The original request URL.
     * \param enabled Whether mirror redirection is currently enabled in the settings.
     * \returns The redirected URL, or the input URL unchanged when redirection is
     *          disabled or no known prefix matches.
     */
    static QUrl redirect(const QUrl& url, bool enabled);
};
