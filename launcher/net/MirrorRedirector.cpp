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

#include "MirrorRedirector.h"

#include <QDebug>
#include <QString>

namespace {
struct MirrorRule {
    const char* from;
    const char* to;
};

// Field-proven prefix table sourced from HMCL's BMCLAPIDownloadProvider and PCL2.
// Entries are matched as string prefixes from position 0 of the full URL; the
// first matching entry wins. Extend cautiously.
constexpr MirrorRule s_mirrorRules[] = {
    { "https://launchermeta.mojang.com", "https://bmclapi2.bangbang93.com" },
    { "https://piston-meta.mojang.com", "https://bmclapi2.bangbang93.com" },
    { "https://piston-data.mojang.com", "https://bmclapi2.bangbang93.com" },
    { "https://launcher.mojang.com", "https://bmclapi2.bangbang93.com" },
    { "https://resources.download.minecraft.net", "https://bmclapi2.bangbang93.com/assets" },
    { "https://libraries.minecraft.net", "https://bmclapi2.bangbang93.com/libraries" },
    { "https://files.minecraftforge.net/maven", "https://bmclapi2.bangbang93.com/maven" },
    { "https://maven.minecraftforge.net", "https://bmclapi2.bangbang93.com/maven" },
    { "https://libraries.minecraftforge.net", "https://bmclapi2.bangbang93.com/maven" },
};
}  // namespace

QUrl MirrorRedirector::redirect(const QUrl& url, bool enabled)
{
    if (!enabled) {
        return url;
    }

    const QString before = url.toString();
    // Never rewrite a URL that already points at the mirror: mirrors can redirect back to
    // an upstream host, and re-rewriting such redirects could ping-pong forever.
    if (url.host() == QLatin1String("bmclapi2.bangbang93.com")) {
        return url;
    }
    for (const auto& rule : s_mirrorRules) {
        const QString from = QLatin1String(rule.from);
        if (before.startsWith(from)) {
            const QString after = QLatin1String(rule.to) + before.mid(from.size());
            qDebug() << "MirrorRedirector:" << before << "->" << after;
            return QUrl(after);
        }
    }

    return url;
}
