/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __FFmpegDecoderModule_h__
#define __FFmpegDecoderModule_h__

#include "PlatformDecoderModule.h"
#include "FFmpegAudioDecoder.h"
#include "FFmpegVideoDecoder.h"

namespace mozilla
{

class FFmpegDecoderModule : public PlatformDecoderModule
{
public:
  static already_AddRefed<PlatformDecoderModule>
  Create()
  {
    RefPtr<PlatformDecoderModule> pdm = new FFmpegDecoderModule();

    return pdm.forget();
  }

  explicit FFmpegDecoderModule() {}
  virtual ~FFmpegDecoderModule() {}

  already_AddRefed<MediaDataDecoder>
  CreateVideoDecoder(const CreateDecoderParams& aParams) override
  {
    RefPtr<MediaDataDecoder> decoder =
      new FFmpegVideoDecoder(aParams.mTaskQueue,
                             aParams.mCallback,
                             aParams.VideoConfig(),
                             aParams.mImageContainer);
    return decoder.forget();
  }

  already_AddRefed<MediaDataDecoder>
  CreateAudioDecoder(const CreateDecoderParams& aParams) override
  {
    RefPtr<MediaDataDecoder> decoder =
      new FFmpegAudioDecoder(aParams.mTaskQueue,
                             aParams.mCallback,
                             aParams.AudioConfig());
    return decoder.forget();
  }

  bool SupportsMimeType(const nsACString& aMimeType,
                        DecoderDoctorDiagnostics* aDiagnostics) const override
  {
    AVCodecID videoCodec = FFmpegVideoDecoder::GetCodecId(aMimeType);
    AVCodecID audioCodec = FFmpegAudioDecoder::GetCodecId(aMimeType);
    if (audioCodec == AV_CODEC_ID_NONE && videoCodec == AV_CODEC_ID_NONE) {
      return false;
    }
    AVCodecID codec = audioCodec != AV_CODEC_ID_NONE ? audioCodec : videoCodec;
    return !!FFmpegDataDecoder::FindAVCodec(codec);
  }

  ConversionRequired
  DecoderNeedsConversion(const TrackInfo& aConfig) const override
  {
    if (aConfig.IsVideo() &&
        (aConfig.mMimeType.EqualsLiteral("video/avc") ||
         aConfig.mMimeType.EqualsLiteral("video/mp4"))) {
      return ConversionRequired::kNeedAVCC;
    } else {
      return ConversionRequired::kNeedNone;
    }
  }

private:
};

} // namespace mozilla

#endif // __FFmpegDecoderModule_h__
