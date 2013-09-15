LOCAL_PATH := device/motorola/qcom-common
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/idc/melfas-ts.idc:system/usr/idc/melfas-ts.idc \
    $(LOCAL_PATH)/idc/evfwd.idc:system/usr/idc/evfwd.idc \
    $(LOCAL_PATH)/idc/gpio-keys.idc:system/usr/idc/gpio-keys.idc
