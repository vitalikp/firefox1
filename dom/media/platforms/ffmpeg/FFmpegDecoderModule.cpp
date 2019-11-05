/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "FFmpegDecoderModule.h"
#include "FFmpegAudioDecoder.h"
#include "FFmpegVideoDecoder.h"

namespace mozilla {

bool FFmpegDecoderModule::sInitialized = false;

/* static */
void
FFmpegDecoderModule::Init()
{
  if (sInitialized) {
    return;
  }

  avcodec_register_all();
  sInitialized = true;
}

already_AddRefed<MediaDataDecoder>
FFmpegDecoderModule::CreateVideoDecoder(const CreateDecoderParams& aParams)
{
  RefPtr<MediaDataDecoder> decoder =
    new FFmpegVideoDecoder(aParams.mTaskQueue,
                           aParams.mCallback,
                           aParams.VideoConfig(),
                           aParams.mImageContainer);
  return decoder.forget();
}

already_AddRefed<MediaDataDecoder>
FFmpegDecoderModule::CreateAudioDecoder(const CreateDecoderParams& aParams)
{
  RefPtr<MediaDataDecoder> decoder =
    new FFmpegAudioDecoder(aParams.mTaskQueue,
                           aParams.mCallback,
                           aParams.AudioConfig());
  return decoder.forget();
}

bool FFmpegDecoderModule::SupportsMimeType(const nsACString& aMimeType,
                      DecoderDoctorDiagnostics* aDiagnostics) const
{
  AVCodecID videoCodec = FFmpegVideoDecoder::GetCodecId(aMimeType);
  AVCodecID audioCodec = FFmpegAudioDecoder::GetCodecId(aMimeType);
  if (audioCodec == AV_CODEC_ID_NONE && videoCodec == AV_CODEC_ID_NONE) {
    return false;
  }
  AVCodecID codec = audioCodec != AV_CODEC_ID_NONE ? audioCodec : videoCodec;
  return !!FFmpegDataDecoder::FindAVCodec(codec);
}

PlatformDecoderModule::ConversionRequired
FFmpegDecoderModule::DecoderNeedsConversion(const TrackInfo& aConfig) const
{
  if (aConfig.IsVideo() &&
      (aConfig.mMimeType.EqualsLiteral("video/avc") ||
       aConfig.mMimeType.EqualsLiteral("video/mp4"))) {
    return ConversionRequired::kNeedAVCC;
  } else {
    return ConversionRequired::kNeedNone;
  }
}

} // namespace mozilla
