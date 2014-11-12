/*
 * Copyright (C) 2014 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define CAMERA_PARAMETERS_EXTRA_C \
const char CameraParameters::KEY_SUPPORTED_VIDEO_HIGH_FRAME_RATE_MODES[] = "video-hfr-values"; \
const char CameraParameters::KEY_ZSL[] = "zsl"; \
const char CameraParameters::KEY_SUPPORTED_ZSL_MODES[] = "zsl-values"; \
const char CameraParameters::KEY_FACE_DETECTION[] = "face-detection";

#define CAMERA_PARAMETERS_EXTRA_H \
static const char KEY_SUPPORTED_VIDEO_HIGH_FRAME_RATE_MODES[]; \
static const char KEY_ZSL[]; \
static const char KEY_SUPPORTED_ZSL_MODES[]; \
static const char KEY_FACE_DETECTION[];
