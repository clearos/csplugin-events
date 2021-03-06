// ClearSync: System Monitor plugin.
// Copyright (C) 2011 ClearFoundation <http://www.clearfoundation.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <clearsync/csplugin.h>

#include <sys/types.h>

#include <unistd.h>
#include <pwd.h>
#include <openssl/sha.h>

#include "events-alert.h"

csEventsAlert::csEventsAlert()
{
    Reset();
    SetCreated();
    SetUpdated(GetCreated());
}

csEventsAlert::csEventsAlert(
    uint32_t id, uint32_t flags, uint32_t type, const string &origin,
    const string &basename, const string &uuid, const string &desc)
{
    Reset();
    SetCreated();
    SetUpdated(GetCreated());

    data.id = id;
    data.flags = flags;
    data.type = type;
    data.origin = origin;
    data.basename = basename;
    data.uuid = uuid;
    data.desc = desc;
}

csEventsAlert::~csEventsAlert()
{
}

void csEventsAlert::Reset(void)
{
    data.id = 0;
    data.created = 0;
    data.updated = 0;
    data.flags = csAF_LVL_NORM;
    data.type = csAT_NULL;
    data.user = 0;
    data.groups.clear();
    data.origin.clear();
    data.basename.clear();
    data.uuid.clear();
    data.desc.clear();
}

void csEventsAlert::AddGroup(gid_t gid)
{
    bool found = false;
    for (vector<gid_t>::iterator i = data.groups.begin();
        i != data.groups.end(); i++) {
        if ((*i) != gid) continue;
        found = true;
        break;
    }

    if (!found) data.groups.push_back(gid);
}

void csEventsAlert::GetGroups(vector<gid_t> &groups)
{
    groups.clear();
    for (vector<gid_t>::iterator i = data.groups.begin();
        i != data.groups.end(); i++) {
        groups.push_back((*i));
    }
}

void csEventsAlert::SetData(const csEventsAlertData &data)
{
    SetId(data.id);
    SetCreated(data.created);
    SetUpdated(data.updated);
    SetFlags(data.flags);
    SetType(data.type);
    SetUser(data.user);

    for (vector<gid_t>::const_iterator i = data.groups.begin();
        i != data.groups.end(); i++) AddGroup((*i));

    SetOrigin(data.origin);
    SetBasename(data.basename);
    SetUUID(data.uuid);
    SetDescription(data.desc);
}

void csEventsAlert::SetUser(const string &user)
{
    struct passwd *pwent = NULL;

    pwent = getpwnam(user.c_str());
    if (pwent == NULL) {
        unsigned long long uid = strtoull(user.c_str(), NULL, 0);
        if ((pwent = getpwuid((uid_t)uid)) == NULL)
            throw csException(ENOENT, "User not found");
    }
    data.user = pwent->pw_uid;
}

void csEventsAlert::SetUser(void)
{
    data.user = geteuid();
}

void csEventsAlert::UpdateHash(void)
{
    SHA_CTX ctx;

    if (SHA1_Init(&ctx) != 1)
        throw csException(EINVAL, "SHA1_Init");

    // Hash alert fields...

    // ...type
    SHA1_Update(&ctx, (const uint8_t *)&data.type, sizeof(uint32_t));
    // ...user
    SHA1_Update(&ctx, (const uint8_t *)&data.user, sizeof(uid_t));
    // ...groups
    for (vector<gid_t>::const_iterator i = data.groups.begin();
        i != data.groups.end(); i++) {
        SHA1_Update(&ctx, (const uint8_t *)&(*i), sizeof(gid_t));
    }

    // ...origin
    if (data.origin.length() > 0) {
        SHA1_Update(&ctx,
            (const uint8_t *)data.origin.c_str(), data.origin.length());
    }

    // ...basename
    if (data.basename.length() > 0) {
        SHA1_Update(&ctx,
            (const uint8_t *)data.basename.c_str(), data.basename.length());
    }

    // ...uuid
    if (data.uuid.length() > 0) {
        SHA1_Update(&ctx,
            (const uint8_t *)data.uuid.c_str(), data.uuid.length());
    }

    // ...description
    /*
    if (data.desc.length() > 0) {
        SHA1_Update(&ctx,
            (const uint8_t *)data.desc.c_str(), data.desc.length());
    }
    */

    SHA1_Final(hash, &ctx);
    ::csBinaryToHex(hash, hash_str, SHA_DIGEST_LENGTH);
#if 0
    csLog::Log(csLog::Debug, "Alert hash: %s", hash_str.c_str());
#endif
}

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4
