/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "FFmpegRuntimeLinker.h"
#include "FFmpegDecoderModule.h"
#include "mozilla/ArrayUtils.h"
#include "FFmpegLog.h"
#include "prlink.h"

namespace mozilla
{

FFmpegRuntimeLinker::LinkStatus FFmpegRuntimeLinker::sLinkStatus =
  LinkStatus_INIT;
const char* FFmpegRuntimeLinker::sLinkStatusLibraryName = "";

/* static */ already_AddRefed<PlatformDecoderModule>
FFmpegRuntimeLinker::CreateDecoderModule()
{
  RefPtr<PlatformDecoderModule> module;

  module = FFmpegDecoderModule::Create();

  return module.forget();
}

/* static */ const char*
FFmpegRuntimeLinker::LinkStatusString()
{
  switch (sLinkStatus) {
    case LinkStatus_INIT:
      return "Libavcodec not initialized yet";
    case LinkStatus_SUCCEEDED:
      return "Libavcodec linking succeeded";
    case LinkStatus_INVALID_FFMPEG_CANDIDATE:
      return "Invalid FFMpeg libavcodec candidate";
    case LinkStatus_UNUSABLE_LIBAV57:
      return "Unusable LibAV's libavcodec 57";
    case LinkStatus_INVALID_LIBAV_CANDIDATE:
      return "Invalid LibAV libavcodec candidate";
    case LinkStatus_OBSOLETE_FFMPEG:
      return "Obsolete FFMpeg libavcodec candidate";
    case LinkStatus_OBSOLETE_LIBAV:
      return "Obsolete LibAV libavcodec candidate";
    case LinkStatus_INVALID_CANDIDATE:
      return "Invalid libavcodec candidate";
    case LinkStatus_NOT_FOUND:
      return "Libavcodec not found";
  }
  MOZ_ASSERT_UNREACHABLE("Unknown sLinkStatus value");
  return "?";
}

} // namespace mozilla
