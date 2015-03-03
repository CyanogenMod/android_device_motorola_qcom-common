LOCAL_PATH := device/motorola/qcom-common
PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/keylayout/atmel_mxt_ts.kl:system/usr/keylayout/atmel_mxt_ts.kl \
	$(LOCAL_PATH)/keylayout/msm8960-snd-card_Button_Jack.kl:system/usr/keylayout/msm8960-snd-card_Button_Jack.kl \
	$(LOCAL_PATH)/keylayout/cyttsp-i2c.kl:system/usr/keylayout/cyttsp-i2c.kl \
	$(LOCAL_PATH)/keylayout/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl \
	$(LOCAL_PATH)/keylayout/keypad_8960.kl:system/usr/keylayout/keypad_8960.kl \
	$(LOCAL_PATH)/keylayout/keypad_8960_liquid.kl:system/usr/keylayout/keypad_8960_liquid.kl \
	$(LOCAL_PATH)/keylayout/philips_remote_ir.kl:system/usr/keylayout/philips_remote_ir.kl \
	$(LOCAL_PATH)/keylayout/samsung_remote_ir.kl:system/usr/keylayout/samsung_remote_ir.kl \
	$(LOCAL_PATH)/keylayout/ue_rf4ce_remote.kl:system/usr/keylayout/ue_rf4ce_remote.kl
