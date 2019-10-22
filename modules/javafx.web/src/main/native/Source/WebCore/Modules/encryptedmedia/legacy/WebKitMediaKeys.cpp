/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebKitMediaKeys.h"

#if ENABLE(LEGACY_ENCRYPTED_MEDIA)

#include "HTMLMediaElement.h"
#include "WebKitMediaKeySession.h"

namespace WebCore {

ExceptionOr<Ref<WebKitMediaKeys>> WebKitMediaKeys::create(const String& keySystem)
{
    // From <http://dvcs.w3.org/hg/html-media/raw-file/tip/encrypted-media/encrypted-media.html#dom-media-keys-constructor>:
    // The MediaKeys(keySystem) constructor must run the following steps:

    // 1. If keySystem is null or an empty string, throw an InvalidAccessError exception and abort these steps.
    if (keySystem.isEmpty())
        return Exception { InvalidAccessError };

    // 2. If keySystem is not one of the user agent's supported Key Systems, throw a NotSupportedError and abort these steps.
    if (!LegacyCDM::supportsKeySystem(keySystem))
        return Exception { NotSupportedError };

    // 3. Let cdm be the content decryption module corresponding to keySystem.
    // 4. Load cdm if necessary.
    auto cdm = LegacyCDM::create(keySystem);

    // 5. Create a new MediaKeys object.
    // 5.1 Let the keySystem attribute be keySystem.
    // 6. Return the new object to the caller.
    return adoptRef(*new WebKitMediaKeys(keySystem, WTFMove(cdm)));
}

WebKitMediaKeys::WebKitMediaKeys(const String& keySystem, std::unique_ptr<LegacyCDM>&& cdm)
    : m_keySystem(keySystem)
    , m_cdm(WTFMove(cdm))
{
    m_cdm->setClient(this);
}

WebKitMediaKeys::~WebKitMediaKeys()
{
    // From <http://dvcs.w3.org/hg/html-media/raw-file/tip/encrypted-media/encrypted-media.html#dom-media-keys-constructor>:
    // When destroying a MediaKeys object, follow the steps in close().
    for (auto& session : m_sessions) {
        session->close();
        session->detachKeys();
    }
}

ExceptionOr<Ref<WebKitMediaKeySession>> WebKitMediaKeys::createSession(ScriptExecutionContext& context, const String& type, Ref<Uint8Array>&& initData)
{
    // From <http://www.w3.org/TR/2014/WD-encrypted-media-20140218/#dom-createsession>:
    // The createSession(type, initData) method must run the following steps:
    // Note: The contents of initData are container-specific Initialization Data.

    // 1. If contentType is null or an empty string, throw an InvalidAccessError exception and abort these steps.
    if (type.isEmpty())
        return Exception { InvalidAccessError };

    // 2. If initData is an empty array, throw an InvalidAccessError exception and abort these steps.
    if (!initData->length())
        return Exception { InvalidAccessError };

    // 3. If type contains a MIME type that is not supported or is not supported by the keySystem, throw
    // a NotSupportedError exception and abort these steps.
    if (!m_cdm->supportsMIMEType(type))
        return Exception { NotSupportedError };

    // 4. Create a new MediaKeySession object.
    // 4.1 Let the keySystem attribute be keySystem.
    // 4.2 Let the sessionId attribute be a unique Session ID string. It may be generated by cdm.
    auto session = WebKitMediaKeySession::create(context, *this, m_keySystem);

    m_sessions.append(session.copyRef());

    // 5. Schedule a task to initialize the session, providing contentType, initData, and the new object.
    session->generateKeyRequest(type, WTFMove(initData));

    // 6. Return the new object to the caller.
    return WTFMove(session);
}

bool WebKitMediaKeys::isTypeSupported(const String& keySystem, const String& mimeType)
{
    // 1. If keySystem contains an unrecognized or unsupported Key System, return false and abort these steps.
    // Key system string comparison is case-sensitive.
    if (keySystem.isEmpty() || !LegacyCDM::supportsKeySystem(keySystem))
        return false;

    // 2. If type is null or an empty string, return true and abort these steps.
    if (mimeType.isEmpty())
        return true;

    // 3. If the Key System specified by keySystem does not support decrypting the container and/or codec
    // specified by type, return false and abort these steps.
    if (!LegacyCDM::keySystemSupportsMimeType(keySystem, mimeType))
        return false;

    // 4. Return true;
    return true;
}

void WebKitMediaKeys::setMediaElement(HTMLMediaElement* element)
{
    if (m_mediaElement && m_mediaElement->player())
        m_mediaElement->player()->setCDMSession(nullptr);

    m_mediaElement = makeWeakPtr(element);

    if (m_mediaElement && m_mediaElement->player() && !m_sessions.isEmpty())
        m_mediaElement->player()->setCDMSession(m_sessions.last()->session());
}

RefPtr<MediaPlayer> WebKitMediaKeys::cdmMediaPlayer(const LegacyCDM*) const
{
    if (!m_mediaElement)
        return nullptr;
    return m_mediaElement->player();
}

void WebKitMediaKeys::keyAdded()
{
    if (m_mediaElement)
        m_mediaElement->keyAdded();
}

RefPtr<ArrayBuffer> WebKitMediaKeys::cachedKeyForKeyId(const String& keyId) const
{
    for (auto& session : m_sessions) {
        if (auto key = session->cachedKeyForKeyId(keyId))
            return key;
    }
    return nullptr;
}

}

#endif
